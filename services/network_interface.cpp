#include "network_interface.h"
#include "dispatch_msg_service.h"
#include "logger.h"
#include "ini_config.h"
#include "config_def.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static ConnectSession* session_init(i32 fd, struct bufferevent* bev)
{
	ConnectSession* temp = new ConnectSession();

	if (!temp)
	{
		LOG_WARN("session_init: malloc failed\n");
		return nullptr;
	}

	memset(temp, 0, sizeof(ConnectSession));
	temp->bev = bev;
	temp->fd = fd;

	return temp;
}

void session_free(ConnectSession* cs)
{
	if (cs)
	{
		if (cs->read_buf)
		{
			delete[] cs->read_buf;
			cs->read_buf = nullptr;
		}
		if (cs->write_buf)
		{
			delete[] cs->write_buf;
			cs->write_buf = nullptr;
		}

		delete cs;
	}
}

void session_reset(ConnectSession* cs)
{
	if (cs)
	{
		if (cs->read_buf)
		{
			delete[] cs->read_buf;
			cs->read_buf = nullptr;
		}
		if (cs->write_buf)
		{
			delete[] cs->write_buf;
			cs->write_buf = nullptr;
		}

		cs->session_stat = SESSION_STATUS::SS_REQUEST;
		cs->req_stat = MESSAGE_STATUS::MS_READ_HEADER;

		cs->message_len = 0;
		cs->read_msg_len = 0;
		cs->read_header_len = 0;
	}
}

NetworkInterface::NetworkInterface()
	:base_(nullptr), listener_(nullptr)
{
}

NetworkInterface::~NetworkInterface()
{
	close();
}

bool NetworkInterface::start(uint16_t port)
{
	struct sockaddr_in local;
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(port);

	base_ = event_base_new();
	listener_ = evconnlistener_new_bind(base_, NetworkInterface::listener_callback,
		base_, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, backlog, (struct sockaddr*)&local, sizeof(local));

	return true;
}

void NetworkInterface::close()
{
	if (base_)
	{
		event_base_free(base_);
		base_ = nullptr;
	}
	if (listener_)
	{
		evconnlistener_free(listener_);
		listener_ = nullptr;
	}
}

void NetworkInterface::network_event_dispatch()
{
	event_base_loop(base_, EVLOOP_NONBLOCK);
	// 处理响应事件，发送响应
	DispatchMsgService::getInstance()->handleAllResponseEvent(this);
}

void NetworkInterface::listener_callback(evconnlistener* listener, evutil_socket_t fd, 
	sockaddr* client_addr, int socklen, void* arg)
{
	struct event_base* base = static_cast<event_base*>(arg);

	LOG_DEBUG("accept a new client: fd %d\n", fd);

	// 为每个客户端分配bufferevent
	struct bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

	ConnectSession* cs = session_init(fd, bev);
	cs->session_stat = SESSION_STATUS::SS_REQUEST;
	cs->req_stat = MESSAGE_STATUS::MS_READ_HEADER;
	strcpy(cs->remote_ip, inet_ntoa(((sockaddr_in*)client_addr)->sin_addr));

	LOG_DEBUG("remote ip: %s\n", cs->remote_ip);

	bufferevent_setcb(bev, NetworkInterface::handle_request, NetworkInterface::handle_response, 
		NetworkInterface::handle_error, cs);
	bufferevent_enable(bev, EV_READ | EV_PERSIST);

	IniConfig* iniConfig = IniConfig::getInstance();
	ConfigDef config_args = iniConfig->getConfig();
	const int read_timeout = config_args.getReadTimeout();
	const int write_timeout = config_args.getWriteTimeout();
	bufferevent_settimeout(bev, read_timeout, write_timeout);
}

/***********************************************
*			4字节	2字节	4字节		
* 请求格式: FBEB	事件ID	数据长度N	数据内容
* 
* 1. 包标识:   包头部特殊标识，用来标识包的开始
* 2. 事件类型: 事件ID标识请求事件类型
* 3. 数据长度: 正文数据包大小
* 4. 数据内容: 长度为数据包头部定义大小
************************************************/
void NetworkInterface::handle_request(bufferevent* bev, void* arg)
{
	ConnectSession* cs = static_cast<ConnectSession*>(arg);
	if (cs->session_stat != SESSION_STATUS::SS_REQUEST)
	{
		LOG_WARN("NetworkInterface::handle_request - wrong session state[%d]\n", cs->session_stat);
		return;
	}

	if (cs->req_stat == MESSAGE_STATUS::MS_READ_HEADER)
	{
		read_header(bev, cs);
	}

	if (cs->req_stat == MESSAGE_STATUS::MS_READ_MESSAGE && evbuffer_get_length(bufferevent_get_input(bev)) > 0)
	{
		read_message(bev, cs);
	}
}

