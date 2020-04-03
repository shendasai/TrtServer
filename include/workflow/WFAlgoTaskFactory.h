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

  Author: Xie Han (xiehan@sogou-inc.com)
*/

#ifndef _WFALGOTASKFACTORY_H_
#define _WFALGOTASKFACTORY_H_

#include "WFTask.h"

/* Sort */
template <typename T>
struct SortInput
{
	T *first;
	T *last;
};

template <typename T>
using SortOutput = SortInput<T>;

template <typename T>
using WFSortTask = WFThreadTask<SortInput<T>, SortOutput<T>>;

template <typename T>
using sort_callback_t = std::function<void (WFSortTask<T> *)>;

/* Merge */
template <typename T>
struct MergeInput
{
	T *first1;
	T *last1;
	T *first2;
	T *last2;
	T *d_first;
};

template <typename T>
struct MergeOutput
{
	T *first;
	T *last;
};

template <typename T>
using WFMergeTask = WFThreadTask<MergeInput<T>, MergeOutput<T>>;

template <typename T>
using merge_callback_t = std::function<void (WFMergeTask<T> *)>;

class WFAlgoTaskFactory
{
public:
	template <typename T, class CB = sort_callback_t<T>>
	static WFSortTask<T> *create_sort_task(const std::string& queue_name,
										   T *first, T *last,
										   CB callback);

	template <typename T, class CMP, class CB = sort_callback_t<T>>
	static WFSortTask<T> *create_sort_task(const std::string& queue_name,
										   T *first, T *last,
										   CMP compare,
										   CB callback);

	template <typename T, class CB = merge_callback_t<T>>
	static WFMergeTask<T> *create_merge_task(const std::string& queue_name,
											 T *first1, T *last1,
											 T *first2, T *last2,
											 T *d_first,
											 CB callback);

	template <typename T, class CMP, class CB = merge_callback_t<T>>
	static WFMergeTask<T> *create_merge_task(const std::string& queue_name,
											 T *first1, T *last1,
											 T *first2, T *last2,
											 T *d_first,
											 CMP compare,
											 CB callback);

	template <typename T, class CB = sort_callback_t<T>>
	static WFSortTask<T> *create_psort_task(const std::string& queue_name,
											T *first, T *last,
											CB callback);

	template <typename T, class CMP, class CB = sort_callback_t<T>>
	static WFSortTask<T> *create_psort_task(const std::string& queue_name,
											T *first, T *last,
											CMP compare,
											CB callback);
};

#include "WFAlgoTaskFactory.inl"

#endif

