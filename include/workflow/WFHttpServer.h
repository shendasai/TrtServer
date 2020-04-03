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

#ifndef _WFHTTPSERVER_H_
#define _WFHTTPSERVER_H_

#include "HttpMessage.h"
#include "WFServer.h"
#include "WFTaskFactory.h"

const struct WFServerParams HTTP_SERVER_PARAMS_DEFAULT =
{
	.max_connections		=	2000,
	.peer_response_timeout	=	10 * 1000,
	.receive_timeout		=	-1,
	.keep_alive_timeout		=	60 * 1000,
	.request_size_limit		=	(size_t)-1,
	.ssl_accept_timeout		=	10 * 1000,
	.listen_fd				=	-1
};

typedef std::function<void (WFHttpTask *)> http_process_t;

class WFHttpServer : public WFServer<HttpRequest, HttpResponse>
{
public:
	WFHttpServer(http_process_t process):
		WFServer<HttpRequest, HttpResponse>(&HTTP_SERVER_PARAMS_DEFAULT,
											std::move(process))
	{}

	WFHttpServer(const WFServerParams *params, http_process_t process):
		WFServer<HttpRequest, HttpResponse>(params, std::move(process))
	{}
};

#endif

