/***********************************************************
文件名	:	client_net_data.h
作者		:	Faker
版本号	:	1.0
日期		:	2015.06.05

说明:	
	本文件作为客户端网络数据包的处理

***********************************************************/

#ifndef __CLIENT_NET_DATA_H_
#define __CLIENT_NET_DATA_H_

#include "all.h"

/* 网络数据包 */
struct net_data{

	struct sockaddr_in serveraddr;	/* 目标 */
	
	int cnt;				/* 剩余发送次数 */
	int time;				/* 重复发送倒计时 */
	
	char *data;				/* 数据 */
	int data_len;				/* 数据长度 */

	struct net_data *ptNext;	/* 链表 */
};


int add_net_data_list(char *buf, int len, struct sockaddr_in *addr);
int __printf_net_data_list(void);


int del_net_data_list(char *buf, int len);

void __test(void);

int init_net_data_list(void);


#endif


