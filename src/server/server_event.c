




#include <stdio.h>
#include <string.h>
#include <sys/socket.h>     
#include <netinet/in.h>     
#include <arpa/inet.h>     
#include <netdb.h> 
#include <stdlib.h>
#include <unistd.h>


#include <event2/event_struct.h>
#include <event2/event_compat.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>


#include "all.h"
#include "server.h"
#include "server_client_node_list.h"

// 事件base 
struct event_base* base; 

extern struct tcp_aff_info tcp_aff_table[];



void send_cb(struct bufferevent *bev, char *buf, int len)
{
	bufferevent_write(bev, buf, len);
}

// 读缓冲区回调
void read_cb(struct bufferevent *bev, void *arg)
{
    char buf[2024] = {0}; 
	int ret;
    ret = bufferevent_read(bev, buf, sizeof(buf));

	printf("%s %d %p \r\n", __FILE__, __LINE__, bev);
	server_tcp_recv(bev, buf, ret);
    // 发数据给客户端
    //bufferevent_write(bev, p, strlen(p)+1);
    //printf("====== send buf: %s\n", p);
}

// 写缓冲区回调
void write_cb(struct bufferevent *bev, void *arg)
{
    //printf("我是写缓冲区的回调函数...\n"); 
}

// 事件
void event_cb(struct bufferevent *bev, short events, void *arg)
{
    if (events & BEV_EVENT_EOF)
    {
        printf("connection closed\n");  
    }
    else if(events & BEV_EVENT_ERROR)   
    {
        printf("some other error\n");
    }

	update_client_status(bev);

    bufferevent_free(bev);    
    printf("buffevent free...\n"); 	
}



void cb_listener(
        struct evconnlistener *listener, 
        evutil_socket_t fd, 
        struct sockaddr *addr, 
        int len, void *ptr)
{
   printf("connect new client\n");

   struct event_base* base = (struct event_base*)ptr;
   // 通信操作
   // 添加新事件
   struct bufferevent *bev;
   bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

   // 给bufferevent缓冲区设置回调
   bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);
   bufferevent_enable(bev, EV_READ);  //默认写enable，读disenable
}


int socket_event_init(short port)
{

    // init server 
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(SERVER_PORT_TCP);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);

	printf("%s %d \r\n", __FILE__, __LINE__);
    struct event_base* base;
    base = event_base_new();
    // 创建套接字
    // 绑定
    // 接收连接请求
    struct evconnlistener* listener;
    //第二个base传给了cb_listener的ptr(base --> ptr)
    //printf("%s %d \r\n", __FILE__, __LINE__);
    listener = evconnlistener_new_bind(base, cb_listener, base, 
                                  LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 
                                  36, (struct sockaddr*)&serv, sizeof(serv));
	//printf("%s %d \r\n", __FILE__, __LINE__);
    event_base_dispatch(base);
	//printf("%s %d \r\n", __FILE__, __LINE__);
    evconnlistener_free(listener);
	//printf("%s %d \r\n", __FILE__, __LINE__);
    event_base_free(base);
	//printf("%s %d \r\n", __FILE__, __LINE__);
    return 0;
}

#if 0
static int
foreach_call_fd(const struct event_base *base, const struct event *ev, void *arg)
{
	struct inotify_fd *intf_fp = (struct inotify_fd *)arg;
	
	int fd;
	
	if (event_get_callback(ev) == onRead)
	{
		fd = event_get_fd(ev);

		/* 发送信息*/
		printf("foreach_call_fd %s \r\n", intf_fp->name);
	}

	return 0;
}

void foreach_socket(void *pdata)
{
	
	event_base_foreach_event(base, foreach_call_fd, pdata);
}
#endif


void *__tcp_server_event(void *pdata)
{
	server_client_node_list_init();
	socket_event_init(SERVER_PORT_TCP);
}


void create_tcp_server_for_event(void)
{
	pthread_t server_thread;
    pthread_attr_t server_thread_attr;

	/* 创建子进程 */
	pthread_attr_init(&server_thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&server_thread_attr, PTHREAD_CREATE_DETACHED);
//	  if (pthread_create(&recv_thread, &recv_thread_attr, recv_pthread, NULL) < 0)
	if (pthread_create(&server_thread, &server_thread_attr, __tcp_server_event, NULL) < 0)
	{
		perror("pthread_create");
	}
}


