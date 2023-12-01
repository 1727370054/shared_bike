#pragma once

#include "user_event_handler.h"
#include "mysql_connection.h"

#include <memory>

class BusinessProcess
{
public:
	BusinessProcess(std::shared_ptr<MySQLConnection> mysqlconn);

	bool Init();

	virtual ~BusinessProcess();
private:
	std::shared_ptr<MySQLConnection> mysqlconn_;
	std::shared_ptr<UserEventHandler> userhandler_;
};
