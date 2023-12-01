#include "dispatch_msg_service.h"
#include "global_def.h"
#include "network_interface.h"
#include "events_def.h"
#include "bike.pb.h"
#include "logger.h"

#include <algorithm>
#include <vector>

DispatchMsgService* DispatchMsgService::DMS_ = nullptr;
std::mutex DispatchMsgService::mutex_;
std::atomic<bool> DispatchMsgService::svr_exit_(false);
std::mutex DispatchMsgService::queue_mutex_;
std::queue<iEvent*> DispatchMsgService::response_events_;


DispatchMsgService::DispatchMsgService()
	:tp_(nullptr)
{
}

DispatchMsgService::~DispatchMsgService()
{
}

BOOL DispatchMsgService::open()
{
	tp_ = thread_pool_init();
	return tp_ ? TRUE : FALSE;
}

void DispatchMsgService::close()
{
	svr_exit_ = true;
	thread_pool_destroy(tp_);

	tp_ = nullptr;
	subscribes_.clear();
}

void DispatchMsgService::subscribe(u32 eid, iEventHandler* handler)
{
	LOG_DEBUG("DispatchMsgService::subscribe eid: %u\n", eid);
	T_EventHandlersMap::iterator iter = subscribes_.find(eid);
	if (iter != subscribes_.end())
	{
		T_EventHandlers::iterator hdl_iter = std::find(iter->second.begin(), iter->second.end(), handler);
		if (hdl_iter == iter->second.end())
		{
			iter->second.push_back(handler);
		}
	}
	else
	{
		subscribes_[eid].push_back(handler);
	}
}

void DispatchMsgService::unsubscribe(u32 eid, iEventHandler* handler)
{
	T_EventHandlersMap::iterator iter = subscribes_.find(eid);
	if (iter != subscribes_.end())
	{
		T_EventHandlers::iterator hdl_iter = std::find(iter->second.begin(), iter->second.end(), handler);
		if (hdl_iter != iter->second.end())
		{
			iter->second.erase(hdl_iter);
		}
	}
}

i32 DispatchMsgService::enqueue(iEvent* ev)
{
	if (nullptr == ev)
	{
		return -1;
	}

	thread_task_t * task = thread_task_alloc(0);
	task->handler = DispatchMsgService::svc;
	task->ctx = ev;

	return thread_task_post(tp_, task);
}

iEvent* DispatchMsgService::process(const iEvent* ev)
{
	LOG_DEBUG("DispatchMsgService::process -ev: %p\n", ev);
	if (nullptr == ev)
	{
		return nullptr;
	}

	u32 eid = ev->get_eid();

	LOG_DEBUG("DispatchMsgService::process -eid: %u\n", eid);

	if (eid == EEVENTID_UNKOWN)
	{
		LOG_WARN("DispatchMsgService: unkown evend id %d \n", eid);
		return nullptr;
	}

	T_EventHandlersMap::iterator handlers = subscribes_.find(eid);
	if (handlers == subscribes_.end())
	{
		LOG_WARN("DispatchMsgService: no any event handler subscribed %d\n", eid);
		return nullptr;
	}

	iEvent* response = nullptr;
	for (auto iter = handlers->second.begin(); iter != handlers->second.end(); iter++)
	{
		iEventHandler* handler = *iter;
		LOG_DEBUG("DispatchMsgService: get handler %s\n", handler->getName().c_str());
		response = handler->handle(ev);
	}

	return response;
}

void DispatchMsgService::svc(void* arg)
{
	DispatchMsgService* dms = DispatchMsgService::getInstance();
	iEvent* ev = static_cast<iEvent *>(arg);
	if (!dms->svr_exit_)
	{
		LOG_DEBUG("DispatchMsgService::svc ... \n");
		iEvent* response = dms->process(ev);
		if (response)
		{
			response->dump(std::cout);
			response->set_args(ev->get_args());
		}
		else
		{
			// 生成终止响应事件
			response = new ExitRspEv();
			response->set_args(ev->get_args());
		}
		queue_mutex_.lock();
		dms->response_events_.push(response);
		queue_mutex_.unlock();
	}
}