void NetworkInterface::handle_response(bufferevent* bev, void* arg)
{
	LOG_DEBUG("NetworkInterface::handle_response\n");
}

void NetworkInterface::handle_error(bufferevent* bev, short events, void* arg)
{
	ConnectSession* cs = static_cast<ConnectSession*>(arg);

	LOG_DEBUG("NetworkInterface::handle_error\n");

	// 连接关闭
	if (events & BEV_EVENT_EOF)
	{
		LOG_DEBUG("NetworkInterface::handle_error - connection closed\n");
	}
	else if ((events & BEV_EVENT_TIMEOUT) && (events & BEV_EVENT_READING))
	{
		LOG_WARN("NetworkInterface::handle_error - reading timeout ... remote ip: %s, eid: %u, session stat: %d \n",
			cs->remote_ip, cs->eid, cs->session_stat);
	}
	else if ((events & BEV_EVENT_TIMEOUT) && (events & BEV_EVENT_WRITING))
	{
		LOG_WARN("NetworkInterface::handle_error - writing timeout ... remote ip: %s, eid: %u, session stat: %d \n", 
			cs->remote_ip, cs->eid, cs->session_stat);
	}
	else if (events & BEV_EVENT_ERROR)
	{
		LOG_ERROR("NetworkInterface::handle_error - some other error \n");
	}

	bufferevent_free(bev);
	session_free(cs);
}


void NetworkInterface::send_response_messaage(ConnectSession* cs)
{
	if (cs->eid == EEVENTID_EXIT_RSP)
	{
		bufferevent_free(cs->bev);
		if (cs->request)
		{
			delete cs->request;
		}
		session_free(cs);
	}
	else
	{
		bufferevent_write(cs->bev, cs->write_buf, cs->send_len);
		session_reset(cs);
	}
}

void NetworkInterface::read_header(bufferevent* bev, ConnectSession* cs)
{
	ssize_t len = bufferevent_read(bev, cs->header + cs->read_header_len, MESSAGE_HEADER_LEN - cs->read_header_len);
	cs->read_header_len += len;

	if (cs->read_header_len == MESSAGE_HEADER_LEN)
	{
		cs->header[cs->read_header_len] = '\0';
		LOG_DEBUG("recv from client <<<< %s\n", cs->header);

		if (strncasecmp(cs->header, MESSAGE_HEADER_ID, strlen(MESSAGE_HEADER_ID)) == 0)
		{
			cs->eid = *((u16*)(cs->header + 4));
			cs->message_len = *((u32*)(cs->header + 6));
			LOG_DEBUG("NetworkInterface::read_header - read %d byte in header, message len: %d\n",
				cs->read_header_len, cs->message_len);

			if (cs->message_len < 1 || cs->message_len > MAX_MESSAGE_LEN)
			{
				LOG_ERROR("NetworkInterface::read_header - wrong message len: %d\n", cs->message_len);
				bufferevent_free(bev);
				session_free(cs);
				return;
			}

			cs->read_buf = new char[cs->message_len + 1];
			cs->req_stat = MESSAGE_STATUS::MS_READ_MESSAGE;
			cs->read_msg_len = 0;
		}
		else
		{
			LOG_ERROR("NetworkInterface::read_header - invalied request from %s\n", cs->remote_ip);
			bufferevent_free(bev);
			session_free(cs);
			return;
		}
	}
}

void NetworkInterface::read_message(bufferevent* bev, ConnectSession* cs)
{
	ssize_t len = bufferevent_read(bev, cs->read_buf + cs->read_msg_len, cs->message_len - cs->read_msg_len);
	cs->read_msg_len += len;
	LOG_DEBUG("NetworkInterface::read_message - bufferevent_read: %d bytes, message len: %d, read len: %d\n",
		len, cs->message_len, cs->read_msg_len);

	if (cs->message_len == cs->read_msg_len)
	{
		cs->session_stat = SESSION_STATUS::SS_RESPONSE;
		iEvent* ev = DispatchMsgService::getInstance()->parseEvent(cs->read_buf, cs->read_msg_len, cs->eid);

		delete[] cs->read_buf;
		cs->read_buf = nullptr;
		cs->read_msg_len = 0;

		if (ev)
		{
			ev->set_args(cs);
			DispatchMsgService::getInstance()->enqueue(ev);
		}
		else
		{
			LOG_ERROR("NetworkInterface::read_message - ev is null, remote ip: %s, eid: %d\n", cs->remote_ip, cs->eid);
			bufferevent_free(bev);
			session_free(cs);
			return;
		}
	}
}
