/*
  Copyright (c) 2019 Sogou, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Authors: Wu Jiaxu (wujiaxu@sogou-inc.com)
           Xie Han (xiehan@sogou-inc.com)
*/

#ifndef _WFTASKFACTORY_H_
#define _WFTASKFACTORY_H_

#include <functional>
#include "URIParser.h"
#include "RedisMessage.h"
#include "HttpMessage.h"
#include "MySQLMessage.h"
#include "DNSRoutine.h"
#include "WFTask.h"
#include "Workflow.h"
#include "EndpointParams.h"
#include "WFAlgoTaskFactory.h"

// Network Client tasks
typedef WFNetworkTask<HttpRequest, HttpResponse> WFHttpTask;
typedef std::function<void (WFHttpTask *)> http_callback_t;

typedef WFNetworkTask<RedisRequest, RedisResponse> WFRedisTask;
typedef std::function<void (WFRedisTask *)> redis_callback_t;

typedef WFNetworkTask<MySQLRequest, MySQLResponse> WFMySQLTask;
typedef std::function<void (WFMySQLTask *)> mysql_callback_t;

// Timer task
typedef std::function<void (WFTimerTask *)> timer_callback_t;

// Counter task
typedef std::function<void (WFCounterTask *)> counter_callback_t;

// File IO tasks
struct FileIOArg
{
	int fd;
	void *buf;
	size_t count;
	off_t offset;
};

struct FileVIOArg
{
	int fd;
	const struct iovec *iovec;
	int iovcnt;
	off_t offset;
};

typedef WFFileTask<FileIOArg> WFFileIOTask;
typedef std::function<void (WFFileIOTask *)> fio_callback_t;

typedef WFFileTask<FileVIOArg> WFFileVIOTask;
typedef std::function<void (WFFileVIOTask *)> fvio_callback_t;

// DNS task
typedef WFThreadTask<DNSInput, DNSOutput> WFDNSTask;
typedef std::function<void (WFDNSTask *)> dns_callback_t;

typedef WFGenericTask WFEmptyTask;

class WFTaskFactory
{
public:
	static WFHttpTask *create_http_task(const std::string& url,
										int redirect_max,
										int retry_max,
										http_callback_t callback);

	static WFHttpTask *create_http_task(const URI& uri,
										int redirect_max,
										int retry_max,
										http_callback_t callback);

	static WFRedisTask *create_redis_task(const std::string& url,
										  int retry_max,
										  redis_callback_t callback);

	static WFRedisTask *create_redis_task(const URI& uri,
										  int retry_max,
										  redis_callback_t callback);

	static WFMySQLTask *create_mysql_task(const std::string& url,
										  int retry_max,
										  mysql_callback_t callback);

	static WFMySQLTask *create_mysql_task(const URI& uri,
										  int retry_max,
										  mysql_callback_t callback);

public:
	/* timer_name has no use currently. */
	static WFTimerTask *create_timer_task(const std::string& timer_name,
										  unsigned int microseconds,
										  timer_callback_t callback);

	static WFTimerTask *create_timer_task(unsigned int microseconds,
										  timer_callback_t callback);

	/* It's recommended to create a named counter, and use count_by_name(). */
	static WFCounterTask *create_counter_task(const std::string& counter_name,
											  unsigned int target_value,
											  counter_callback_t callback);

	/* No matter how you create a counter task, with or without name,
	 * count operation CAN be performed before the task is started. */

	/* If you create named counter, the only way to count is count_by_name().
	 * You can count exceeding target_value safely if the name is unique.
	 * NOTE: counter task must be created before.
	 * NOTE2: Never use task->count() with a named counter task. */
	static void count_by_name(const std::string& counter_name);

	/* Create unnamed counter task. Call task->count() directly,
	 * and make sure never count exceeding target_value.
	 * Unnamed counter is useful for neural network and other DAGs. */
	static WFCounterTask *create_counter_task(unsigned int target_value,
											  counter_callback_t callback)
	{
		return new WFCounterTask(target_value, std::move(callback));
	}

public:
	static WFFileIOTask *create_pread_task(int fd,
										   void *buf,
										   size_t count,
										   off_t offset,
										   fio_callback_t callback);

	static WFFileIOTask *create_pwrite_task(int fd,
											void *buf,
											size_t count,
											off_t offset,
											fio_callback_t callback);

	static WFFileVIOTask *create_preadv_task(int fd,
											 const struct iovec *iovec,
											 int iovcnt,
											 off_t offset,
											 fvio_callback_t callback);

	static WFFileVIOTask *create_pwritev_task(int fd,
											  const struct iovec *iovec,
											  int iovcnt,
											  off_t offset,
											  fvio_callback_t callback);

public:
	template <class FUNC, class... ARGS>
	static WFGoTask *create_go_task(const std::string& queue_name,
									FUNC func, ARGS... args);

public:
	// if use this dns task, NOTICE:
	//   1. Result will free by freeaddrinfo after task callback
	//   2. This task does NOT read/write/affect Framework-DNSCache
	static WFDNSTask *create_dns_task(const std::string& host,
									  unsigned short port,
									  dns_callback_t callback);

public:
	static WFEmptyTask *create_empty_task()
	{
		return new WFEmptyTask;
	}
};

template <class REQ, class RESP>
class WFNetworkTaskFactory
{
private:
	using T = WFNetworkTask<REQ, RESP>;

public:
	static T *create_client_task(TransportType type,
								 const std::string& host,
								 unsigned short port,
								 int retry_max,
								 std::function<void (T *)> callback);

	static T *create_client_task(TransportType type,
								 const std::string& url,
								 int retry_max,
								 std::function<void (T *)> callback);

	static T *create_client_task(TransportType type,
								 const URI& uri,
								 int retry_max,
								 std::function<void (T *)> callback);

public:
	/* For WFServer. Don't call directly. */
	static T *create_server_task(std::function<void (T *)>& process);
};

template <class INPUT, class OUTPUT>
class WFThreadTaskFactory
{
private:
	using T = WFThreadTask<INPUT, OUTPUT>;
	using MT = WFMultiThreadTask<INPUT, OUTPUT>;

public:
	static T *create_thread_task(const std::string& queue_name,
								 std::function<void (INPUT *, OUTPUT *)> routine,
								 std::function<void (T *)> callback);

	static MT *create_multi_thread_task(const std::string& queue_name,
										std::function<void (INPUT *, OUTPUT *)> routine,
										size_t nthreads,
										std::function<void (MT *)> callback);

public:
	static T *create_thread_task(ExecQueue *queue, Executor *executor,
								 std::function<void (INPUT *, OUTPUT *)> routine,
								 std::function<void (T *)> callback);
};

//impl. not for user
#include "WFTaskFactory.inl"

#endif

