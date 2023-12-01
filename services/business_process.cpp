#include "business_process.h"
#include "mysql_tables.h"

BusinessProcess::BusinessProcess(std::shared_ptr<MySQLConnection> mysqlconn)
	:mysqlconn_(mysqlconn), userhandler_(new UserEventHandler())
{
}

bool BusinessProcess::Init()
{
	MysqlTables tables(mysqlconn_);
	tables.CreateUserInfo();
	tables.CreateBikeTable();
	return true;
}

BusinessProcess::~BusinessProcess()
{
	userhandler_.reset();
}
