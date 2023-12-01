#pragma once

#include "global_def.h"
#include "ievent.h"

#include <string>
#include <string.h>

#include <event.h>
#include <event2/bufferevent.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/listener.h>

#define MESSAGE_HEADER_LEN 10
#define MESSAGE_HEADER_ID  "FBEB"

enum class SESSION_STATUS
{
	SS_REQUEST,
	SS_RESPONSE
};

enum class MESSAGE_STATUS
{
	MS_READ_HEADER = 0,
	MS_READ_MESSAGE = 1, // ��Ϣ����δ��ʼ
	MS_READ_DONE = 2,    // ��Ϣ�������
	MS_SENDING = 3		 // ��Ϣ������
};

typedef struct _ConnectSession
{
	char remote_ip[32];

	SESSION_STATUS session_stat;

	iEvent* request;
	MESSAGE_STATUS req_stat;

	iEvent* response;
	MESSAGE_STATUS res_stat;

	u32 eid;			// ���浱ǰ�����¼���id
	i32 fd;				// ���浱ǰ���ӵ��ļ����

	struct bufferevent* bev;

	char* read_buf;		// �������Ϣ������
	u32 message_len;	// ��ǰ��ȡ��Ϣ����
	u32 read_msg_len;	// �Ѿ���ȡ��Ϣ����

	char header[MESSAGE_HEADER_LEN + 1];	// ����ͷ�� 10 + 1
	u32 read_header_len;					// �Ѷ�ȡͷ������

	char* write_buf;	// ���ͻ�����
	u32 send_len;		// �ѷ������ݳ���
}ConnectSession;

class NetworkInterface
{
	const int backlog = 8;
public:
	NetworkInterface();
	~NetworkInterface();

	bool start(uint16_t port);
	void close();

	static void listener_callback(struct evconnlistener* listener, evutil_socket_t fd, 
		struct sockaddr* client_addr, int socklen, void * arg);

	static void handle_request(struct bufferevent * bev, void * arg); // ������ص�
	static void handle_response(struct bufferevent* bev, void* arg);  // ��Ӧ�ص�
	static void handle_error(struct bufferevent* bev, short events,void* arg); // �쳣����ص�

	void network_event_dispatch();

	void send_response_messaage(ConnectSession * cs);

private:
	static void read_header(bufferevent* bev, ConnectSession* cs);
	static void read_message(bufferevent* bev, ConnectSession* cs);

private:
	struct event_base* base_;
	struct evconnlistener* listener_;
};