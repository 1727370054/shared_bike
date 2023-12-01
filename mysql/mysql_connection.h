#pragma once

#include <mysql/mysql.h>
#include <mysql/errmsg.h>
#include <string>
#include <string.h>
#include <cassert>

#include "global_def.h"
#include "logger.h"

class SqlRecordSet
{
public:
	SqlRecordSet() 
		:mysql_res_(nullptr)
	{
	}

	explicit SqlRecordSet(MYSQL_RES* mysql_res)
	{
		mysql_res_ = mysql_res;
	}

	inline MYSQL_RES* GetResult()
	{
		return mysql_res_;
	}

	inline void SetResult(MYSQL_RES* mysql_res)
	{
		assert(mysql_res_ == nullptr);
		if (mysql_res_)
		{
			LOG_WARN("the MYSQL_RES has already stored result, maybe will cause memory leak\n");
		}
		mysql_res_ = mysql_res;
	}

	void FetchRow(MYSQL_ROW& row)
	{
		row = mysql_fetch_row(mysql_res_);
	}

	inline i32 GetRowCount()
	{
		return mysql_res_->row_count;
	}

	~SqlRecordSet()
	{
		if (mysql_res_)
		{
			mysql_free_result(mysql_res_);
		}
	}

private:
	MYSQL_RES* mysql_res_;
};

class MySQLConnection
{
public:
	MySQLConnection();
	~MySQLConnection();

	MYSQL* GetMysql() { return mysql_; }

	bool Init(const char* szHost, int16_t nPort, const char* szUser, const char* szPasswd, const char* szDb);

	bool Execute(const char * szSql);

	bool Execute(const char* szSql, SqlRecordSet& recordSet);

	int EscapeString(const char* pSrc, int nSrcLen, char* pDest);

	void Close();

	const char* GetErrInfo();

	void Reconnct();

private:
	MYSQL* mysql_;
};