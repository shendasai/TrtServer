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

  Author: Wu Jiaxu (wujiaxu@sogou-inc.com)
*/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <new>
#include <string>
#include <functional>
#include <utility>
#include "WFGlobal.h"
#include "Workflow.h"
#include "WFTask.h"
#include "UpstreamManager.h"
#include "RouteManager.h"
#include "URIParser.h"
#include "WFTaskError.h"
#include "EndpointParams.h"

inline WFTimerTask *WFTaskFactory::create_timer_task(const std::string& name,
													 unsigned int microseconds,
													 timer_callback_t callback)
{
	return new WFTimerTask(microseconds,
						   WFGlobal::get_scheduler(),
						   std::move(callback));
}

inline WFTimerTask *WFTaskFactory::create_timer_task(unsigned int microseconds,
													 timer_callback_t callback)
{
	return new WFTimerTask(microseconds,
						   WFGlobal::get_scheduler(),
						   std::move(callback));
}

template <class FUNC, class... ARGS>
inline WFGoTask *WFTaskFactory::create_go_task(const std::string& queue_name,
											   FUNC func, ARGS... args)
{
	return new WFGoTask(WFGlobal::get_exec_queue(queue_name),
						WFGlobal::get_compute_executor(),
						std::bind(std::move(func), std::forward(args)...));
}

/**********WFComplexNetworkTask**********/

// If you design Derived WFComplexNetworkTask, You have two choices:
// 1) First choice will upstream by uri, then dns/dns-cache
// 2) Second choice will directly communicate without upstream/dns/dns-cache

// 1) First choice:
// step 1. Child-Constructor call Father-Constructor to new WFComplexNetworkTask
// step 2. call init(type, uri)
// step 3. call set_type(type)
// step 4. call set_info(info) or do nothing with info

// 2) Second choice:
// step 1. Child-Constructor call Father-Constructor to new WFComplexNetworkTask
// step 2. call init(type, addr, addrlen, info)

/*
DNS_CACHE_LEVEL_0	->	NO cache
DNS_CACHE_LEVEL_1	->	TTL MIN
DNS_CACHE_LEVEL_2	->	TTL [DEFAULT]
DNS_CACHE_LEVEL_3	->	Forever
*/

#define DNS_CACHE_LEVEL_0		0
#define DNS_CACHE_LEVEL_1		1
#define DNS_CACHE_LEVEL_2		2
#define DNS_CACHE_LEVEL_3		3

class WFRouterTask : public WFGenericTask
{
private:
	using router_callback_t = std::function<void (WFRouterTask *)>;
	using WFDNSTask = WFThreadTask<DNSInput, DNSOutput>;

public:
	RouteManager::RouteResult route_result_;

	WFRouterTask(TransportType type,
				 const std::string& host,
				 unsigned short port,
				 const std::string& info,
				 int dns_cache_level,
				 unsigned int dns_ttl_default,
				 unsigned int dns_ttl_min,
				 const struct EndpointParams *endpoint_params,
				 router_callback_t&& callback) :
		type_(type),
		host_(host),
		port_(port),
		info_(info),
		dns_cache_level_(dns_cache_level),
		dns_ttl_default_(dns_ttl_default),
		dns_ttl_min_(dns_ttl_min),
		endpoint_params_(*endpoint_params),
		callback_(std::move(callback))
	{}

private:
	virtual void dispatch();
	virtual SubTask *done();
	void dns_callback(WFDNSTask *dns_task);
	void dns_callback_internal(DNSOutput *dns_task,
							   unsigned int ttl_default,
							   unsigned int ttl_min);

private:
	TransportType type_;
	std::string host_;
	unsigned short port_;
	std::string info_;
	int dns_cache_level_;
	unsigned int dns_ttl_default_;
	unsigned int dns_ttl_min_;
	struct EndpointParams endpoint_params_;
	router_callback_t callback_;
	bool insert_dns_;
};

template<class REQ, class RESP>
class WFComplexNetworkTask : public WFClientTask<REQ, RESP>
{
protected:
	using __WFClientTask = WFClientTask<REQ, RESP>;
	using wf_callback_t = std::function<void (WFNetworkTask<REQ, RESP> *)>;

public:
	WFComplexNetworkTask(int retry_max, wf_callback_t&& callback):
		__WFClientTask(NULL,
					   WFGlobal::get_scheduler(),
					   std::move(callback)),
		init_succ_(false),
		type_(TT_TCP),
		retry_max_(retry_max),
		retry_time_(0),
		is_retry_(false)
	{}

	void set_info(const std::string& info)
	{
		info_.assign(info);
	}

	void set_info(const char *info)
	{
		info_.assign(info);
	}

