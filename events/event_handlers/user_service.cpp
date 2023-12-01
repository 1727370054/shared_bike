#include "user_service.h"

#include <cstdio>
#include <mysql/mysql.h>

UserService::UserService(std::shared_ptr<MySQLConnection> mysqlconn)
	:mysqlconn_(mysqlconn)
{
}

bool UserService::exist(const std::string& mobile)
{
	char sql_content[1024] = { 0 };
	sprintf(sql_content, \
			"select * from userinfo where mobile = %s",\
			mobile.c_str());
	SqlRecordSet record_set;
	if (!mysqlconn_->Execute(sql_content, record_set))
	{
		return false;
	}
	return (record_set.GetRowCount() != 0);
}

bool UserService::insert(const std::string& mobile)
{
	char sql_content[1024] = { 0 };
	sprintf(sql_content, \
		"insert into userinfo (mobile) values (%s)", \
		mobile.c_str());
	if (!mysqlconn_->Execute(sql_content))
	{
		return false;
	}
	return true;
}

bool UserService::update(const std::string& mobile, const i32 amount)
{
	char sql_content[1024] = { 0 };
	sprintf(sql_content, \
		"update userinfo set money=%d where mobile='%s'", \
		amount, mobile.c_str());
	if (!mysqlconn_->Execute(sql_content))
	{
		return false;
	}
	return true;
}

i32 UserService::select_balance(const std::string& mobile)
{
	char sql_content[1024] = { 0 };
	// 使用参数化查询
	sprintf(sql_content, \
		"select money from userinfo where mobile = '%s'", \
		mobile.c_str());

	SqlRecordSet record_set;
	if (!mysqlconn_->Execute(sql_content, record_set))
	{
		return -1;
	}

	MYSQL_ROW row;
	record_set.FetchRow(row);
	if (row) {
		return std::stoi(row[0]);
	}
	else {
		return -2; // 没有查询到相关数据
	}
}

