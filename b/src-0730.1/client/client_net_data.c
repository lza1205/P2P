/***********************************************************
文件名	:	client_net_data.c
作者		:	Faker
版本号	:	1.0
日期		:	2015.06.05

说明:	
	本文件作为客户端网络数据包的处理

***********************************************************/


#include "all.h"
#include "client.h"

#include "client_net_data.h"


#include <pthread.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <stdio.h>


/* 这些函数涉及到多线程并发问题，不好处理 */
/* 需加上互斥锁 */
static pthread_mutex_t net_data_lock = PTHREAD_MUTEX_INITIALIZER;


/* 链表头 */
static struct net_data *net_data_list;



/********************************************
函数名:	__add_list
功能:		往网络数据链表中添加数据
********************************************/
int __add_list(struct net_data *new_data)
{
	struct net_data * ptTmp;

	if (!net_data_list)
	{
		net_data_list   = new_data;
		new_data->ptNext = NULL;
	}
	else
	{
		ptTmp = net_data_list;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = new_data;
		new_data->ptNext = NULL;
	}

	return 0;
}

/********************************************
函数名:	__del_list
功能:		往网络数据链表中删除数据
********************************************/

int __del_list(struct net_data *tmp, struct net_data *data)
{
	if(tmp == NULL)	//没有上一个，说明data是第一个
	{
		net_data_list = data->ptNext;
	}else{
		tmp->ptNext = data->ptNext;
	}
	free(data->data);
	free(data);

	return 0;
}


/********************************************
函数名:	__printf_net_data_list
功能:		打印所有网络数据包
********************************************/

int __printf_net_data_list(void)
{
	struct net_data *tmp;
	struct check_head *head;
	int i = 0;

	pthread_mutex_lock(&net_data_lock);
	
	tmp = net_data_list;

	dbg_printf("/********** printf list **********/\r\n");
	
	while(tmp != NULL)
	{
		head = (struct check_head *)(tmp->data);
		dbg_printf("[%d] %d\r\n", i++, head->timecnt[0]);
		tmp = tmp->ptNext;
	}

	pthread_mutex_unlock(&net_data_lock);
	
	return 0;
}



/********************************************
函数名:	add_net_data_list
功能:		把数据添加到网络数据链表中
********************************************/

int add_net_data_list(char *buf, int len, struct sockaddr_in *addr)
{
	struct net_data *new_data;
	char *data;
	
	new_data = (struct net_data*)malloc(sizeof(struct net_data));
	if(new_data == NULL)
		return -1;

	data = (char *)malloc(len);
	if(data == NULL){
		free(new_data);
		return -1;
	}

	/* 复制数据 */
	if(addr != NULL)
		memcpy(&(new_data->serveraddr), addr, sizeof(struct sockaddr_in));

	memcpy(data, buf, len);
	new_data->data = data;
	new_data->data_len = len;

	new_data->cnt = 2;
	new_data->time = 3;

	pthread_mutex_lock(&net_data_lock);

	/* 添加到链表中 */
	__add_list(new_data);

	pthread_mutex_unlock(&net_data_lock);

	return 0;
}



/********************************************
函数名:	del_net_data_list
功能:		把数据从网络数据链表中删除
********************************************/

int del_net_data_list(char *buf, int len)
{
	struct check_head *head, *data_head;

	struct net_data *now_data = NULL, *tmp_data = NULL;

	head = (struct check_head *)buf;

	pthread_mutex_lock(&net_data_lock);

	now_data = net_data_list;

	while(now_data != NULL){
		data_head = (struct check_head *)now_data->data;
		//if((memcmp(data_head->timecnt, head->timecnt, 6) == 0)){
		if(data_head->key == head->key){
			__del_list(tmp_data, now_data);
//			printf("ack is ok \r\n");
			pthread_mutex_unlock(&net_data_lock);
			return 0;
		}
		tmp_data = now_data;
		now_data = now_data->ptNext;
	}

	pthread_mutex_unlock(&net_data_lock);
	return -1;
}


/********************************************
函数名:	__test
功能:		测试
********************************************/

void __test(void)
{
	int i;
	struct check_head data;

	for(i = 0; i < 10; i++)
	{
		data.timecnt[0] = i;
		add_net_data_list((char *)&data, sizeof(struct check_head), NULL);
	}
	__printf_net_data_list();

	dbg_printf("/************* del 4 *************/\r\n");

	data.timecnt[0] = 4;
	del_net_data_list((char *)&data, sizeof(struct check_head));
	__printf_net_data_list();

	dbg_printf("/************* del all *************/\r\n");
	
	for(i = 9; i >= 0; i--)
	{
		data.timecnt[0] = i;
		del_net_data_list((char *)&data, sizeof(struct check_head));
		__printf_net_data_list();
	}
	__printf_net_data_list();
}



