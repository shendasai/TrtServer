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

#include <stdint.h>
#include <string.h>
#include <string>
#include "mysql_types.h"
#include "mysql_util.h"

using MySQLHandshakeRequest = MySQLMessage;

class MySQLHandshakeResponse : public MySQLMessage
{
public:
	void get_challenge(char *arr) const
	{
		memcpy(arr, auth_plugin_data_part_1_, 8);
		memcpy(arr + 8, auth_plugin_data_part_2_, 12);
	}

private:
	virtual int append(const void *buf, size_t size);
	bool decode_packet(const char *buf, size_t buflen);

	uint8_t protocol_version_;
	std::string server_version_;
	uint32_t connection_id_;
	uint8_t auth_plugin_data_part_1_[8];
	uint8_t character_set_;
	uint16_t status_flags_;
	uint8_t auth_plugin_data_part_2_[12];

public:
	MySQLHandshakeResponse() = default;
	//move constructor
	MySQLHandshakeResponse(MySQLHandshakeResponse&& move) = default;
	//move operator
	MySQLHandshakeResponse& operator= (MySQLHandshakeResponse&& move) = default;
};

class MySQLAuthRequest : public MySQLMessage
{
public:
	void set_auth(const std::string& username,
				  const std::string& password,
				  const std::string& db,
				  int character_set)
	{
		username_ = username;
		password_ = password;
		db_ = db;
		character_set_ = character_set;
	}

	void set_challenge(const char *arr)
	{
		challenge_.assign(arr, 20);
	}

private:
	virtual int encode(struct iovec vectors[], int max);

	std::string username_;
	std::string password_;
	std::string db_;
	std::string challenge_;
	int character_set_ = 33;

public:
	MySQLAuthRequest() = default;
	//move constructor
	MySQLAuthRequest(MySQLAuthRequest&& move) = default;
	//move operator
	MySQLAuthRequest& operator= (MySQLAuthRequest&& move) = default;
};

using MySQLAuthResponse = MySQLResponse;

//////////

inline mysql_parser_t *MySQLMessage::get_parser() const
{
	return parser_;
}

inline int MySQLMessage::get_seqid() const
{
	return seqid_;
}

inline void MySQLMessage::set_seqid(int seqid)
{
	seqid_ = seqid;
}

inline int MySQLMessage::get_command() const
{
	return parser_->cmd;
}

inline void MySQLMessage::set_command(int cmd) const
{
	mysql_parser_set_command(cmd, parser_);
}

inline MySQLMessage::MySQLMessage():
	stream_(new mysql_stream_t),
	parser_(new mysql_parser_t),
	seqid_(0),
	cur_size_(0)
{
	mysql_stream_init(stream_);
	mysql_parser_init(parser_);
}

inline void MySQLRequest::set_query(const char *query)
{
	set_query(query, strlen(query));
}

inline void MySQLRequest::set_query(const std::string& query)
{
	set_query(query.c_str(), query.size());
}

inline bool MySQLResponse::is_ok_packet() const
{
	return parser_->packet_type == MYSQL_PACKET_OK;
}

inline bool MySQLResponse::is_error_packet() const
{
	return parser_->packet_type == MYSQL_PACKET_ERROR;
}

inline int MySQLResponse::get_packet_type() const
{
	return parser_->packet_type;
}

inline unsigned long long MySQLResponse::get_affected_rows() const
{
	return parser_->affected_rows;
}

inline unsigned long long MySQLResponse::get_last_insert_id() const
{
	return parser_->insert_id;
}

inline int MySQLResponse::get_warnings() const
{
	return parser_->warning_count;
}

inline int MySQLResponse::get_status_flags() const
{
	return parser_->server_status;
}

inline int MySQLResponse::get_error_code() const
{
	return parser_->error;
}

inline std::string MySQLResponse::get_error_msg() const
{
	const char *s;
	size_t slen;

	mysql_parser_get_err_msg(&s, &slen, parser_);
	return std::string(s, slen);
}

inline std::string MySQLResponse::get_sql_state() const
{
	const char *s;
	size_t slen;

	mysql_parser_get_net_state(&s, &slen, parser_);
	return std::string(s, slen);
}

inline std::string MySQLResponse::get_info() const
{
	const char *s;
	size_t slen;

	mysql_parser_get_info(&s, &slen, parser_);
	return std::string(s, slen);
}
