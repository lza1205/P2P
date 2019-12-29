
#ifndef __SERVER_AFF_H_
#define __SERVER_AFF_H_

#ifdef __SERVER_AFF_C_
#define __SERVER_AFF_EXT_
#else
#define __SERVER_AFF_EXT_ extern
#endif

#include "all.h"
#include "server.h"



struct aff_info{
	unsigned int aff;	 /* 事务类型 */

	/* 事务处理函数 */
	void (*aff_fun)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr);
};


struct tcp_aff_info{
	unsigned int aff;	 /* 事务类型 */

	/* 事务处理函数 */
	void (*aff_fun)(void *pfd, char *buf, int len);
};


/*
存储 json 结构体的数据
*/
struct json_data{
	char *key;
	int data_type;		/* 数据类型 */
	char val_s[100];
	
};



__SERVER_AFF_EXT_ struct aff_info aff_table[];



void aff_clientt_login(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr);
void aff_client_sync(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr);
void server_tcp_recv(void *iCliFd, char *buf, int iLen);


#endif