DispatchMsgService* DispatchMsgService::getInstance()
{
	if (nullptr == DMS_)
	{
		mutex_.lock();
		if (nullptr == DMS_)
		{
			DMS_ = new DispatchMsgService();
		}
		mutex_.unlock();
	}
	return DMS_;
}

ConnectSession* DispatchMsgService::serialize(iEvent* ev, u32 eid)
{
	ConnectSession* cs = static_cast<ConnectSession*>(ev->get_args());
	cs->response = ev;
	cs->eid = eid;

	// 序列化响应
	cs->message_len = ev->ByteSize();
	cs->send_len = cs->message_len + MESSAGE_HEADER_LEN;
	cs->write_buf = new char[cs->send_len];

	// 组装头部
	memcpy(cs->write_buf, MESSAGE_HEADER_ID, strlen(MESSAGE_HEADER_ID));
	*(u16*)(cs->write_buf + 4) = eid;
	*(i32*)(cs->write_buf + 6) = cs->message_len;

	ev->SerializeToArray(cs->write_buf + MESSAGE_HEADER_LEN, cs->message_len);

	return cs;
}

void DispatchMsgService::handleAllResponseEvent(NetworkInterface* Interface)
{
	bool done = false;

	while (!done)
	{
		iEvent* ev;
		queue_mutex_.lock();
		if (!response_events_.empty())
		{
			ev = response_events_.front();
			response_events_.pop();
		}
		else
		{
			done = true;
		}
		queue_mutex_.unlock();

		if (!done)
		{
			u32 eid = ev->get_eid();
			switch (eid)
			{
			case EEVENTID_GET_MOBLIE_CODE_RSP:
			{
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent - eid: %d\n", eid);

				ConnectSession* cs = serialize(ev, EEVENTID_GET_MOBLIE_CODE_RSP);
				Interface->send_response_messaage(cs);
			}
			break;
			case EEVENTID_LOGIN_RSP:
			{
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent - eid: %d\n", eid);

				ConnectSession* cs = serialize(ev, EEVENTID_LOGIN_RSP);

				Interface->send_response_messaage(cs);
			}
			break;
			case EEVENTID_RECHARGE_RSP:
			{
				LOG_DEBUG("DispatchMsgService::handleAllResponseEvent - eid: %d\n", eid);

				ConnectSession* cs = serialize(ev, EEVENTID_RECHARGE_RSP);

				Interface->send_response_messaage(cs);
			}
			break;
			case EEVENTID_EXIT_RSP:
			{
				ConnectSession* cs = static_cast<ConnectSession*>(ev->get_args());
				cs->response = ev;
				cs->eid = eid;
				Interface->send_response_messaage(cs);
			}
			break;
			default:
				break;
			}
		}
	}
}

iEvent* DispatchMsgService::parseEvent(const char* message, u32 len, u32 eid)
{
	if (!message)
	{
		LOG_ERROR("DispatchMsgService::parseEvent - message is null\n");
		return nullptr;
	}

	switch (eid)
	{
	case EEVENTID_GET_MOBLIE_CODE_REQ:
	{
		tutorial::mobile_request mrt;
		if (mrt.ParseFromArray(message, len))
		{
			MobileCodeReqEv* ev = new MobileCodeReqEv(mrt.mobile());
			return ev;
		}
		else
		{
			return nullptr;
		}
	}
		break;
	case EEVENTID_LOGIN_REQ:
	{
		tutorial::login_request lr;
		if (lr.ParseFromArray(message, len))
		{
			LoginReqEv* ev = new LoginReqEv(lr.mobile(), lr.icode());
			return ev;
		}
		else
		{
			return nullptr;
		}
	}
	break;
	case EEVENTID_RECHARGE_REQ:
	{
		tutorial::recharge_request rr;
		if (rr.ParseFromArray(message, len))
		{
			RechargeReqEv* ev = new RechargeReqEv(rr.mobile(), rr.amount());
			return ev;
		}
		else
		{
			return nullptr;
		}
	}
	break;
	case EEVENTID_GET_ACCOUNT_BALANCE_REQ:
	{

	}
	break;
	case EEVENTID_LIST_ACCOUNT_RECORDS_REQ:
	{

	}
	break;
	case EEVENTID_LIST_TRAVELS_REQ:
	{

	}
	break;
	default:
		break;
	}
	return nullptr;
}
