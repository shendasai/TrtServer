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

#ifndef _URIPARSER_H_
#define _URIPARSER_H_

#include <string>
#include <vector>
#include <map>
#include <string.h>

#define URI_STATE_INIT		0
#define URI_STATE_SUCCESS	1
#define URI_STATE_INVALID	2
#define URI_STATE_ERROR		3

// RAII: YES
class URI
{
public:
	char *scheme;
	char *userinfo;
	char *host;
	char *port;
	char *path;
	char *query;
	char *fragment;
	int state;
	int error;

	URI() { init(); }
	virtual ~URI() { deinit(); }

	//copy constructor
	URI(const URI& copy);
	//copy operator
	URI& operator= (const URI& copy);
	//move constructor
	URI(URI&& move);
	//move operator
	URI& operator= (URI&& move);

private:
	void init();
	void deinit();
};

// static class
class URIParser
{
public:
	// return 0 mean succ, -1 mean fail
	static int parse(const char *str, URI& uri);
	static int parse(const std::string& str, URI& uri);

	static std::map<std::string, std::vector<std::string>>
	split_query_strict(const std::string &query);

	static std::map<std::string, std::string>
	split_query(const std::string &query);

	static std::vector<std::string> split_path(const std::string &path);
};

////////////////////

inline void URI::init()
{
	scheme = NULL;
	userinfo = NULL;
	host = NULL;
	port = NULL;
	path = NULL;
	query = NULL;
	fragment = NULL;
	state = URI_STATE_INIT;
	error = 0;
}

inline URI::URI(const URI& copy)
{
	init();
	this->operator= (copy);
}

inline URI::URI(URI&& move)
{
	init();
	this->operator= (std::move(move));
}

inline int URIParser::parse(const std::string& str, URI& uri)
{
	return parse(str.c_str(), uri);
}

#endif