	void set_type(TransportType type)
	{
		type_ = type;
	}

	void init(const URI& uri)
	{
		init_succ_ = false;
		uri_ = uri;
		init_with_uri();
	}

	void init(URI&& uri)
	{
		init_succ_ = false;
		uri_ = std::move(uri);
		init_with_uri();
	}

	void init(TransportType type,
			  const struct sockaddr *addr,
			  socklen_t addrlen,
			  const std::string& info);

	const URI *get_uri() const { return &uri_; }

protected:
	virtual void dispatch();
	virtual SubTask *done();
	void clear_resp()
	{
		size_t size = this->resp.get_size_limit();
		this->resp.~RESP();
		new(&this->resp) RESP();
		this->resp.set_size_limit(size);
	}

	void on_success();

	URI uri_;
	RouteManager::RouteResult route_result_;
	bool skip_once_;
	bool init_succ_;

private:
	void init_with_uri();
	void router_callback(WFRouterTask *task);

	UpstreamManager::UpstreamResult upstream_result_;

	TransportType type_;
	std::string info_;

	int retry_max_;
	int retry_time_;
	bool is_retry_;
};

template<class REQ, class RESP>
void WFComplexNetworkTask<REQ, RESP>::init(TransportType type,
										   const struct sockaddr *addr,
										   socklen_t addrlen,
										   const std::string& info)
{
	init_succ_ = false;
	type_ = type;
	info_.assign(info);
	struct addrinfo addrinfo;
	RouteManager::RouteResult result;
	const auto *params = &WFGlobal::get_global_setting()->endpoint_params;

	addrinfo.ai_addrlen = addrlen;
	addrinfo.ai_addr = (struct sockaddr *)addr;
	addrinfo.ai_canonname = NULL;
	addrinfo.ai_next = NULL;
	addrinfo.ai_flags = 0;
	addrinfo.ai_family = addr->sa_family;
	addrinfo.ai_socktype = SOCK_STREAM;
	addrinfo.ai_protocol = 0;
	if (WFGlobal::get_route_manager()->get(type, &addrinfo, info_, params,
										   route_result_) < 0)
	{
		this->state = WFT_STATE_SYS_ERROR;
		this->error = errno;
	}
	else if (!route_result_.request_object)
	{
		//should not happen
		this->state = WFT_STATE_SYS_ERROR;
		this->error = EAGAIN;
	}
	else
		init_succ_ = true;
}

template<class REQ, class RESP>
void WFComplexNetworkTask<REQ, RESP>::init_with_uri()
{
	if (uri_.state == URI_STATE_SUCCESS)
	{
		int port = 0;

		if (uri_.port && uri_.port[0])
		{
			port = atoi(uri_.port);
			if (port < 0 || port > 65535)
			{
				this->state = WFT_STATE_TASK_ERROR;
				this->error = WFT_ERR_URI_PORT_INVALID;
				return;
			}
		}

		if (port == 0 && uri_.scheme)
		{
			const char *port_str = WFGlobal::get_default_port(uri_.scheme);

			if (port_str)
			{
				size_t port_len = strlen(port_str);

				if (uri_.port)
					free(uri_.port);

				uri_.port = (char *)malloc(port_len + 1);
				if (!uri_.port)
				{
					uri_.state = URI_STATE_ERROR;
					uri_.error = errno;
					this->state = WFT_STATE_SYS_ERROR;
					this->error = errno;
					return;
				}

				memcpy(uri_.port, port_str, port_len);
				uri_.port[port_len] = '\0';
			}
		}

		int ret = UpstreamManager::choose(uri_, upstream_result_);

		if (ret < 0)
		{
			this->state = WFT_STATE_SYS_ERROR;
			this->error = errno;
		}
		else if (upstream_result_.state == UPSTREAM_ALL_DOWN)
		{
			this->state = WFT_STATE_TASK_ERROR;
			this->error = WFT_ERR_UPSTREAM_UNAVAILABLE;
		}
		else
			init_succ_ = true;
	}
	else
	{
		if (uri_.state == URI_STATE_ERROR)
		{
			this->state = WFT_STATE_SYS_ERROR;
			this->error = uri_.error;
		}
		else
		{
			this->state = WFT_STATE_TASK_ERROR;
			this->error = WFT_ERR_URI_PARSE_FAILED;
		}
	}
}

template<class REQ, class RESP>
void WFComplexNetworkTask<REQ, RESP>::router_callback(WFRouterTask *task)
{
	int state = task->get_state();

	if (state == WFT_STATE_SUCCESS)
		route_result_ = task->route_result_;
	else
	{
		this->state = state;
		this->error = task->get_error();
	}

	if (route_result_.request_object)
		series_of(this)->push_front(this);
	else
	{
		UpstreamManager::notify_unavailable(upstream_result_.cookie);

		//dns failed, task over, need callback user
		if (this->callback)
			this->callback(this);

		delete this;
	}
}

