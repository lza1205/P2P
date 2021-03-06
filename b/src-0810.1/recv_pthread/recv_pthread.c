
#include "recv_pthread.h"
#include <sys/time.h>

/*
模板
*/
static pthread_mutex_t mb_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t mb_cond = PTHREAD_COND_INITIALIZER;

int my_pthread_mutex_init(pthread_mutex_t *mtx, char *p)
{
	memcpy(mtx, &mb_mtx, sizeof(pthread_mutex_t));
	return 0;
}

int my_pthread_cond_init(pthread_cond_t *cond, char *p)
{
	memcpy(cond, &mb_cond, sizeof(pthread_cond_t));
	return 0;
}



LIST_HEAD(recv_pthread_list);		//定义并初始化一个链表头

/* 需加上互斥锁 */
static pthread_mutex_t recv_pthread_list_lock = PTHREAD_MUTEX_INITIALIZER;


/* 监听相关的 */
static pthread_mutex_t listen_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t listen_cond = PTHREAD_COND_INITIALIZER;

/* poll 机制相关的 */
static pthread_mutex_t poll_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t poll_cond = PTHREAD_COND_INITIALIZER;


/* 唤醒所有poll 线程 */
static void poll_cond_broadcast(void)
{
	pthread_mutex_lock(&poll_mtx);//需要操作head这个临界资源，先加锁，
	pthread_cond_broadcast(&poll_cond);		//唤醒条件
	pthread_mutex_unlock(&poll_mtx);//解锁
}



static int recv_list_num, recv_list_max;


void add_data_to_list(
	struct listen *recv_list, 
	struct sockaddr *addr,		/* 数据包地址信息 */
	char *data,					/* 数据包内容 */
	int len					/* 数据包长度 */
);

void add_recv_pthread_list(
	struct sockaddr *addr,		/* 数据包地址信息 */
	char *buf,					/* 数据包内容 */
	int len					/* 数据包长度 */
)
{
	int ret;
	struct listen *pos,*n;
	
	/* 不要存放太多节点信息 */
	pthread_mutex_lock(&recv_pthread_list_lock);
	
	list_for_each_entry_safe_reverse(pos, n, &recv_pthread_list, listen_list, struct listen)
	{
		if(memcmp(addr, &(pos->addr), sizeof(struct sockaddr)) == 0)
		{
			/* 地址相同。复制数据到该队列中去 */
			add_data_to_list(pos, addr, buf, len);
			poll_cond_broadcast();			//唤醒 poll 线程

			pthread_mutex_unlock(&recv_pthread_list_lock);
			return ;
		}
	}
	if(recv_list_num < recv_list_max)
	{
		/* 创建新的接收链表头 */
		struct listen *head;
		head = (struct listen *)malloc(sizeof(struct listen));
		if(head == NULL)
			goto out1;

		/* 初始化线程锁 */
		ret = my_pthread_mutex_init(&head->mutex, NULL);
		if(ret != 0)
		{
			free(head);
			goto out1;
		}

		/* 初始化条件变量相关的 */
		ret = my_pthread_cond_init(&head->recv_cond, NULL);
		if(ret != 0)
		{
			free(head);
		}
		ret = my_pthread_mutex_init(&head->recv_mtx, NULL);
		if(ret != 0){
			free(head);
		}
		
		/* 初始化链表头 */
		INIT_LIST_HEAD(&(head->head));
		
		head->data_num = 0;
		head->data_max_num = 10;	//最多存放10个数据

		/* 复制地址信息 */
		memcpy(&(head->addr), addr, sizeof(struct sockaddr));

		/* 加到队列中去 */
		list_add(&(head->listen_list), &recv_pthread_list);
		recv_list_num ++;

		add_data_to_list(head, addr, buf, len);
		poll_cond_broadcast();			//唤醒 poll 线程

		pthread_mutex_unlock(&recv_pthread_list_lock);

		/* 唤醒监听 */
		pthread_mutex_lock(&listen_mtx);//需要操作head这个临界资源，先加锁，

		head->list_flg = 1;		/* 需要上报 */
		pthread_cond_signal(&listen_cond);		//唤醒条件

		pthread_mutex_unlock(&listen_mtx);//解锁

		return ;
		
	}


out1:
	pthread_mutex_unlock(&recv_pthread_list_lock);
	return ;
}




