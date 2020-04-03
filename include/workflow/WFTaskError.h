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

#ifndef _WFTASKERROR_H_
#define _WFTASKERROR_H_

enum
{
	//COMMON
	WFT_ERR_URI_PARSE_FAILED = 1001,
	WFT_ERR_URI_SCHEME_INVALID = 1002,
	WFT_ERR_URI_PORT_INVALID = 1003,
	WFT_ERR_UPSTREAM_UNAVAILABLE = 1004,

	//HTTP
	WFT_ERR_HTTP_BAD_REDIRECT_HEADER = 2001,

	//REDIS
	WFT_ERR_REDIS_ACCESS_DENIED = 3001,
	WFT_ERR_REDIS_DISABLED_COMMAND = 3002,

	//MYSQL
	WFT_ERR_MYSQL_ACCESS_DENIED = 4001,
	WFT_ERR_MYSQL_DISABLED_COMMAND = 4002,
	WFT_ERR_MYSQL_INVALID_CHARACTER_SET = 4003,
};

#endif