template<class REQ, class RESP>
void WFComplexNetworkTask<REQ, RESP>::dispatch()
{
	if (!init_succ_)
	{
		skip_once_ = false;
		this->subtask_done();
	}
	else if (route_result_.request_object)
	{
		skip_once_ = false;
		this->set_request_object(route_result_.request_object);
		this->__WFClientTask::dispatch();
	}
	else if (uri_.state != URI_STATE_SUCCESS)
	{
		skip_once_ = false;
		if (uri_.state == URI_STATE_ERROR)
		{
			this->state = WFT_STATE_SYS_ERROR;
			this->error = uri_.error;
		}
		else
		{
			this->state = WFT_STATE_TASK_ERROR;
			this->error = WFT_ERR_URI_PARSE_FAILED;
		}

		this->subtask_done();
	}
	else
	{
		skip_once_ = true;
		unsigned int dns_ttl_default;
		unsigned int dns_ttl_min;
		const struct EndpointParams *endpoint_params;
		int dns_cache_level = (is_retry_ ? DNS_CACHE_LEVEL_1
										 : DNS_CACHE_LEVEL_2);
		auto&& cb = std::bind(&WFComplexNetworkTask::router_callback,
							  this,
							  std::placeholders::_1);

		is_retry_ = false;
		if (upstream_result_.state == UPSTREAM_SUCCESS)
		{
			const auto *params = upstream_result_.address_params;

			dns_ttl_default = params->dns_ttl_default;
			dns_ttl_min = params->dns_ttl_min;
			endpoint_params = &params->endpoint_params;
		}
		else
		{
			const auto *params = WFGlobal::get_global_setting();

			dns_ttl_default = params->dns_ttl_default;
			dns_ttl_min = params->dns_ttl_min;
			endpoint_params = &params->endpoint_params;
		}

		auto *task = new WFRouterTask(type_,
									  uri_.host ? uri_.host : "",
									  uri_.port ? atoi(uri_.port) : 0,
									  info_,
									  dns_cache_level,
									  dns_ttl_default,
									  dns_ttl_min,
									  endpoint_params,
									  std::move(cb));

		series_of(this)->push_front(task);
		this->subtask_done();
	}
}

template<class REQ, class RESP>
void WFComplexNetworkTask<REQ, RESP>::on_success()
{
	RouteManager::notify_available(route_result_.cookie, this->target);
	UpstreamManager::notify_available(upstream_result_.cookie);
	upstream_result_.clear();
}

template<class REQ, class RESP>
SubTask *WFComplexNetworkTask<REQ, RESP>::done()
{
	SeriesWork *series = series_of(this);

	if (skip_once_)
	{
		skip_once_ = false;
		return series->pop();
	}

	if (init_succ_)
	{
		if (this->state == WFT_STATE_SUCCESS)
			this->on_success();
		else if (this->state == WFT_STATE_SYS_ERROR)
		{
			RouteManager::notify_unavailable(route_result_.cookie, this->target);
			UpstreamManager::notify_unavailable(upstream_result_.cookie);
			if (retry_time_ < retry_max_)
			{
				this->clear_resp();
				this->set_request_object(NULL);
				route_result_.clear();
				is_retry_ = true;
				retry_time_++;
				return this;
			}
		}
	}

	return this->__WFClientTask::done();
}

/**********Template Network Factory**********/

template<class REQ, class RESP>
WFNetworkTask<REQ, RESP> *
WFNetworkTaskFactory<REQ, RESP>::create_client_task(TransportType type,
													const std::string& host,
													unsigned short port,
													int retry_max,
													std::function<void (WFNetworkTask<REQ, RESP> *)> callback)
{
	auto *task = new WFComplexNetworkTask<REQ, RESP>(retry_max, std::move(callback));
	char buf[8];
	std::string url = "scheme://";
	URI uri;

	sprintf(buf, "%u", port);
	url += host;
	url += ":";
	url += buf;
	URIParser::parse(url, uri);
	task->init(std::move(uri));
	task->set_type(type);
	return task;
}

template<class REQ, class RESP>
WFNetworkTask<REQ, RESP> *
WFNetworkTaskFactory<REQ, RESP>::create_client_task(TransportType type,
													const std::string& url,
													int retry_max,
													std::function<void (WFNetworkTask<REQ, RESP> *)> callback)
{
	auto *task = new WFComplexNetworkTask<REQ, RESP>(retry_max, std::move(callback));
	URI uri;

	URIParser::parse(url, uri);
	task->init(std::move(uri));
	task->set_type(type);
	return task;
}

