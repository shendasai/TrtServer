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

#ifndef _THRIFTMESSAGE_H_
#define _THRIFTMESSAGE_H_

#include <string>
#include "Communicator.h"
#include "EncodeStream.h"

static const int32_t THRIFT_VERSION_MASK = ((int32_t)0xffff0000);
static const int32_t THRIFT_VERSION_1 = ((int32_t)0x80010000);

enum ThriftMessageType {
  TMT_CALL       = 1,
  TMT_REPLY      = 2,
  TMT_EXCEPTION  = 3,
  TMT_ONEWAY     = 4
};

enum ThtirftDataType {
  TDT_STOP       = 0,
  TDT_VOID       = 1,
  TDT_BOOL       = 2,
  TDT_BYTE       = 3,
  TDT_I08        = 3,
  TDT_I16        = 6,
  TDT_I32        = 8,
  TDT_U64        = 9,
  TDT_I64        = 10,
  TDT_DOUBLE     = 4,
  TDT_STRING     = 11,
  TDT_UTF7       = 11,
  TDT_STRUCT     = 12,
  TDT_MAP        = 13,
  TDT_SET        = 14,
  TDT_LIST       = 15,
  TDT_UTF8       = 16,
  TDT_UTF16      = 17
};

struct thrift_parser_t
{
	int parse_succ;
	int status;
	int32_t framesize;
	size_t framesize_read_byte;
	std::string msgbuf;
	int32_t read_idx;
};

class ThriftMessage : public CommMessageOut, public CommMessageIn
{
public:
	ThriftMessage();
	virtual ~ThriftMessage();
	//move constructor
	ThriftMessage(ThriftMessage&& move);
	//move operator
	ThriftMessage& operator= (ThriftMessage &&move);

	//peek after CommMessageIn append
	bool parse_success() const;

	//clear data/result, memory stay and can be reused
	//not support for now
	//void clear();

	size_t get_data(std::string& data) const;
	void set_data(const std::string& data);

	int readMessageBegin(std::string& name, ThriftMessageType& messageType, int32_t& seqid) const;
	int readFieldBegin(ThtirftDataType& fieldType, int16_t& fieldId) const;
	int readI08(int8_t& val) const;
	int readI16(int16_t& val) const;
	int readI32(int32_t& val) const;
	int readI64(int64_t& val) const;
	int readString(std::string& str) const;
	int skip(const ThtirftDataType fieldType) const;

	int writeMessageBegin(const std::string& name, const ThriftMessageType messageType, const int32_t seqid);
	int writeFieldBegin(const ThtirftDataType fieldType, const int16_t fieldId);
	int writeFieldStop();
	int writeI08(const int8_t val);
	int writeI16(const int16_t val);
	int writeI32(const int32_t val);
	int writeI64(const int64_t val);
	int writeString(const std::string& str);

protected:
	int readStringBody(std::string& str, const int32_t slen) const;

	thrift_parser_t *parser_;

private:
	virtual int encode(struct iovec vectors[], int max);
	virtual int append(const void *buf, size_t size);

	EncodeStream *stream_;
};

using ThriftRequest = ThriftMessage;
using ThriftResponse = ThriftMessage;

/*
class ThriftRequest : public ThriftMessage
{
public:
	ThriftRequest() = default;
	//move constructor
	ThriftRequest(ThriftRequest&& move) = default;
	//move operator
	ThriftRequest& operator= (ThriftRequest&& move) = default;
};

class ThriftResponse : public ThriftMessage
{
public:
	ThriftResponse() = default;
	//move constructor
	ThriftResponse(ThriftResponse&& move) = default;
	//move operator
	ThriftResponse& operator= (ThriftResponse&& move) = default;
};
*/
#endif

