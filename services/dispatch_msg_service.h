#pragma once

/**
* 1. ����ַ���Ϣ����ģ�飬���ⲿ�յ�����Ϣ��ת�����ڲ��¼���data->msg->event�Ľ������
* 2. Ȼ����¼�Ͷ�ݵ��̳߳ص���Ϣ���У����̵߳���proccess �������¼����д������յ���ÿ��event��handler����
*    ������event����ʱÿ��event handler��Ҫsubscribe����event�Żᱻ����
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

	// ���¼�Ͷ�ݵ��̳߳�
	virtual i32 enqueue(iEvent* ev);

	// �Ծ����¼����зַ�����
	virtual iEvent* process(const iEvent* ev);

	// �̳߳ػص�����
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

	static std::mutex mutex_; // ����ģʽ��

	static std::mutex queue_mutex_;
	static std::queue<iEvent*> response_events_; // ��Ӧ�¼�����
};