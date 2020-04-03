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

#ifndef _UPSTREAM_MANAGER_H_
#define _UPSTREAM_MANAGER_H_
#include <string>
#include <vector>
#include <functional>
#include "URIParser.h"
#include "EndpointParams.h"

// 1: path 2: query 3: fragment
// path/query/fragment is empty string when uri not contain that region
// path/query/fragment would never be NULL
using upstream_route_t = std::function<unsigned int (const char *, const char *, const char *)>;

// try_another
// -- If one server reach max_fails, whether or not try another
//    default is false, you can call upstream_set_attr to change it
// rehash
// -- when try_another is true and upstream is NOT wighted random
//    when upstream select server is down, upstream will use this to try another
//    you need to know we use Consistent-Hash algorithm for rehash
//    so rehash need to return value in 0~2^31-1 Balance/Monotonicity/Spread/Smoothness
struct UpstreamAttr
{
	bool try_another;
	upstream_route_t rehash;
};

static const struct UpstreamAttr UPSTREAM_ATTR_DEFAULT =
{
	.try_another	=	false,
	.rehash			=	nullptr,
};

// weight
// -- [1, 65535] weight = 0 means weight = 1
// max_fails
// -- [1, INT32_MAX] max_fails = 0 means max_fails = 1
// The max_fails directive sets the number of consecutive unsuccessful attempts to communicate with the server.
// After 30s following the server failure, upstream probe the server with some live client’s requests.
// If the probes have been successful, the server is marked as a live one.
// If max_fails is set to 1, it means server would out of upstream selection in 30 seconds when failed only once
struct AddressParams
{
	struct EndpointParams endpoint_params;
	unsigned int dns_ttl_default;
	unsigned int dns_ttl_min;
	unsigned int max_fails;
	unsigned short weight;
#define SERVER_TYPE_MASTER		0
#define SERVER_TYPE_SLAVE		1
	int server_type;
	int group_id;
};

static const struct AddressParams ADDRESS_PARAMS_DEFAULT =
{
	.endpoint_params	=	ENDPOINT_PARAMS_DEFAULT,
	.dns_ttl_default	=	12 * 3600,
	.dns_ttl_min		=	180,
	.max_fails			=	200,
	.weight				=	1,	//only for master of UPSTREAM_WEIGHTED_RANDOM
	.server_type		=	SERVER_TYPE_MASTER,
	.group_id			=	-1,
};

/*
upstream_create("abc.sogou", NULL);                           //UPSTREAM_WEIGHTED_RANDOM
upstream_create("abc.sogou", [](const char *path,             //UPSTREAM_SELECT
                                const char *query,
                                const char *fragment) -> int {
                                   return select_xxx(...));
                               });
upstream_add_server("abc.sogou", "192.168.2.100:8081");       //weight=1, max_fails=200
upstream_add_server("abc.sogou", "192.168.2.100:9090");       //weight=1, max_fails=200
AddressParams params = ADDRESS_PARAMS_DEFAULT;
params.weight = 2;
params.max_fails = 6;
upstream_add_server("abc.sogou", "www.sogou.com", &params);   //weight=2, max_fails=6
UpstreamAttr attr = UPSTREAM_ATTR_DEFAULT;
attr.try_another = false;
attr.rehash = some_consistent_hash_function;
upstream_set_attr("abc.sogou", &attr);

//send request with url like http://abc.sogou/somepath/somerequest
*/
class UpstreamManager
{
public:
	// hash_function == NULL means UPSTREAM_WEIGHTED_RANDOM
	// or UPSTREAM_SELECT with select for each request
	// if you want consistent-hash upstream, then:
	//    1. set select = your_consistent_hash_function
	//    2. set attr.rehash = your_consistent_hash_function
	//    3. set attr.try_another = true
	static int upstream_create(const std::string& name,
							   upstream_route_t select);
	static int upstream_delete(const std::string& name);
	static int upstream_set_attr(const std::string& name,
								 const UpstreamAttr *attr);

	static int upstream_add_server(const std::string& name,
								   const std::string& address);
	static int upstream_add_server(const std::string& name,
								   const std::string& address,
								   const struct AddressParams *address_params);
	static int upstream_remove_server(const std::string& name,
									  const std::string& address);

	//static std::vector<std::string> upstream_get_server_addresses(const std::string& name);

public:
	//used by framwork only
	class UpstreamResult
	{
public:
		void *cookie;
		const struct AddressParams *address_params;
#define UPSTREAM_SUCCESS	0
#define UPSTREAM_NOTFOUND	1
#define UPSTREAM_ALL_DOWN	2
		int state;

public:
		UpstreamResult():
			cookie(NULL),
			address_params(NULL),
			state(UPSTREAM_NOTFOUND)
		{}

		void clear()
		{
			cookie = NULL;
			address_params = NULL;
			state = UPSTREAM_NOTFOUND;
		}
	};

	static int choose(URI& uri, UpstreamResult& result);
	static void notify_unavailable(void *cookie);
	static void notify_available(void *cookie);
};

#endif