template<class REQ, class RESP>
WFNetworkTask<REQ, RESP> *
WFNetworkTaskFactory<REQ, RESP>::create_client_task(TransportType type,
													const URI& uri,
													int retry_max,
													std::function<void (WFNetworkTask<REQ, RESP> *)> callback)
{
	auto *task = new WFComplexNetworkTask<REQ, RESP>(retry_max, std::move(callback));

	task->init(uri);
	task->set_type(type);
	return task;
}

template<class REQ, class RESP>
WFNetworkTask<REQ, RESP> *
WFNetworkTaskFactory<REQ, RESP>::create_server_task(std::function<void (WFNetworkTask<REQ, RESP> *)>& process)
{
	return new WFServerTask<REQ, RESP>(WFGlobal::get_scheduler(), process);
}

/**********Server Factory**********/

class WFServerTaskFactory
{
public:
	static WFHttpTask *create_http_task(std::function<void (WFHttpTask *)>& process);
};

/**********Template Network Factory Sepcial**********/

template<>
inline WFHttpTask *
WFNetworkTaskFactory<HttpRequest, HttpResponse>::create_server_task(std::function<void (WFHttpTask *)>& process)
{
	return WFServerTaskFactory::create_http_task(process);
}

/**********Template Thread Task Factory**********/

template<class INPUT, class OUTPUT>
class __WFThreadTask : public WFThreadTask<INPUT, OUTPUT>
{
protected:
	virtual void execute()
	{
		this->routine(&this->input, &this->output);
	}

protected:
	std::function<void (INPUT *, OUTPUT *)> routine;

public:
	__WFThreadTask(ExecQueue *queue, Executor *executor,
				   std::function<void (INPUT *, OUTPUT *)>&& rt,
				   std::function<void (WFThreadTask<INPUT, OUTPUT> *)>&& cb) :
		WFThreadTask<INPUT, OUTPUT>(queue, executor, std::move(cb)),
		routine(std::move(rt))
	{
	}
};

template<class INPUT, class OUTPUT>
WFThreadTask<INPUT, OUTPUT> *
WFThreadTaskFactory<INPUT, OUTPUT>::create_thread_task(const std::string& queue_name,
						std::function<void (INPUT *, OUTPUT *)> routine,
						std::function<void (WFThreadTask<INPUT, OUTPUT> *)> callback)
{
	return new __WFThreadTask<INPUT, OUTPUT>(WFGlobal::get_exec_queue(queue_name),
											 WFGlobal::get_compute_executor(),
											 std::move(routine),
											 std::move(callback));
}

template<class INPUT, class OUTPUT>
WFThreadTask<INPUT, OUTPUT> *
WFThreadTaskFactory<INPUT, OUTPUT>::create_thread_task(ExecQueue *queue, Executor *executor,
						std::function<void (INPUT *, OUTPUT *)> routine,
						std::function<void (WFThreadTask<INPUT, OUTPUT> *)> callback)
{
	return new __WFThreadTask<INPUT, OUTPUT>(queue, executor,
											 std::move(routine),
											 std::move(callback));
}

template<class INPUT, class OUTPUT>
class __WFThreadTask__ : public __WFThreadTask<INPUT, OUTPUT>
{
private:
	virtual SubTask *done() { return NULL; }

public:
	using __WFThreadTask<INPUT, OUTPUT>::__WFThreadTask;
};

template<class INPUT, class OUTPUT>
WFMultiThreadTask<INPUT, OUTPUT> *
WFThreadTaskFactory<INPUT, OUTPUT>::create_multi_thread_task(const std::string& queue_name,
						std::function<void (INPUT *, OUTPUT *)> routine, size_t nthreads,
						std::function<void (WFMultiThreadTask<INPUT, OUTPUT> *)> callback)
{
	WFThreadTask<INPUT, OUTPUT> **tasks = new WFThreadTask<INPUT, OUTPUT> *[nthreads];
	char buf[32];
	size_t i;

	for (i = 0; i < nthreads; i++)
	{
		sprintf(buf, "-%zu@MTT", i);
		tasks[i] = new __WFThreadTask__<INPUT, OUTPUT>
							(WFGlobal::get_exec_queue(queue_name + buf),
							 WFGlobal::get_compute_executor(),
							 std::function<void (INPUT *, OUTPUT *)>(routine),
							 nullptr);
	}

	auto *mt = new WFMultiThreadTask<INPUT, OUTPUT>(tasks, nthreads, std::move(callback));
	delete []tasks;
	return mt;
}

