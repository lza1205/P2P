/***********************************************************
文件名	:	my_socket.h
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.13

说明:
	对TCP/IP 协议栈近一步封装

***********************************************************/

#ifndef __MY_SOCKET_H_
#define __MY_SOCKET_H_

#include "all.h"

int aes256_recvfrom(int fd, void *buf, int len, int flags, struct sockaddr *addr, socklen_t *addr_len);

int aes256_sendto(int fd, void *buf, int len,
		       int flags, struct sockaddr *addr,
		       socklen_t addr_len);

int p2p_recvfrom(int fd, void *buf, int len, int flags, struct sockaddr *addr, socklen_t *addr_len);

int p2p_sendto(int fd, void *buf, int len,
		       int flags, struct sockaddr *addr,
		       socklen_t addr_len);

void create_server_socket(struct sockaddr_in *servaddr);

#endif



