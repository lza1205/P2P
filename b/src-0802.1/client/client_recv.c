#define __CLIENT_RECV_C_

#include "all.h"
#include "client.h"

#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include <sys/time.h>



/********************************************
函数名: client_recv_handle
功能:		客户端udp接收到数据后的第一个处理函数
*********************************************/

void client_recv_handle(void)
{
	int i;
	struct sockaddr_in clientaddr;
	char recvbuf[UDP_PACK_MAX_SIZE];
	struct check_head *head;
	int ret;
	int clilen;

	while(1)
	{
		
		clilen = sizeof(clientaddr);
		ret = p2p_recvfrom(sys_cfg.sockfd, (char *)recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clilen);


		if (ret < 0)
		{
			perror("my_recvfrom_handle");
			continue;
		}
		else
		{
			head = (struct check_head *)recvbuf;

			/* 检查长度 */
			if(ret < sizeof(struct check_head))
				continue;

			/* 对数据包头部进行校验 */
			if(check_head_crc(head) != 0)
				continue;			

//			printf_client(&clientaddr);
//			printf("aff is %d \r\n", head->affairs);
			
			for(i = 0; client_aff_table[i].aff_fun != NULL; i++)
			{
				if(client_aff_table[i].aff == head->affairs)
				{
					client_aff_table[i].aff_fun(sys_cfg.sockfd, recvbuf, ret, &clientaddr);
					break ;
				}
			}
		}
	}

}




