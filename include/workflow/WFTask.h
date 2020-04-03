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

#ifndef _WFTASK_H_
#define _WFTASK_H_

#include <errno.h>
#include <string.h>
#include <assert.h>
#include <utility>
#include <functional>
#include "Executor.h"
#include "ExecRequest.h"
#include "Communicator.h"
#include "CommScheduler.h"
#include "CommRequest.h"
#include "SleepRequest.h"
#include "IORequest.h"
#include "Workflow.h"

enum
{
	WFT_STATE_UNDEFINED = -1,
	WFT_STATE_SUCCESS = CS_STATE_SUCCESS,
	WFT_STATE_TOREPLY = CS_STATE_TOREPLY,		/* for server task only */
	WFT_STATE_NOREPLY = CS_STATE_TOREPLY + 1,	/* for server task only */
	WFT_STATE_SYS_ERROR = CS_STATE_ERROR,
	WFT_STATE_SSL_ERROR = 65,
	WFT_STATE_DNS_ERROR = 66,					/* for client task only */
	WFT_STATE_TASK_ERROR = 67
};

template<class INPUT, class OUTPUT>
class WFThreadTask : public ExecRequest
{
public:
	void start()
	{
		assert(!series_of(this));
		Workflow::start_series_work(this, nullptr);
	}

	void dismiss()
	{
		assert(!series_of(this));
		delete this;
	}

public:
	INPUT *get_input() { return &this->input; }
	OUTPUT *get_output() { return &this->output; }

public:
	void *user_data;

public:
	int get_state() const { return this->state; }
	int get_error() const { return this->error; }

public:
	void set_callback(std::function<void (WFThreadTask<INPUT, OUTPUT> *)> cb)
	{
		this->callback = std::move(cb);
	}

protected:
	virtual SubTask *done()
	{
		SeriesWork *series = series_of(this);

		if (this->callback)
			this->callback(this);

		delete this;
		return series->pop();
	}

protected:
	INPUT input;
	OUTPUT output;
	std::function<void (WFThreadTask<INPUT, OUTPUT> *)> callback;

public:
	WFThreadTask(ExecQueue *queue, Executor *executor,
				 std::function<void (WFThreadTask<INPUT, OUTPUT> *)>&& cb) :
		ExecRequest(queue, executor),
		callback(std::move(cb))
	{
		this->user_data = NULL;
		this->state = WFT_STATE_UNDEFINED;
		this->error = 0;
	}

protected:
	virtual ~WFThreadTask() { }
};

template<class INPUT, class OUTPUT>
class WFMultiThreadTask : public ParallelTask
{
public:
	void start()
	{
		assert(!series_of(this));
		Workflow::start_series_work(this, nullptr);
	}

	void dismiss()
	{
		assert(!series_of(this));
		delete this;
	}

public:
	INPUT *get_input(size_t index)
	{
		return static_cast<Thread *>(this->subtasks[index])->get_input();
	}

	OUTPUT *get_output(size_t index)
	{
		return static_cast<Thread *>(this->subtasks[index])->get_output();
	}

public:
	void *user_data;

public:
	int get_state(size_t index) const
	{
		return static_cast<const Thread *>(this->subtasks[index])->get_state();
	}

	int get_error(size_t index) const
	{
		return static_cast<const Thread *>(this->subtasks[index])->get_error();
	}

public:
	void set_callback(
		std::function<void (WFMultiThreadTask<INPUT, OUTPUT> *)> cb)
	{
		this->callback = std::move(cb);
	}

protected:
	virtual SubTask *done()
	{
		SeriesWork *series = series_of(this);

		if (this->callback)
			this->callback(this);

		delete this;
		return series->pop();
	}

protected:
	std::function<void (WFMultiThreadTask<INPUT, OUTPUT> *)> callback;

protected:
	using Thread = WFThreadTask<INPUT, OUTPUT>;

public:
	WFMultiThreadTask(Thread *const tasks[], size_t n,
			std::function<void (WFMultiThreadTask<INPUT, OUTPUT> *)>&& cb) :
		ParallelTask(new SubTask *[n], n),
		callback(std::move(cb))
	{
		size_t i;

		for (i = 0; i < n; i++)
			this->subtasks[i] = tasks[i];

		this->user_data = NULL;
	}

protected:
	virtual ~WFMultiThreadTask()
	{
		size_t n = this->subtasks_nr;

		while (n > 0)
			delete this->subtasks[--n];

		delete []this->subtasks;
	}
};

