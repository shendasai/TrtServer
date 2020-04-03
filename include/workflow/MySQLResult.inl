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

  Authors: Li Yingxin (liyingxin@sogou-inc.com)
*/

#include <string>
#include <utility>
#include "mysql_util.h"

inline std::string MySQLField::get_name() const
{
	if (this->data_type == MYSQL_TYPE_NULL)
		return "";
	return std::string(this->name, this->name_length);
}

inline std::string MySQLField::get_org_name() const
{
	if (this->data_type == MYSQL_TYPE_NULL)
		return "";
	return std::string(this->org_name, this->org_name_length);
}

inline std::string MySQLField::get_table() const
{
	if (this->data_type == MYSQL_TYPE_NULL)
		return "";
	return std::string(this->table, this->table_length);
}

inline std::string MySQLField::get_org_table() const
{
	if (this->data_type == MYSQL_TYPE_NULL)
		return "";
	return std::string(this->org_table, this->org_table_length);
}

inline std::string MySQLField::get_db() const
{
	if (this->data_type == MYSQL_TYPE_NULL)
		return "";
	return std::string(this->db, this->db_length);
}

inline std::string MySQLField::get_catalog() const
{
	if (this->data_type == MYSQL_TYPE_NULL)
		return "";
	return std::string(this->catalog, this->catalog_length);
}

inline std::string MySQLField::get_def() const
{
	if (this->data_type == MYSQL_TYPE_NULL)
		return "";
	return std::string(this->def, this->def_length);
}

inline int MySQLField::get_charsetnr() const
{
	if (this->data_type == MYSQL_TYPE_NULL)
		return 0;
	return this->charsetnr;
}

inline int MySQLField::get_length() const
{
	if (this->data_type == MYSQL_TYPE_NULL)
		return 0;
	return this->length;
}

inline int MySQLField::get_flags() const
{
	if (this->data_type == MYSQL_TYPE_NULL)
		return 0;
	return this->flags;
}

inline int MySQLField::get_decimals() const
{
	if (this->data_type == MYSQL_TYPE_NULL)
		return 0;
	return this->decimals;
}

inline int MySQLField::get_data_type() const
{
	return this->data_type;
}

inline MySQLCell::MySQLCell(MySQLCell&& move)
{
	this->operator=(std::move(move));
}

inline MySQLCell& MySQLCell::operator=(MySQLCell&& move)
{
	if (this != &move)
	{
		this->data = move.data;
		this->len = move.len;
		this->data_type = move.data_type;

		move.data = NULL;
		move.len = 0;
	}

	return *this;
}

inline MySQLCell::MySQLCell(const void *data, size_t len, int data_type)
{
	this->data_type = data_type;
	this->data = const_cast<void *>(data);
	this->len = len;
}

inline MySQLCell::MySQLCell()
{
	this->data = NULL;
	this->len = 0;
	this->data_type = MYSQL_TYPE_NULL;
}

inline int MySQLCell::get_data_type() const
{
	return this->data_type;
}

inline void MySQLCell::get_cell_nocopy(const void **data, size_t *len,
									   int *data_type) const
{
	*data = this->data;
	*len = this->len;
	*data_type = this->data_type;
}

inline bool MySQLCell::is_null() const
{
	return (this->data_type == MYSQL_TYPE_NULL);
}

inline std::string MySQLCell::as_binary_string() const
{
	return std::string((char *)this->data, this->len);
}

inline bool MySQLCell::is_int() const
{
	return (this->data_type == MYSQL_TYPE_INT24 ||
			this->data_type == MYSQL_TYPE_LONG);
}

inline int MySQLCell::as_int() const
{
	if (!this->is_int())
		return 0;
	char num[NUM_STR_LENGTH];
	memcpy(num, this->data, this->len);
	num[this->len] = '\0';
	return atoi(num);
}

inline bool MySQLCell::is_char() const
{
	return (this->data_type == MYSQL_TYPE_TINY);
}

inline char MySQLCell::as_char() const
{
	if (!this->is_char())
		return '\0';

	return *((char *)this->data);
}