/********************************************
函数名: connet_to_server
功能:		向服务器发起UDP 连接请求
*********************************************/
int connet_to_server(char *serverip, int port)
{
	char sendbuf[1024];
	int send_len;
	struct sockaddr_in servaddr;
	struct sockaddr_in recvaddr;
	int nread;
	int ret;
	int fromlen;
	char recvbuf[1024];
    fd_set readfds;
    fd_set writefds;
	struct timeval tv;

	struct check_head head, *recv_head;;
	char *str;

	struct proto_s_login_ack *ack;


	dbg_printf("connet to %s:%d\r\n", serverip, port);

	/* 创建服务器socket 套签字 */
	memset(&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(serverip);
    servaddr.sin_port = htons(port);

	/* 构造包头 */
	compages_head(&head, _aff_client_login_);

	/* 合并数据 */	
	memcpy(sendbuf, &head, sizeof(head));
	send_len = sizeof(head);
	
    ret = p2p_sendto(sys_cfg.sockfd, sendbuf, send_len, 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
	if(ret == -1){
		return -1;
	}

   while(1)
   {
	   
	   int maxfd = sys_cfg.sockfd;
	   
	   FD_ZERO(&readfds);
	   FD_ZERO(&writefds);
	   FD_SET(sys_cfg.sockfd, &readfds);

	   /* 等待5 秒，如果服务器没有返回，那么失败 */
	   tv.tv_sec = 5;
	   tv.tv_usec = 0;
	   ret = select(maxfd + 1, &readfds, &writefds, NULL, &tv);
	   
	   if (ret > 0)
	   {
		   if (FD_ISSET(sys_cfg.sockfd, &readfds)) 
		   {
			   fromlen = sizeof(servaddr);
			   nread = p2p_recvfrom(sys_cfg.sockfd,(char *)&recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&recvaddr, (socklen_t *)&fromlen);
			   if(nread <= 0)
				   return -1;
			   
			   printf_client(&recvaddr);

			   if(recvaddr.sin_addr.s_addr != inet_addr(sys_cfg.serverip))
				   continue;

			   recv_head = (struct check_head *)recvbuf;
			   if((recv_head->affairs == _aff_server_login_ack_))
				   goto login_ack;
		   }
	   }
	   ret = p2p_sendto(sys_cfg.sockfd, sendbuf, send_len, 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
	   if(ret == -1){
		   perror("sendto ");
	   }
   }

login_ack:
	
	ack = (struct proto_s_login_ack *)(recvbuf + sizeof(struct check_head));
	return ack->ack;		//返回登陆情况

}



/********************************************
函数名: sync_service
功能:		用来做同步的子进程
*********************************************/

void *sync_thread_fun(void *pdata)
{
	while(1)
	{
		sync_service(); 			//同步服务器
		sleep(SYNC_TIME);
	}
}



/************************************************
函数名: client_recv_pthread
功能:		客户端接收处理线程
************************************************/
void *client_recv_pthread(void *pdata)
{
	
	pthread_t sync_thread;
    pthread_attr_t sync_thread_attr;

	/* 连接到服务器 */
	connet_to_server(sys_cfg.serverip, sys_cfg.ser_port);

	/* 创建一个同步子线程 */
	pthread_attr_init(&sync_thread_attr);		//初始化进程属性
    pthread_attr_setdetachstate(&sync_thread_attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&sync_thread, &sync_thread_attr, sync_thread_fun, NULL) < 0)
    {
        perror("pthread_create");
    }


	client_recv_handle();

	while(1);
}




/********************************************
函数名: sync_service
功能:		往服务器发送心跳包
*********************************************/

void sync_service(void)
{
	int ret;
	struct sockaddr_in servaddr;
	struct check_head head;
	char sendbuf[1024];
	int send_len;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(sys_cfg.serverip);
    servaddr.sin_port = htons(sys_cfg.ser_port);

	compages_head(&(head), _aff_client_sync_);

	memcpy(sendbuf, &head, sizeof(head));
	send_len = sizeof(head);

	ret = p2p_sendto(sys_cfg.sockfd, sendbuf, send_len, 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
	if(ret == -1){
		perror("sendto ");
	}
}



void init_network_pthread(void)
{
	pthread_t thread;
    pthread_attr_t thread_attr;
	
	pthread_t aff_thread;
    pthread_attr_t aff_thread_attr;

	/* 创建子进程 */
	pthread_attr_init(&thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&thread, &thread_attr, client_recv_pthread, NULL) < 0)
	{
		perror("pthread_create");
	}
}






/* ---------------------------------------------------------------------- */

/* 提供给用户的同步接收数据 */
static pthread_mutex_t recv_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t recv_cond = PTHREAD_COND_INITIALIZER;

/* 队列锁 */
static pthread_mutex_t recv_list_lock = PTHREAD_MUTEX_INITIALIZER;


LIST_HEAD(recv_data_list);		//定义并初始化一个链表头
volatile int recv_data_num = 0;



void __add_recv_data(struct recv_data *r_data)
{
	struct recv_data *pos,*n;
	/* 不要存放太多节点信息 */
	pthread_mutex_lock(&recv_list_lock);
	if(recv_data_num > 50)
	{		
		list_for_each_entry_safe_reverse(pos,n,&recv_data_list,list, struct recv_data)
		{
			list_del(&(pos->list));
			free(pos);
			recv_data_num --;
			goto out1;
		}
	}
out1:
	list_add(&r_data->list, &recv_data_list);
	recv_data_num ++;
	
	pthread_mutex_unlock(&recv_list_lock);
}

/* 把接收到的网络数据包放到队列中去 */
int add_recv_data_list(char *buf, int len, struct sockaddr_in *addr)
{
	struct recv_data *new_data;
	char *data;
	
	new_data = (struct recv_data*)malloc(sizeof(struct recv_data));
	if(new_data == NULL)
		return -1;

	data = (char *)malloc(len);
	if(data == NULL){
		free(new_data);
		return -1;
	}

	/* 复制数据 */
	if(addr != NULL)
		memcpy(&(new_data->recv_addr), addr, sizeof(struct sockaddr_in));

	memcpy(data, buf, len);
	new_data->data = data;
	new_data->data_len = len;

	pthread_mutex_lock(&recv_mtx);//需要操作head这个临界资源，先加锁，

	__add_recv_data(new_data);
	pthread_cond_signal(&recv_cond);		//唤醒条件

	pthread_mutex_unlock(&recv_mtx);//解锁
	return 0;
}



/*
接收网络节点的数据
time 表示等待时间。单位秒
-1 表示永久等待
*/
int recv_node_data(char *buf, int len, struct sockaddr_in *addr, int delay_time)
{
	int ret = -1;

	pthread_mutex_lock(&recv_mtx);

	while(recv_data_num == 0){
		if(delay_time == -1)
		{
			pthread_cond_wait(&recv_cond,&recv_mtx);	//等待条件
		}else{
			struct timespec to;
			to.tv_sec = time(NULL) + delay_time;
			to.tv_nsec = 0;
			ret = pthread_cond_timedwait(&recv_cond,&recv_mtx, &to);	//等待条件.超时退出
			if(ret == ETIMEDOUT)
			{				
				pthread_mutex_unlock(&recv_mtx);//临界区数据操作完毕，释放互斥锁
				return -1;		//超时退出
			}
		}
		
	}

	ret = -1;
	struct recv_data *pos,*n;

	pthread_mutex_lock(&recv_list_lock);
	list_for_each_entry_safe_reverse(pos,n,&recv_data_list,list, struct recv_data)
	{
		/* 复制数据 */
		ret = __min(len, pos->data_len);
		memcpy(buf, pos->data, ret);
		/* 复制地址 */
		memcpy(addr, &(pos->recv_addr), sizeof(struct sockaddr_in));
		
		list_del(&(pos->list));

		/* 释放内存 */
		free(pos->data);
		free(pos);
		
		recv_data_num --;
		goto out1;
	}
out1:
	pthread_mutex_unlock(&recv_list_lock);

	pthread_mutex_unlock(&recv_mtx);//临界区数据操作完毕，释放互斥锁

	return ret;
}