void add_data_to_list(
	struct listen *recv_list, 
	struct sockaddr *addr,		/* 数据包地址信息 */
	char *buf,					/* 数据包内容 */
	int len					/* 数据包长度 */
)
{
	char *new_data;

	/* 数据链表最大长度 */
	pthread_mutex_lock(&recv_list->mutex);

	if(recv_list->data_num >= recv_list->data_max_num)
	{
		pthread_mutex_unlock(&recv_list->mutex);
		return ;
	}

	pthread_mutex_unlock(&recv_list->mutex);


	/* 创建新的接收链表头 */
	struct recv_data *node;
	node = (struct recv_data *)malloc(sizeof(struct recv_data));
	if(node == NULL)
		return ;


	new_data = (char *)malloc(len);
	if(new_data == NULL)
	{
		free(node);
		return ;
	}
	

	memcpy(&(node->addr), addr, sizeof(struct sockaddr));
	
	node->len = len;
	memcpy(new_data, buf, len);
	node->data = new_data;

	pthread_mutex_lock(&recv_list->mutex);

	/* 加到队列中去 */
	list_add(&(node->data_list), &(recv_list->head));
	recv_list->data_num ++;

	pthread_mutex_unlock(&recv_list->mutex);

	
	/* 唤醒线程 */
	pthread_mutex_lock(&recv_list->recv_mtx);
	pthread_cond_signal(&recv_list->recv_cond);		//唤醒条件
	pthread_mutex_unlock(&recv_list->recv_mtx);	
}


#define __max(a,b)        (((a) > (b)) ? (a) : (b))
#define __min(a,b)        (((a) < (b)) ? (a) : (b))

int recv_from_listen(	
	struct listen *recv_list, 
	struct sockaddr *addr,		/* 数据包地址信息 */
	char *buf,					/* 数据包内容 */
	int len,					/* 数据包长度 */
	int delay_time				/* 休眠时间。 -1表示永久休眠 */
)
{
	int ret;

	pthread_mutex_lock(&recv_list->recv_mtx);
	
	while(recv_list->data_num == 0)
	{
		if(delay_time == -1)
		{
			pthread_cond_wait(&recv_list->recv_cond, &recv_list->recv_mtx);	//等待条件
		}else{
			struct timespec to;
			to.tv_sec = time(NULL) + delay_time;
			to.tv_nsec = 0;
			ret = pthread_cond_timedwait(&recv_list->recv_cond, &recv_list->recv_mtx, &to);	//等待条件.超时退出
			if(ret == ETIMEDOUT)
			{				
				pthread_mutex_unlock(&recv_list->recv_mtx);//临界区数据操作完毕，释放互斥锁
				return -1;		//超时退出
			}
		}
	}


	pthread_mutex_lock(&recv_list->mutex);

	/* 从队列中取出数据 */
	ret = -1;
	struct recv_data *pos,*n;
	
	list_for_each_entry_safe_reverse(pos,n,&(recv_list->head), data_list, struct recv_data)
	{
		/* 复制数据 */
		ret = __min(len, pos->len);
		memcpy(buf, pos->data, ret);
		/* 复制地址 */
		memcpy(addr, &(pos->addr), sizeof(struct sockaddr_in));

		/* 删除队列 */
		list_del(&(pos->data_list));

		/* 释放内存 */
		free(pos->data);
		free(pos);

		recv_list->data_num --;

		goto out1;
	}

out1:
	pthread_mutex_unlock(&recv_list->mutex);

	
	pthread_mutex_unlock(&recv_list->recv_mtx);//临界区数据操作完毕，释放互斥锁

	return ret;
}



