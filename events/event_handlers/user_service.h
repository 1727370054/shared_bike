#pragma once

#include "mysql_connection.h"

#include <memory>

class UserService
{
public:
	UserService(std::shared_ptr<MySQLConnection> mysqlconn);
	bool exist(const std::string& mobile);
	bool insert(const std::string& mobile);
	bool update(const std::string& mobile, const i32 amount);
	i32 select_balance(const std::string& mobile);
private:
	std::shared_ptr<MySQLConnection> mysqlconn_;
};
