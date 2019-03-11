
#ifndef __RECV_PTHREAD_H__
#define __RECV_PTHREAD_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include <pthread.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <stdio.h>

#include "my_list.h"



struct recv_data_node{
	struct sockaddr_in addr;		/* 数据包地址信息 */
	char *data;					/* 数据包内容 */
	int len;					/* 数据包长度 */

	
	struct list_head data_list;		/* 数据包队列 */
};


typedef struct listen{		
	struct sockaddr addr;		/* 数据包地址信息 */
	char name[USER_NAME_LEN];	/* 数据包源名字 */
	unsigned int proto;			/* 数据包源协议 */
	
	int data_num;				/* 数据包数量 */
	int data_max_num;			/* 数据包最大数量 */
	
	int list_flg;				/* 是否已经被监听了 */

	pthread_mutex_t mutex;	/* 线程锁 */

	/* 这两个条件变量相关的 */
	
	pthread_mutex_t recv_mtx;
	pthread_cond_t recv_cond;

	struct list_head head;		/* 数据包队列 */
	
	struct list_head listen_list;		/*接收的线程队列 */

	struct list_head poll_list;			/* poll 机制的队列 */
}listen_t;

/*--------------------------------------------------------------------------*/
//提供给用户的接口
/*--------------------------------------------------------------------------*/


/*-------------------------------------------------------------------
常用接口
-------------------------------------------------------------------*/

/* 
从一个listen 中获取数据
*/
int recv_from_listen(	
	struct listen *recv_list, 
	struct sockaddr *addr,		/* 数据包地址信息 */
	char *buf,					/* 数据包内容 */
	int len,					/* 数据包长度 */
	int delay_time				/* 休眠时间。 -1表示永久休眠 */
);



/* 获取一个连接 */
struct listen *server_accept(void);

/*
关闭某个客户端
*/
int listen_close(struct listen *listen);


/*
开始监听
*/
void server_listen(int num);

/*
创建处理线程
*/
void listen_pthread(listen_t *listen, void *(*start_rtn)(void *));



/*-------------------------------------------------------------------
poll 机制接口
-------------------------------------------------------------------*/
	
/*
初始化一个 listen 队列头
*/
void listen_head_init(struct list_head *head);

/* 
将要监听的listen 加入到这个head 链表中
*/
int listen_add(struct list_head *head, listen_t *listen);

/* 从要监听的listen队列中获取数据 */
int recv_from_listen_head(
		struct list_head* head,
		struct listen **recv_listen, 	/* 指向指针的指针 */
		struct sockaddr *addr,		/* 数据包地址信息 */
		char *buf,					/* 数据包内容 */
		int len,					/* 数据包长度 */
		int delay_time				/* 休眠时间。 -1表示永久休眠 */
	);



/*-------------------------------------------------------------------
自己创建一个 listen 接口
-------------------------------------------------------------------*/
listen_t *create_listen(char *name, unsigned int proto);




#endif