template<class REQ, class RESP>
class WFNetworkTask : public CommRequest
{
public:
	/* start(), dismiss() for client task only. */
	void start()
	{
		assert(!series_of(this));
		Workflow::start_series_work(this, nullptr);
	}

	void dismiss()
	{
		assert(!series_of(this));
		delete this;
	}

	/* noreply() for server task only. */
	void noreply()
	{
		if (this->state == WFT_STATE_TOREPLY)
			this->state = WFT_STATE_NOREPLY;
	}

public:
	REQ *get_req() { return &this->req; }
	RESP *get_resp() { return &this->resp; }

public:
	void *user_data;

public:
	int get_state() const { return this->state; }
	int get_error() const { return this->error; }

	/* Call when error is ETIMEDOUT, return values:
	 * TOR_NOT_TIMEOUT, TOR_WAIT_TIMEOUT, TOR_CONNECT_TIMEOUT,
	 * TOR_TRANSMIT_TIMEOUT (send or receive).
	 * SSL connect timeout also returns TOR_CONNECT_TIMEOUT. */
	int get_timeout_reason() const { return this->timeout_reason; }

	/* Call only in callback or server's process. */
	long long get_task_seq() const
	{
		if (!this->target)
		{
			errno = ENOTCONN;
			return -1;
		}

		return this->get_seq();
	}

	int get_peer_addr(struct sockaddr *addr, socklen_t *addrlen) const;

public:
	/* All in milleseconds. timeout == -1 for unlimited. */
	void set_send_timeout(int timeout) { this->send_timeo = timeout; }
	void set_receive_timeout(int timeout) { this->receive_timeo = timeout; }
	void set_keep_alive(int timeout) { this->keep_alive_timeo = timeout; }

public:
	void set_callback(std::function<void (WFNetworkTask<REQ, RESP> *)> cb)
	{
		this->callback = std::move(cb);
	}

protected:
	virtual int send_timeout() { return this->send_timeo; }
	virtual int receive_timeout() { return this->receive_timeo; }
	virtual int keep_alive_timeout() { return this->keep_alive_timeo; }

protected:
	virtual SubTask *done()
	{
		SeriesWork *series = series_of(this);

		if (this->state == WFT_STATE_SYS_ERROR && this->error < 0)
		{
			this->state = WFT_STATE_SSL_ERROR;
			this->error = -this->error;
		}

		if (this->callback)
			this->callback(this);

		delete this;
		return series->pop();
	}

protected:
	int send_timeo;
	int receive_timeo;
	int keep_alive_timeo;
	REQ req;
	RESP resp;
	std::function<void (WFNetworkTask<REQ, RESP> *)> callback;

protected:
	WFNetworkTask(CommSchedObject *object, CommScheduler *scheduler,
			std::function<void (WFNetworkTask<REQ, RESP> *)>&& cb) :
		CommRequest(object, scheduler),
		callback(std::move(cb))
	{
		this->user_data = NULL;
		this->send_timeo = -1;
		this->receive_timeo = -1;
		this->keep_alive_timeo = 0;
		this->target = NULL;
		this->timeout_reason = TOR_NOT_TIMEOUT;
		this->state = WFT_STATE_UNDEFINED;
		this->error = 0;
	}

	virtual ~WFNetworkTask() { }
};

class WFTimerTask : public SleepRequest
{
public:
	void start()
	{
		assert(!series_of(this));
		Workflow::start_series_work(this, nullptr);
	}