/* 获取一个连接 */
struct listen *server_accept(void)
{	
	struct listen *pos,*n;

	pthread_mutex_lock(&listen_mtx);	

loop:	
	/* 找到还未被监听的 */
	
	pthread_mutex_lock(&recv_pthread_list_lock);

	
	list_for_each_entry_safe_reverse(pos, n, &recv_pthread_list, listen_list, struct listen)
	{
		if(pos->list_flg == 1)		
		{
			/* 需要被监听 */
			pos->list_flg = 0;
			pthread_mutex_unlock(&recv_pthread_list_lock);

			pthread_mutex_unlock(&listen_mtx);
			return pos;
		}
	}
	pthread_mutex_unlock(&recv_pthread_list_lock);

	/* 等待条件唤醒 */

	pthread_cond_wait(&listen_cond, &listen_mtx);	//等待条件

	goto loop;

	pthread_mutex_unlock(&listen_mtx);	
	return NULL;
}



/*
关闭某个客户端
*/
int listen_close(struct listen *listen)
{
	pthread_mutex_lock(&recv_pthread_list_lock);


	/* 释放内存 */	
	pthread_mutex_lock(&listen->mutex);
	struct recv_data *pos,*n;

	list_for_each_entry_safe_reverse(pos,n,&listen->head, data_list, struct recv_data)
	{
		/* 删除队列 */
		list_del(&(pos->data_list));

		/* 释放内存 */
		free(pos->data);
		free(pos);

		list_del(&(listen->listen_list));
		recv_list_num --;

		free(listen);
	}
	pthread_mutex_unlock(&listen->mutex);
	
	pthread_mutex_unlock(&recv_pthread_list_lock);
}


void *server_recv_pthread(void *pdata)
{
	int i;
	
	struct sockaddr_in clientaddr;
	char recvbuf[4096];
	int ret;
	int clilen;

	/* 获取 sockert 套签字 */
	int *fdp = pdata;
	int sockfd = (int)(*fdp);			/* 获取fd */

	while(1)
	{
		clilen = sizeof(clientaddr);
		ret = recvfrom(sockfd, (char *)recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clilen);

		if (ret < 0)
		{
			perror("my_recvfrom_handle");
			continue;
		}
		else
		{
			/* 发送到等待队列中去 */
			add_recv_pthread_list((struct sockaddr *)&clientaddr,(char *)recvbuf, ret);
		}
	}
}

/*
开始监听
*/
void server_listen(int *sockfd, int num)
{	
	pthread_t recv_thread;
    pthread_attr_t recv_thread_attr;

	recv_list_max = num;

	/* 创建子进程 */
	pthread_attr_init(&recv_thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&recv_thread_attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&recv_thread, &recv_thread_attr, server_recv_pthread, sockfd) < 0)
	{
		perror("pthread_create");
		exit(1);
	}
}




/*
创建处理线程
*/
void listen_pthread(listen_t *listen, void *(*start_rtn)(void *))
{	
	pthread_t recv_thread;
    pthread_attr_t recv_thread_attr;

	/* 创建子进程 */
	pthread_attr_init(&recv_thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&recv_thread_attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&recv_thread, &recv_thread_attr, start_rtn, listen) < 0)
	{
		perror("pthread_create");
		exit(1);
	}
}





/*--------------------------------------------------------------------*/
/*
以下为扩展功能
*/
/*--------------------------------------------------------------------*/


/**********************************************************************
实现同时接收多个 listen 的数据
**********************************************************************/

/*
初始化一个 listen 队列头
*/
void listen_head_init(struct list_head *head)
{
	INIT_LIST_HEAD(head);
}


