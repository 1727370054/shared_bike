#include "event_type.h"

static EErrorReason EER[] =
{
	{EERC_SUCCESS, "ok"},
	{EERC_INVALID_MSG, "Invalid message"},
	{EERC_INVALID_DATA, "Invalid data"},
	{EERC_METHOD_NOT_ALLOWED, "Method not allowed"},
	{EERO_PROCCESS_FAILED, "Proccess failed"},
	{EERO_BIKE_IS_TOOK, "Bike is took"},
	{EERO_BIKE_IS_RUNNING, "Bike is running"},
	{EERO_BIKE_IS_DAMAGED, "Bike is damaged"},
	{ERR_NULL, "Undefined"}
};

const char* getReasonByErrorCode(i32 code)
{
	i32 i = 0;
	for (i = 0; EER[i].code != ERR_NULL; i++)
	{
		if (EER[i].code == code)
		{
			return EER[i].reason;
		}
	}

	return EER[i].reason;
}
