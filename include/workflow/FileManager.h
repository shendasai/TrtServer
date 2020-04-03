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

#ifndef _FILEMANAGER_H_
#define _FILEMANAGER_H_

#include <mutex>
#include <string>
#include <map>
#include "rbtree.h"
#include "EndpointParams.h"

class FileManager
{
public:
	FileManager()
	{
		cache_.rb_node = NULL;
	}

	~FileManager();

	//flags -- O_RDONLY, O_WRONLY, O_RDWR, O_DIRECT
	int get_fd(const std::string& file_path, int flags);
	void put_fd(int fd);

private:
	std::mutex mutex_;
	struct rb_root cache_;
	std::map<int, void *> fd_map_;
};

#endif

