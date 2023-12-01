#include <iostream>
#include <memory>
#include <unistd.h>

#include "events_def.h"
#include "global_def.h"
#include "bike.pb.h"
#include "user_event_handler.h"
#include "dispatch_msg_service.h"
#include "network_interface.h"
#include "ini_config.h"
#include "config_def.h"
#include "logger.h"
#include "mysql_connection.h"
#include "business_process.h"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Please input %s <config file path>\n", argv[0]);
		exit(-1);
	}

	if (!Logger::getInstance()->initLogger(static_cast<std::string>(argv[2])))
	{
		fprintf(stderr, "init log module failed \n");
		exit(-2);
	}

	IniConfig* iniConfig = IniConfig::getInstance();
	if (!iniConfig->loadFile(static_cast<std::string>(argv[1])))
	{
		LOG_ERROR("load %s failed", argv[1]);
		exit(-3);
	}

	ConfigDef config_args = iniConfig->getConfig();

	int16_t port = config_args.getSvrPort();
	const char* szHost = config_args.getDbIp().c_str();
	int16_t nPort = config_args.getDbPort();
	const char* szUser = config_args.getDbUser().c_str();
	const char* szPasswd = config_args.getDbPwd().c_str();
	const char* szDb = config_args.getDbName().c_str();

	std::shared_ptr<MySQLConnection> mysqlconn(new MySQLConnection());
	if (!mysqlconn->Init(szHost, nPort, szUser, szPasswd, szDb))
	{
		LOG_ERROR("Database init failed exit \n");
		exit(-4);
	}

	BusinessProcess busPro(mysqlconn);
	busPro.Init();
	
	DispatchMsgService* DMS = DispatchMsgService::getInstance();
	DMS->open();

	std::unique_ptr<NetworkInterface> net(new NetworkInterface());
	net->start(port);
	while (true)
	{
		net->network_event_dispatch();
	}

	DMS->close();

	return 0;
}

