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
	MS_READ_MESSAGE = 1, // 消息传输未开始
	MS_READ_DONE = 2,    // 消息传输完毕
	MS_SENDING = 3		 // 消息传输中
};

typedef struct _ConnectSession
{
	char remote_ip[32];

	SESSION_STATUS session_stat;

	iEvent* request;
	MESSAGE_STATUS req_stat;

	iEvent* response;
	MESSAGE_STATUS res_stat;

	u32 eid;			// 保存当前请求事件的id
	i32 fd;				// 保存当前链接的文件句柄

	struct bufferevent* bev;

	char* read_buf;		// 保存读消息缓冲区
	u32 message_len;	// 当前读取消息长度
	u32 read_msg_len;	// 已经读取消息长度

	char header[MESSAGE_HEADER_LEN + 1];	// 保存头部 10 + 1
	u32 read_header_len;					// 已读取头部长度

	char* write_buf;	// 发送缓冲区
	u32 send_len;		// 已发送数据长度
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

	static void handle_request(struct bufferevent * bev, void * arg); // 读请求回调
	static void handle_response(struct bufferevent* bev, void* arg);  // 响应回调
	static void handle_error(struct bufferevent* bev, short events,void* arg); // 异常处理回调

	void network_event_dispatch();

	void send_response_messaage(ConnectSession * cs);

private:
	static void read_header(bufferevent* bev, ConnectSession* cs);
	static void read_message(bufferevent* bev, ConnectSession* cs);

private:
	struct event_base* base_;
	struct evconnlistener* listener_;
};