/********************************************
函数名:	__send_again
功能:		再尝试发送一次
********************************************/

void __send_again(struct net_data *net_dat)
{
//	struct check_head *head;

//	head = (struct check_head *)(net_dat->data);
//	dbg_printf("__send_again to %s get %s\r\n", 
//					inet_ntoa((net_dat->serveraddr).sin_addr),
//					head->AID);
	p2p_sendto(sys_cfg.sockfd, (char *)(net_dat->data), net_dat->data_len, 0, (struct sockaddr *)&(net_dat->serveraddr),
		   sizeof(struct sockaddr_in));
}


/********************************************
函数名:	net_data_send_err
功能:		对重复三次都发送失败的网络数据进行处理
		如果是服务器中转:

		如果是P2P 通信失败，则走服务器中转，
			同时标记该客户端P2P不可用
********************************************/

void net_data_send_err(struct net_data *net_dat)
{
//	struct check_head *head;
//	struct net_data *new_data;
//	char *data;

//	head = (struct check_head *)(net_dat->data);

//	dbg_printf("net_data_send_err\r\n");

	

#if 0
	if(head->affairs== AFF_SEND_CLIENT)		/* 服务器中转失败 */
	{
		
	}else if(head->affairs == AFF_CLIENT_DATA_P2P)	/* P2P失败 */
	{
		struct sockaddr_in serveraddr;
	
		serveraddr.sin_family = AF_INET;
	    serveraddr.sin_addr.s_addr = inet_addr(sys_cfg.handle_ip);
	    serveraddr.sin_port = htons(sys_cfg.handle_port);

		p2p_send_err(net_dat->data, net_dat->data_len);
		/* 尝试通过服务器转发 */
		compages_head(head, AFF_SEND_CLIENT);

		new_data = (struct net_data*)malloc(sizeof(struct net_data));
		if(new_data == NULL)
			return ;

		data = (char *)malloc(net_dat->data_len);
		if(data == NULL){
			free(new_data);
			return ;
		}

		memcpy(new_data, net_dat, sizeof(struct net_data));
		memcpy(data, net_dat->data, net_dat->data_len);
		memcpy(&(new_data->serveraddr), &serveraddr, sizeof(struct sockaddr_in));

		new_data->data = data;
		new_data->cnt = 3;
		new_data->time = 0;

		dbg_printf("net_data_send_err 2\r\n");
		/* 添加到链表中 */
		__add_list(new_data);
	}
#endif
}


/********************************************
函数名:	net_data_list_pthread
功能:		线程。用于处理网络数据包的重发机制
********************************************/

void *net_data_list_pthread(void *pdata)
{
	struct net_data *now_data = NULL, *tmp_data = NULL, *t_data;

	/* 初始化锁 */
	
//	__test();

	while(1)
	{

//		now_data = NULL;
		tmp_data = NULL;
		t_data 	= NULL;

		pthread_mutex_lock(&net_data_lock);
		
		now_data = net_data_list;

		while(now_data != NULL){
			if(now_data->time > 0){
				now_data->time --;
				goto out1;
			}
			if(now_data->cnt > 0){
				now_data->cnt --;
				now_data->time = 3;
				/* 重新发送一次 */
				__send_again(now_data);
				//dbg_printf("__send_again \r\n");
				goto out1;
			}else if(now_data->cnt == 0){		//重复了3次还是无应答，则可能失败了
				net_data_send_err(now_data);
			}
			/* 删除 */
			t_data = now_data->ptNext;
			if(tmp_data == NULL)	//没有上一个，说明data是第一个
			{
				net_data_list = now_data->ptNext;
			}else{
				tmp_data->ptNext = now_data->ptNext;
			}
			free(now_data->data);
			free(now_data);
			
			now_data = t_data;
			continue;
out1:
			tmp_data = now_data;
			now_data = now_data->ptNext;
		}

		pthread_mutex_unlock(&net_data_lock);
		sleep(2);
	}
}


/********************************************
函数名:	net_data_list_pthread
功能:		初始化网络数据队列的线程
********************************************/

int init_net_data_list(void)
{
	pthread_t thread;
    pthread_attr_t thread_attr;

	/* 创建子进程 */
	pthread_attr_init(&thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&thread, &thread_attr, net_data_list_pthread, NULL) < 0)
	{
		perror("pthread_create");
	}
	return 0;
}


