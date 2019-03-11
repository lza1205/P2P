
#ifndef __CLIENT_RECV_H_
#define __CLIENT_RECV_H_


#include "all.h"
#include "client.h"
#include "my_list.h"


/* 网络数据包 */
struct recv_data{
	struct sockaddr_in recv_addr;	/* 目标 */

	char *data;				/* 数据 */
	int data_len;				/* 数据长度 */

	struct list_head list;	/* 链表 */
};


void init_network_pthread(void);

void sync_service(void);

int add_recv_data_list(char *buf, int len, struct sockaddr_in *addr);
int recv_node_data(char *buf, int len, struct sockaddr_in *addr, int delay_time);


void resolve_recv_data(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr);

#endif


