#ifndef __P2P_THREAD_H_
#define __P2P_THREAD_H_

#include "all.h"
#include "client.h"

struct p2p_node_info{
	int status;				//是否在线 0 在线
	unsigned char name[USER_NAME_LEN];
	char ip[IP_STRING_MAX];
	int port;

	int list_typpe;		/* 0 是测试队列  1 是同步队列 */
	int sync_cnt;		/* 同步计数值 */
	int test_cnt;			/* 测试次数 */
	
	struct list_head list;
};

int p2p_send_data(char *name, char *buf, int len);



#endif

