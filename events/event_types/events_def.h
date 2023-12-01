#pragma once

#include "event_type.h"
#include "ievent.h"
#include "bike.pb.h"

#include <string>

class MobileCodeReqEv : public iEvent
{
public:
	MobileCodeReqEv(const std::string& mobile) 
		: iEvent(EEVENTID_GET_MOBLIE_CODE_REQ, iEvent::generateSeqNo())
	{
		msg_.set_mobile(mobile);
	}

	const std::string& get_mobile() const { return msg_.mobile(); }

	virtual std::ostream& dump(std::ostream& out) const override;
	virtual i32 ByteSize() override { return msg_.ByteSize(); }
	virtual bool SerializeToArray(char* buffer, i32 len) override { return msg_.SerializeToArray(buffer, len); }

private:
	tutorial::mobile_request msg_;
};

class MobileCodeRspEv : public iEvent
{
public:
	MobileCodeRspEv(i32 code, i32 icode)
		:iEvent(EEVENTID_GET_MOBLIE_CODE_RSP, iEvent::generateSeqNo())
	{
		msg_.set_code(code);
		msg_.set_icode(icode);
		msg_.set_desc(getReasonByErrorCode(code));
	}

	const i32 get_code() const { return msg_.code(); }
	const i32 get_icode() const { return msg_.icode(); }
	const std::string& get_desc() const { return msg_.desc(); }

	virtual std::ostream& dump(std::ostream& out) const override;
	virtual i32 ByteSize() override { return msg_.ByteSize(); }
	virtual bool SerializeToArray(char* buffer, i32 len) override { return msg_.SerializeToArray(buffer, len); }

private:
	tutorial::mobile_response msg_;
};

class ExitRspEv : public iEvent
{
public:
	ExitRspEv()
		:iEvent(EEVENTID_EXIT_RSP, iEvent::generateSeqNo())
	{
	}
	~ExitRspEv(){}
};

class LoginReqEv : public iEvent
{
public:
	LoginReqEv(const std::string& mobile, i32 icode)
		:iEvent(EEVENTID_LOGIN_REQ, iEvent::generateSeqNo())
	{
		msg_.set_mobile(mobile);
		msg_.set_icode(icode);
	}

	const std::string& get_mobile() { return msg_.mobile(); }
	const i32 get_icode() { return msg_.icode(); }

	virtual std::ostream& dump(std::ostream& out) const override;
	virtual i32 ByteSize() override { return msg_.ByteSize(); }
	virtual bool SerializeToArray(char* buffer, i32 len) override { return msg_.SerializeToArray(buffer, len); }

private:
	tutorial::login_request msg_;
};

class  LoginRspEv : public iEvent
{
public:
	LoginRspEv(i32 code)
		:iEvent(EEVENTID_LOGIN_RSP, iEvent::generateSeqNo())
	{
		msg_.set_code(code);
		msg_.set_desc(getReasonByErrorCode(code));
	}

	const i32 get_code() { return msg_.code(); }
	const std::string& get_desc() { return msg_.desc(); }

	virtual std::ostream& dump(std::ostream& out) const override;
	virtual i32 ByteSize() override { return msg_.ByteSize(); }
	virtual bool SerializeToArray(char* buffer, i32 len) override { return msg_.SerializeToArray(buffer, len); }

private:
	tutorial::login_response msg_;
};

class RechargeReqEv:public iEvent
{
public:
	RechargeReqEv(const std::string& mobile, i32 amount)
		:iEvent(EEVENTID_RECHARGE_REQ, iEvent::generateSeqNo())
	{
		msg_.set_mobile(mobile);
		msg_.set_amount(amount);
	}

	const std::string& get_mobile() { return msg_.mobile(); }
	const i32 get_amount() { return msg_.amount(); }

	virtual std::ostream& dump(std::ostream& out) const override;
	virtual i32 ByteSize() override { return msg_.ByteSize(); }
	virtual bool SerializeToArray(char* buffer, i32 len) override { return msg_.SerializeToArray(buffer, len); }

private:
	tutorial::recharge_request msg_;
};

class RechargeRspEv:public iEvent
{
public:
	RechargeRspEv(i32 code, i32 balance)
		:iEvent(EEVENTID_RECHARGE_RSP, iEvent::generateSeqNo())
	{
		msg_.set_code(code);
		msg_.set_balance(balance);
		msg_.set_desc(getReasonByErrorCode(code));
	}

	const i32& get_code() { msg_.code(); }
	const i32& get_balance() { return msg_.balance(); }
	const std::string& get_desc() { return msg_.desc(); }

	virtual std::ostream& dump(std::ostream& out) const override;
	virtual i32 ByteSize() override { return msg_.ByteSize(); }
	virtual bool SerializeToArray(char* buffer, i32 len) override { return msg_.SerializeToArray(buffer, len); }

private:
	tutorial::recharge_response msg_;
};




