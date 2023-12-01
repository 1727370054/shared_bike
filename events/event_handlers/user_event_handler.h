#pragma once

#include "global_def.h"
#include "iEventHandler.h"
#include "events_def.h"
#include "threadpool/thread.h"
#include "config_def.h"
#include "ini_config.h"
#include "logger.h"

#include <string>
#include <memory>
#include <mutex>
#include <map>

class MySQLConfig {
public:
    std::string szHost;
    int nPort;
    std::string szUser;
    std::string szPasswd;
    std::string szDb;

    MySQLConfig() {
        ConfigDef config_args = IniConfig::getInstance()->getConfig();
        szHost = config_args.getDbIp();
        nPort = config_args.getDbPort();
        szUser = config_args.getDbUser();
        szPasswd = config_args.getDbPwd();
        szDb = config_args.getDbName();
    }
};


class UserEventHandler : public iEventHandler
{
public:
	UserEventHandler();
	virtual ~UserEventHandler();
	virtual iEvent* handle(const iEvent * ev) override;
private:
	MobileCodeRspEv* handle_mobile_code_req(MobileCodeReqEv* ev);
	i32 icode_gen();

	LoginRspEv* handle_login_req(LoginReqEv * ev);

	RechargeRspEv* handle_recharge_req(RechargeReqEv* ev);

private:
	std::map<std::string, i32> m2c_; // first is mobile, second is code
	static std::mutex mutex_;
	std::unique_ptr<MySQLConfig> mysqlConfig_;
};
