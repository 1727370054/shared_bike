#pragma once

#include "global_def.h"

typedef struct EErrorReason
{
	i32 code;
	const char* reason;
}EErrorReason;

/*事件ID*/
enum EventID
{
	EEVENTID_GET_MOBLIE_CODE_REQ		= 0x01,
	EEVENTID_GET_MOBLIE_CODE_RSP		= 0x02,

	EEVENTID_LOGIN_REQ					= 0x03,
	EEVENTID_LOGIN_RSP					= 0x04,

	EEVENTID_RECHARGE_REQ				= 0x05,
	EEVENTID_RECHARGE_RSP				= 0x06,

	EEVENTID_GET_ACCOUNT_BALANCE_REQ	= 0x07,
	EEVENTID_ACCOUNT_BALANCE_RSP        = 0x08,

	EEVENTID_LIST_ACCOUNT_RECORDS_REQ	= 0x09,
	EEVENTID_ACCOUNT_RECORDS_RSP        = 0x10,

	EEVENTID_LIST_TRAVELS_REQ			= 0x11,
	EEVENTID_LIST_TRAVELS_RSP			= 0x12,

	EEVENTID_EXIT_RSP					= 0xFE,
	EEVENTID_UNKOWN						= 0xFF
};

/*事件响应错误代号*/
enum EErrorCode
{
	EERC_SUCCESS				= 200,
	EERC_INVALID_MSG			= 400,
	EERC_INVALID_DATA			= 404,
	EERC_METHOD_NOT_ALLOWED		= 405,
	EERO_PROCCESS_FAILED		= 406,
	EERO_BIKE_IS_TOOK			= 407,
	EERO_BIKE_IS_RUNNING		= 408,
	EERO_BIKE_IS_DAMAGED		= 409,
	ERR_NULL					= 0
};

const char* getReasonByErrorCode(i32 code);