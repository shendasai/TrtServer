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

#ifndef _WFSERVER_H_
#define _WFSERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "CommScheduler.h"
#include "WFTaskFactory.h"

struct WFServerParams
{
	size_t max_connections;
	int peer_response_timeout;	/* timeout of each read or write operation */
	int receive_timeout;	/* timeout of receiving the whole message */
	int keep_alive_timeout;
	size_t request_size_limit;
	int ssl_accept_timeout;	/* if not ssl, this will be ignored */
	int listen_fd; /* -1 mean first start, other mean graceful restart */
};

const struct WFServerParams SERVER_PARAMS_DEFAULT =
{
	.max_connections		=	2000,
	.peer_response_timeout	=	10 * 1000,
	.receive_timeout		=	-1,
	.keep_alive_timeout		=	60 * 1000,
	.request_size_limit		=	(size_t)-1,
	.ssl_accept_timeout		=	10 * 1000,
	.listen_fd 				=	 -1
};

class WFServerBase : public CommService
{
public:
	WFServerBase(const struct WFServerParams *params):
		params_(*params),
		conn_count_(0),
		unbind_finish_(false)
	{}

public:
	int start(const struct sockaddr *bind_addr, socklen_t addrlen);
	int start(const struct sockaddr *bind_addr, socklen_t addrlen,
			  const char *cert_file, const char *key_file);

	int start(const char *host, unsigned short port);
	int start(const char *host, unsigned short port,
			  const char *cert_file, const char *key_file);

	int start(unsigned short port)
	{
		return start(NULL, port);
	}

	int start(unsigned short port, const char *cert_file, const char *key_file)
	{
		return start(NULL, port, cert_file, key_file);
	}

	void stop();

public:
	int get_listen_fd() const { return params_.listen_fd; }
	size_t get_conn_count() const { return conn_count_.load(); }

protected:
	WFServerParams params_;

private:
	int init(const struct sockaddr *bind_addr, socklen_t addrlen,
			 const char *cert_file, const char *key_file);
	virtual int create_listen_fd();
	virtual CommConnection *new_connection(int accept_fd);
	virtual void handle_unbound();

private:
	std::atomic<size_t> conn_count_;
	std::mutex mutex_;
	std::condition_variable cond_;

	bool unbind_finish_;

	CommScheduler *scheduler_;
};

template<class REQ, class RESP>
class WFServer : public WFServerBase
{
public:
	WFServer(const struct WFServerParams *params,
			 std::function<void (WFNetworkTask<REQ, RESP> *)> process):
		WFServerBase(params),
		process_(std::move(process))
	{}

protected:
	virtual CommSession *new_session(long long seq, CommConnection *conn);

protected:
	std::function<void (WFNetworkTask<REQ, RESP> *)> process_;
};

template<class REQ, class RESP>
CommSession *WFServer<REQ, RESP>::new_session(long long, CommConnection *)
{
	auto *task = WFNetworkTaskFactory<REQ, RESP>::create_server_task(process_);

	task->set_keep_alive(params_.keep_alive_timeout);
	task->set_receive_timeout(params_.receive_timeout);
	task->get_req()->set_size_limit(params_.request_size_limit);
	return task;
}

#endif

