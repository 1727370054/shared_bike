#pragma once

#include "mysql_connection.h"
#include "logger.h"

#include <memory>

class MysqlTables
{
public:
	MysqlTables(std::shared_ptr<MySQLConnection> mysqlconn)
		:mysqlconn_(mysqlconn)
	{
	}

	bool CreateUserInfo()
	{
		const char* pUserInfoTable = "\
				CREATE TABLE IF NOT EXISTS userinfo(	\
				id			int(16)			NOT NULL primary key auto_increment,						\
				mobile		char(16)		NOT NULL unique,											\
				username	varchar(128)	NOT NULL default '',										\
				verify		int(4)			NOT NULL default 0,											\
				registertm	timestamp		NOT NULL default CURRENT_TIMESTAMP comment '注册时间',		\
				money		int(11)			NOT NULL default 0											\	
			)";

			if (!mysqlconn_->Execute(pUserInfoTable))
			{
				LOG_ERROR("create table userinfo failed, error descibe: %s\n", mysqlconn_->GetErrInfo());
				return false;
			}
		return true;
	}

	bool CreateBikeTable()
	{
		const char* pBikeInfoTable = "\
				CREATE TABLE IF NOT EXISTS bikeinfo(\
				id			int				NOT NULL primary key auto_increment,					\
				devno		int				NOT NULL comment '自行车编号',							\
				status		tinyint(1)		NOT NULL default 0,										\
				trouble		int				NOT NULL default 0 comment '损坏类型编号',				\
				tmsg		varchar(256)	NOT NULL default ''comment '损坏原因描述',				\
				latitude	double(10, 6)	NOT NULL default 0,										\
				longitude	double(10, 6)	NOT NULL default 0,										\
				unique(devno)																		\
			)";

		if (!mysqlconn_->Execute(pBikeInfoTable))
		{
			LOG_ERROR("create table bikeinfo failed, error descibe: %s\n", mysqlconn_->GetErrInfo());
			return false;
		}
		return true;
	}

	~MysqlTables(){}
private:
	std::shared_ptr<MySQLConnection> mysqlconn_;
};