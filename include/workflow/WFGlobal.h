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
*/

#ifndef _WFGLOBAL_H_
#define _WFGLOBAL_H_
#include <openssl/ssl.h>
#include <string>
#include "CommScheduler.h"
#include "DNSCache.h"
#include "RouteManager.h"
#include "Executor.h"
#include "EndpointParams.h"

struct WFGlobalSetting
{
	struct EndpointParams endpoint_params;
	unsigned int dns_ttl_default;
	unsigned int dns_ttl_min;
	int dns_threads;
	int poller_threads;
	int handler_threads;
	int compute_threads;
};

static const struct WFGlobalSetting GLOBAL_SETTING_DEFAULT =
{
	.endpoint_params	=	ENDPOINT_PARAMS_DEFAULT,
	.dns_ttl_default	=	12 * 3600,	// in seconds
	.dns_ttl_min		=	180,		// reacquire when communication error
	.dns_threads		=	8,
	.poller_threads		=	2,
	.handler_threads	=	20,
	.compute_threads	=	-1			// <=0 means auto-set by sys-cpu number
};

extern void WORKFLOW_library_init(const struct WFGlobalSetting *setting);

class WFGlobal
{
public:
	static void register_scheme_port(const std::string& scheme, unsigned short port);
	static const char *get_default_port(const std::string& scheme);
	static const struct WFGlobalSetting *get_global_setting();

public:
	//used by framwork only
	static CommScheduler *get_scheduler();
	static DNSCache *get_dns_cache();
	static RouteManager *get_route_manager();
	static SSL_CTX *get_ssl_client_ctx();
	static SSL_CTX *get_ssl_server_ctx();
	static ExecQueue *get_exec_queue(const std::string& queue_name);
	static Executor *get_compute_executor();
	static IOService *get_io_service();
};

#endif