/* 
将要监听的listen 加入到这个head 链表中
*/
int listen_add(struct list_head *head, listen_t *listen)
{
	list_add(&(listen->poll_list), head);
}


int recv_from_listen_head(
		struct list_head* head,
		struct listen **recv_listen, 
		struct sockaddr *addr,		/* 数据包地址信息 */
		char *buf,					/* 数据包内容 */
		int len,					/* 数据包长度 */
		int delay_time				/* 休眠时间。 -1表示永久休眠 */
	)
{
	int ret;
	
	/* 先看下有没有数据。没有才进入等待 */
	/* 从链表中找到数据 */
	listen_t *pos,*n;
	
	list_for_each_entry_safe_reverse(pos,n, head, poll_list, listen_t)
	{
		
		(*recv_listen) = pos;
		ret = recv_from_listen(pos, addr, buf, len, 0);
		if(ret >= 0)
		{
			return ret;
		}
	}

	//printf("poll 3 \r\n");

	pthread_mutex_lock(&poll_mtx);

loop:
	if(delay_time == -1)
	{
		pthread_cond_wait(&poll_cond, &poll_mtx); //等待条件
	}else{
		struct timespec to;
		to.tv_sec = time(NULL) + delay_time;
		to.tv_nsec = 0;
		ret = pthread_cond_timedwait(&poll_cond, &poll_mtx, &to); //等待条件.超时退出
		if(ret == ETIMEDOUT)
		{				
			pthread_mutex_unlock(&poll_mtx);//临界区数据操作完毕，释放互斥锁
			return -1;		//超时退出
		}
	}

	
	list_for_each_entry_safe_reverse(pos,n, head, poll_list, listen_t)
	{
		ret = recv_from_listen(pos, addr, buf, len, 0);
		if(ret >= 0)
		{
			(*recv_listen) = pos;
			pthread_mutex_unlock(&poll_mtx);
			return ret;
		}
	}

goto loop;

	pthread_mutex_unlock(&poll_mtx);

	return -1;
}



/*-------------------------------------------------------------------
自己创建一个 listen 接口
-------------------------------------------------------------------*/
int __init_listen(listen_t *listen, struct sockaddr *addr)
{
	int ret;
	/* 初始化线程锁 */
	ret = my_pthread_mutex_init(&listen->mutex, NULL);
	if(ret != 0)
	{
		free(listen);
		return -1;
	}
	
	/* 初始化条件变量相关的 */
	ret = my_pthread_cond_init(&listen->recv_cond, NULL);
	if(ret != 0)
	{
		free(listen);
	}
	ret = my_pthread_mutex_init(&listen->recv_mtx, NULL);
	if(ret != 0){
		free(listen);
	}
	INIT_LIST_HEAD(&(listen->head));
	
	listen->data_num = 0;
	listen->data_max_num = 10;	//最多存放10个数据
	
	/* 复制地址信息 */
	memcpy(&(listen->addr), addr, sizeof(struct sockaddr));
	
	/* 加到队列中去 */
	list_add(&(listen->listen_list), &recv_pthread_list);

	return 0;
}

/* 创建一个 listen */
listen_t *create_listen(struct sockaddr *addr)
{
	listen_t *listen = NULL;
	pthread_mutex_lock(&recv_pthread_list_lock);
	if(recv_list_num < recv_list_max)
	{
		/* 创建新的接收链表头 */
		listen = (struct listen *)malloc(sizeof(struct listen));
		if(listen == NULL)
			goto out1;

		/* 初始化 */
		__init_listen(listen, addr);
		recv_list_num ++;
	}

out1:
	pthread_mutex_unlock(&recv_pthread_list_lock);
	return listen;
}




/*-------------------------------------------------------------------
listen 的属性设置
-------------------------------------------------------------------*/
/*
设置listen 的最大接收数量
*/
void set_listen_maxdata(listen_t *listen, int max)
{
	listen->data_max_num = max;
}





