#pragma once

/**
* 1. 负责分发消息服务模块，把外部收到的消息，转化成内部事件，data->msg->event的解码过程
* 2. 然后把事件投递到线程池的消息队列，由线程调用proccess 方法对事件进行处理，最终调用每个event的handler方法
*    来处理event，此时每个event handler需要subscribe，该event才会被调用
*/

#include "ievent.h"
#include "event_type.h"
#include "iEventHandler.h"
#include "threadpool/thread.h"
#include "threadpool/thread_pool.h"
#include "network_interface.h"

#include <map>
#include <vector>
#include <mutex>
#include <atomic>
#include <queue>
#include <pthread.h>


class DispatchMsgService
{
public:
	virtual ~DispatchMsgService();

	virtual BOOL open();
	virtual void close();

	virtual void subscribe(u32 eid, iEventHandler* handler);
	virtual void unsubscribe(u32 eid, iEventHandler* handler);

	// 把事件投递到线程池
	virtual i32 enqueue(iEvent* ev);

	// 对具体事件进行分发处理
	virtual iEvent* process(const iEvent* ev);

	// 线程池回调函数
	static void svc(void* arg);

	static DispatchMsgService* getInstance();

	iEvent* parseEvent(const char* message, u32 len, u32 eid);

	void handleAllResponseEvent(NetworkInterface* Interface);

private:
	DispatchMsgService();
	DispatchMsgService(const DispatchMsgService&) = delete;
	DispatchMsgService& operator=(const DispatchMsgService&) = delete;

	ConnectSession* serialize(iEvent* ev, u32 eid);
protected:
	thread_pool_t* tp_;

	static std::atomic<bool> svr_exit_;
	static DispatchMsgService* DMS_;

	using T_EventHandlers = std::vector<iEventHandler*>;
	using T_EventHandlersMap = std::map<u32, T_EventHandlers>;
	T_EventHandlersMap subscribes_;

	static std::mutex mutex_; // 单例模式锁

	static std::mutex queue_mutex_;
	static std::queue<iEvent*> response_events_; // 响应事件队列
};