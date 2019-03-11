

#ifndef __SERVER_LIST_H__
#define __SERVER_LIST_H__

#include "all.h"
//#include "server.h"
//#include "my_list.h"

#include "my_list.h"

struct server_node{
	char name[100];		//服务器的名字
	int port;			//服务器的端口号
	char ip[20];		//服务器的IP
	
	struct list_head list;	/* 链表 */
};

#endif


