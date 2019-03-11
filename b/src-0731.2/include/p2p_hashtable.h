/***********************************************************
文件名	:	p2p_hashtable.h
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.13

说明:
		服务器的哈希表功能函数

***********************************************************/

#ifndef __P2P_HASHTABLE_H_
#define __P2P_HASHTABLE_H_


#include "all.h"
#include "server.h"



/* 客户端信息 */
struct __client_info{
	char name[USER_NAME_LEN];
	char passwd[USER_NAME_LEN];
	char version[USER_NAME_LEN];			//客户端的版本号
    char ip[IP_STRING_MAX];
    unsigned int port;
	char lan_ip[IP_STRING_MAX];		//局域网IP
	int lan_port;	//局域网端口
	
	int login_cnt;			/* 用来判断用户是否活跃 */
};

/* 哈希表 */
struct key
{
    char name[USER_NAME_LEN];
};

/* 哈希表 */
struct value
{
	struct __client_info cli_info;
};

unsigned int hashfromkey(void *ky);

int equalkeys(void *k1, void *k2);

struct value *__search_some(struct hashtable *h_table, struct key *k);
struct value *__remove_some(struct hashtable *h_table, struct key *k);


struct value *insert_hashtable_value(char *name);

int alloc_cli_ipport(struct value *val);


struct value *fine_value(char *name);




#endif