inline bool MySQLCell::is_short() const
{
	return (this->data_type == MYSQL_TYPE_SHORT ||
			this->data_type == MYSQL_TYPE_YEAR);
}

inline short MySQLCell::as_short() const
{
	if (!this->is_short())
		return 0;
	char num[NUM_STR_LENGTH];
	memcpy(num, this->data, this->len);
	num[this->len] = '\0';
	return atoi(num);
}

inline bool MySQLCell::is_float() const
{
	return (this->data_type == MYSQL_TYPE_FLOAT);
}

inline float MySQLCell::as_float() const
{
	if (!this->is_float())
		return 0;

	char num[FLOAT_STR_LENGTH];
	memcpy(num, this->data, this->len);
	num[this->len] = '\0';
	return strtof(num, NULL);
}

inline bool MySQLCell::is_double() const
{
	return (this->data_type == MYSQL_TYPE_DOUBLE);
}

inline double MySQLCell::as_double() const
{
	if (!this->is_double())
		return 0;

	char num[DOUBLE_STR_LENGTH];
	memcpy(num, this->data, this->len);
	num[this->len] = '\0';
	return strtod(num, NULL);
}

inline bool MySQLCell::is_ulonglong() const
{
	return (this->data_type == MYSQL_TYPE_LONGLONG);
}

inline unsigned long long MySQLCell::as_ulonglong() const
{
	if (!this->is_ulonglong())
		return 0;

	char num[NUM_STR_LENGTH];
	memcpy(num, this->data, this->len);
	num[this->len] = '\0';
	return strtoull(num, NULL, 10);
}

inline bool MySQLCell::is_date() const
{
	return (this->data_type == MYSQL_TYPE_DATE);
}

inline std::string MySQLCell::as_date() const
{
	if (!this->is_date())
		return "";

	return std::string((char *)this->data, this->len);
}

inline bool MySQLCell::is_time() const
{
	return (this->data_type == MYSQL_TYPE_TIME);
}

inline std::string MySQLCell::as_time() const
{
	if (!this->is_time())
		return "";

	return std::string((char *)this->data, this->len);
}

inline bool MySQLCell::is_timestamp() const
{
	return (this->data_type == MYSQL_TYPE_DATETIME ||
			this->data_type == MYSQL_TYPE_TIMESTAMP);
}

inline std::string MySQLCell::as_timestamp() const
{
	if (!this->is_timestamp())
		return "";

	return std::string((char *)this->data, this->len);
}

inline bool MySQLCell::is_string() const
{
	return (this->data_type == MYSQL_TYPE_DECIMAL ||
			this->data_type == MYSQL_TYPE_STRING ||
			this->data_type == MYSQL_TYPE_VARCHAR ||
			this->data_type == MYSQL_TYPE_VAR_STRING ||
			this->data_type == MYSQL_TYPE_JSON);
}

inline std::string MySQLCell::as_string() const
{
	if (!this->is_string() && !this->is_time() &&
		!this->is_date() && !this->is_timestamp())
		return "";

	return std::string((char *)this->data, this->len);
}

inline const MySQLField *MySQLResultCursor::fetch_field()
{
	if (this->status != MYSQL_STATUS_GET_RESULT &&
		this->status != MYSQL_STATUS_END)
		return NULL;

	if (this->current_field >= this->field_count)
		return NULL;

	return this->fields[this->current_field++];
}

inline const MySQLField *const *MySQLResultCursor::fetch_fields() const
{
	if (this->status != MYSQL_STATUS_GET_RESULT &&
		this->status != MYSQL_STATUS_END)
		return NULL;

	return this->fields;
}

inline int MySQLResultCursor::get_cursor_status() const
{
	return this->status;
}

inline int MySQLResultCursor::get_field_count() const
{
	if (this->status != MYSQL_STATUS_GET_RESULT &&
		this->status != MYSQL_STATUS_END)
		return 0;

	return this->field_count;
}

inline int MySQLResultCursor::get_rows_count() const
{
	if (this->status != MYSQL_STATUS_GET_RESULT && this->status != MYSQL_STATUS_END)
		return 0;

	return this->row_count;
}

