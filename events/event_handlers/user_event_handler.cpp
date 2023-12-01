#include "user_event_handler.h"
#include "dispatch_msg_service.h"
#include "logger.h"
#include "mysql_connection.h"
#include "ini_config.h"
#include "user_service.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>

std::mutex UserEventHandler::mutex_;

UserEventHandler::UserEventHandler()
	:iEventHandler("UserEventHandler"), mysqlConfig_(new MySQLConfig)
{
	// 实现订阅事件的处理
	DispatchMsgService::getInstance()->subscribe(EEVENTID_GET_MOBLIE_CODE_REQ, this);
	DispatchMsgService::getInstance()->subscribe(EEVENTID_GET_MOBLIE_CODE_RSP, this);
	DispatchMsgService::getInstance()->subscribe(EEVENTID_LOGIN_REQ, this);
	DispatchMsgService::getInstance()->subscribe(EEVENTID_LOGIN_RSP, this);
	DispatchMsgService::getInstance()->subscribe(EEVENTID_RECHARGE_REQ, this);
	DispatchMsgService::getInstance()->subscribe(EEVENTID_RECHARGE_RSP, this);

}

UserEventHandler::~UserEventHandler()
{
	// 实现退订事件的处理
	DispatchMsgService::getInstance()->unsubscribe(EEVENTID_GET_MOBLIE_CODE_REQ, this);
	DispatchMsgService::getInstance()->unsubscribe(EEVENTID_GET_MOBLIE_CODE_RSP, this);
	DispatchMsgService::getInstance()->unsubscribe(EEVENTID_LOGIN_REQ, this);
	DispatchMsgService::getInstance()->unsubscribe(EEVENTID_LOGIN_RSP, this);
	DispatchMsgService::getInstance()->unsubscribe(EEVENTID_RECHARGE_REQ, this);
	DispatchMsgService::getInstance()->unsubscribe(EEVENTID_RECHARGE_RSP, this);

}

iEvent* UserEventHandler::handle(const iEvent* ev)
{
	if (ev == nullptr)
	{
		LOG_ERROR("input ev is nullptr \n");
		printf("input ev is nullptr");
	}

	iEvent* evt = const_cast<iEvent*>(ev);
	u32 eid = ev->get_eid();

	switch (eid)
	{
	case EEVENTID_GET_MOBLIE_CODE_REQ:
		return handle_mobile_code_req(static_cast<MobileCodeReqEv*>(evt));
		break;
	case EEVENTID_LOGIN_REQ:
		return handle_login_req(static_cast<LoginReqEv*>(evt));
		break;
	case EEVENTID_RECHARGE_REQ:
		return handle_recharge_req(static_cast<RechargeReqEv*>(evt));
		break;
	case EEVENTID_GET_ACCOUNT_BALANCE_REQ:
		// return handle_get_account_balance_req(static_cast<GetAccountBalanceEv*>(ev));
		break;
	case EEVENTID_LIST_ACCOUNT_RECORDS_REQ:
		// return handle_list_account_records_req(static_cast<ListAccountRecordsEv*>(ev));
		break;
	case EEVENTID_LIST_TRAVELS_REQ:
		// return handle_list_travels_req(static_cast<ListTravelsEv* > (ev));
		break;
	default:
		break;
	}

	return nullptr;
}

MobileCodeRspEv* UserEventHandler::handle_mobile_code_req(MobileCodeReqEv* ev)
{
	i32 icode = 0;
	std::string mobile = ev->get_mobile();
	LOG_DEBUG("try to mobile phone %s validate code\n", mobile.c_str());

	icode = icode_gen();

	mutex_.lock();
	m2c_[mobile] = icode;
	mutex_.unlock();

	return new MobileCodeRspEv(EERC_SUCCESS, icode);
}

i32 UserEventHandler::icode_gen()
{
	i32 icode = 0;
	srand((unsigned int)time(NULL));

	icode = (unsigned int)(rand() % (999999 - 100000) + 100000);

	return icode;
}

LoginRspEv* UserEventHandler::handle_login_req(LoginReqEv* ev)
{
	LoginRspEv* loginRspEv = nullptr;
	std::string mobile = ev->get_mobile();
	i32 icode = ev->get_icode();

	LOG_DEBUG("try to handle login ev, mobile: %s, icode: %d\n", mobile.c_str(), icode);

	mutex_.lock();
	auto iter = m2c_.find(mobile);
	if (((iter != m2c_.end()) && (icode != iter->second)) || (iter == m2c_.end()))
	{
		loginRspEv =  new LoginRspEv(EERC_INVALID_DATA);
	}
	mutex_.unlock();
	if (loginRspEv) return loginRspEv;

	// 判断用户是否在数据库，不存在插入
	std::shared_ptr<MySQLConnection> mysqlconn(new MySQLConnection());
	if (!mysqlconn->Init(mysqlConfig_->szHost.c_str(), mysqlConfig_->nPort, mysqlConfig_->szUser.c_str(),
		mysqlConfig_->szPasswd.c_str(), mysqlConfig_->szDb.c_str()))
	{
		LOG_ERROR("UserEventHandler::handle_login_req - Database init failed exit \n");
		return new LoginRspEv(EERO_PROCCESS_FAILED);
	}

	UserService userService(mysqlconn);

	bool result = false;
	if (!userService.exist(mobile))
	{
		result = userService.insert(mobile);
		if (!result)
		{
			LOG_ERROR("insert user(%s) to database(%s) failed \n", mobile.c_str());
			return new LoginRspEv(EERO_PROCCESS_FAILED);
		}
	}

	return new LoginRspEv(EERC_SUCCESS);
}

RechargeRspEv* UserEventHandler::handle_recharge_req(RechargeReqEv* ev)
{
	if (!ev)
	{
		return nullptr;
	}
	std::string mobile = ev->get_mobile();
	i32 amount = ev->get_amount();

	LOG_DEBUG("try to handle recharge ev, mobile: %s, amount: %d\n", mobile.c_str(), amount);

	std::shared_ptr<MySQLConnection> mysqlconn(new MySQLConnection());
	if (!mysqlConfig_)
	{
		return nullptr;
	}
	if (!mysqlconn->Init(mysqlConfig_->szHost.c_str(), mysqlConfig_->nPort, mysqlConfig_->szUser.c_str(),
		mysqlConfig_->szPasswd.c_str(), mysqlConfig_->szDb.c_str()))
	{
		LOG_ERROR("UserEventHandler::handle_login_req - Database init failed exit \n");
		return new RechargeRspEv(EERO_PROCCESS_FAILED, -1);
	}

	UserService userService(mysqlconn);

	i32 balance = userService.select_balance(mobile);
	bool result = false;
	if (balance == -1)
	{
		return new RechargeRspEv(EERO_PROCCESS_FAILED, -1);
	}
	else if (balance == -2)
	{
		return new RechargeRspEv(EERC_INVALID_MSG, -1);
	}
	else
	{
		balance += amount;
		result = userService.update(mobile, balance);
		if (!result)
		{
			return new RechargeRspEv(EERO_PROCCESS_FAILED, -1);
		}
	}
	return new RechargeRspEv(EERC_SUCCESS, balance);
}