	void dismiss()
	{
		assert(!series_of(this));
		delete this;
	}

public:
	void *user_data;

public:
	int get_state() const { return this->state; }
	int get_error() const { return this->error; }

protected:
	virtual SubTask *done()
	{
		SeriesWork *series = series_of(this);

		if (this->callback)
			this->callback(this);

		delete this;
		return series->pop();
	}

protected:
	std::function<void (WFTimerTask *)> callback;

public:
	WFTimerTask(unsigned int microseconds, CommScheduler *scheduler,
				std::function<void (WFTimerTask *)> cb) :
		SleepRequest(microseconds, scheduler),
		callback(std::move(cb))
	{
		this->user_data = NULL;
		this->state = WFT_STATE_UNDEFINED;
		this->error = 0;
	}

protected:
	virtual ~WFTimerTask() { }
};

template<class ARG>
class WFFileTask : public IORequest
{
public:
	void start()
	{
		assert(!series_of(this));
		Workflow::start_series_work(this, nullptr);
	}

	void dismiss()
	{
		assert(!series_of(this));
		delete this;
	}

public:
	ARG *get_arg() { return &this->arg; }

	long get_retval()
	{
		if (this->state == WFT_STATE_SUCCESS)
			return this->get_res();
		else
			return -1;
	}

public:
	void *user_data;

public:
	int get_state() const { return this->state; }
	int get_error() const { return this->error; }

protected:
	virtual SubTask *done()
	{
		SeriesWork *series = series_of(this);

		if (this->callback)
			this->callback(this);

		delete this;
		return series->pop();
	}

protected:
	ARG arg;
	std::function<void (WFFileTask<ARG> *)> callback;

public:
	WFFileTask(IOService *service,
			   std::function<void (WFFileTask<ARG> *)>&& cb) :
		IORequest(service),
		callback(std::move(cb))
	{
		this->user_data = NULL;
		this->state = WFT_STATE_UNDEFINED;
		this->error = 0;
	}

protected:
	virtual ~WFFileTask() { }
};

class WFGenericTask : public SubTask
{
public:
	void start()
	{
		assert(!series_of(this));
		Workflow::start_series_work(this, nullptr);
	}

	void dismiss()
	{
		assert(!series_of(this));
		delete this;
	}

public:
	void *user_data;

public:
	int get_state() const { return this->state; }
	int get_error() const { return this->error; }

protected:
	virtual void dispatch()
	{
		this->state = WFT_STATE_SUCCESS;
		this->subtask_done();
	}

	virtual SubTask *done()
	{
		SeriesWork *series = series_of(this);
		delete this;
		return series->pop();
	}

protected:
	int state;
	int error;

public:
	WFGenericTask()
	{
		this->user_data = NULL;
		this->state = WFT_STATE_UNDEFINED;
		this->error = 0;
	}

protected:
	virtual ~WFGenericTask() { }
};

class WFCounterTask : public WFGenericTask
{
public:
	void count()
	{
		if (__sync_sub_and_fetch(&this->value, 1) == 0)
		{
			this->state = WFT_STATE_SUCCESS;
			this->subtask_done();
		}
	}

protected:
	virtual void dispatch()
	{
		this->count();
	}

	virtual SubTask *done()
	{
		SeriesWork *series = series_of(this);

		if (this->callback)
			this->callback(this);

		delete this;
		return series->pop();
	}

protected:
	unsigned int value;
	std::function<void (WFCounterTask *)> callback;

public:
	WFCounterTask(unsigned int target_value,
				  std::function<void (WFCounterTask *)>&& cb)
		: callback(std::move(cb))
	{
		this->value = target_value + 1;
	}

protected:
	virtual ~WFCounterTask() { }
};

class WFGoTask : public ExecRequest
{
public:
	void start()
	{
		assert(!series_of(this));
		Workflow::start_series_work(this, nullptr);
	}

	void dismiss()
	{
		assert(!series_of(this));
		delete this;
	}

protected:
	virtual void execute()
	{
		this->func();
	}

	virtual SubTask *done()
	{
		SeriesWork *series = series_of(this);
		delete this;
		return series->pop();
	}

protected:
	std::function<void ()> func;

public:
	WFGoTask(ExecQueue *queue, Executor *executor,
			 std::function<void ()>&& f) :
		ExecRequest(queue, executor),
		func(std::move(f))
	{
	}

protected:
	virtual ~WFGoTask() { }
};

#include "WFTask.inl"

#endif

