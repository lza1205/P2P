#ifndef __SERVER_CLIENT_INFO_LIST_H_
#define __SERVER_CLIENT_INFO_LIST_H_

#ifdef __SERVER_CLIENT_INFO_LIST_C_
#define __SERVER_CLIENT_INFO_LIST_EXT_
#else
#define __SERVER_CLIENT_INFO_LIST_EXT_	extern
#endif 

#include "all.h"
#include "my_list.h"

struct server_client_node{
	void *pfd;				//是否在线 0 在线
	char name[USER_NAME_LEN];
	struct list_head list;
};

int add_server_client_node(void *pfd, char *name);
void del_server_client_node(void *pfd);
void *find_server_client_socket(char *name);
void server_client_node_list_init(void);
int find_server_client_name(void * pfd, char *name);



#endif

