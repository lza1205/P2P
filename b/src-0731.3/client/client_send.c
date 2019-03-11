

#include "all.h"
#include "client.h"

int send_to_server(char *sendbuf, int send_len)
{
	int ret;
	struct sockaddr_in serveraddr;
	
	serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(sys_cfg.serverip);
    serveraddr.sin_port = htons(sys_cfg.ser_port);

	ret = p2p_sendto(sys_cfg.sockfd, (char *)sendbuf, send_len, 0, (struct sockaddr *)&serveraddr,
		   sizeof(serveraddr));

	/* 把数据添加到队列中去 */
	add_net_data_list(sendbuf, send_len, &serveraddr);
	return ret;
}


/*
获取某个客户端的信息
*/
int get_client_info(char *name)
{
	struct sockaddr_in serveraddr;
	
	struct check_head head;
	int ret;
	char sendbuf[1024];
	int send_len;

	
	serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(sys_cfg.serverip);
    serveraddr.sin_port = htons(sys_cfg.ser_port);

	/* 构造包头 */
	compages_head(&(head), _aff_client_get_client_);

	struct proto_c_get_lient proto;
	__strcpy((proto.name), name, USER_NAME_LEN);

	memcpy(sendbuf, &head, sizeof(head));
	memcpy(sendbuf + sizeof(head), &proto, sizeof(proto));
	send_len = sizeof(head) + sizeof(proto);

	ret = p2p_sendto(sys_cfg.sockfd, (char *)sendbuf, send_len, 0, (struct sockaddr *)&serveraddr,
		   sizeof(serveraddr));

	/* 把数据添加到队列中去 */
	add_net_data_list(sendbuf, send_len, &serveraddr);

	return ret;
}


#if 0

/* 通过服务器中转的方式往某个客户端发送消息 */
int send_client_data(char *name, char *data, int len)
{
	struct proto_c_send_data proto;
	struct check_head head;

	char sendbuf[1204];
	int send_len;
	int ret;
	
	struct sockaddr_in serveraddr;

	serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(sys_cfg.serverip);
    serveraddr.sin_port = htons(sys_cfg.ser_port);

	/* 构造包头 */
	compages_head(&(head), _aff_client_send_data_);

	__strcpy((proto.dest_name), name, USER_NAME_LEN);
	__strcpy((proto.src_name), sys_cfg.myname, USER_NAME_LEN);

	memcpy(sendbuf, &head, sizeof(head));
	memcpy(sendbuf + sizeof(head), &proto, sizeof(proto));
	send_len = sizeof(head) + sizeof(proto);
	memcpy(sendbuf + send_len, data, len);
	send_len += len;

	ret = p2p_sendto(sys_cfg.sockfd, (char *)sendbuf, send_len, 0, (struct sockaddr *)&serveraddr,
		   sizeof(serveraddr));

	/* 把数据添加到队列中去 */
	add_net_data_list(sendbuf, send_len, &serveraddr);

	return ret;
}

#endif



/*----------------------------------------------------------------------*/
 pthread_mutex_t send_mtx = PTHREAD_MUTEX_INITIALIZER;
 pthread_cond_t send_cond = PTHREAD_COND_INITIALIZER;


int send_ack_flg = 0;	//是否有应答
struct check_head ack_head;		//接收到的应答头部

/*
阻塞发送
wait_flg  = 1  阻塞发送
		= 0 不阻塞
*/
int __send_client_data(char *name, char *data, int len, int wait_flg, int delay_time)
{
	int ret;

	struct proto_c_send_data proto;
	struct check_head head;

	char sendbuf[1204];
	int send_len;
	
	struct sockaddr_in serveraddr;

	serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(sys_cfg.serverip);
    serveraddr.sin_port = htons(sys_cfg.ser_port);

	/* 构造包头 */
	compages_head(&(head), _aff_client_send_data_);

	__strcpy((proto.dest_name), name, USER_NAME_LEN);
	__strcpy((proto.src_name), sys_cfg.myname, USER_NAME_LEN);

	memcpy(sendbuf, &head, sizeof(head));
	memcpy(sendbuf + sizeof(head), &proto, sizeof(proto));
	send_len = sizeof(head) + sizeof(proto);
	memcpy(sendbuf + send_len, data, len);
	send_len += len;

	ret = p2p_sendto(sys_cfg.sockfd, (char *)sendbuf, send_len, 0, (struct sockaddr *)&serveraddr,
		   sizeof(serveraddr));

	/* 把数据添加到队列中去 */
	add_net_data_list(sendbuf, send_len, &serveraddr);

	if(wait_flg == 0)
		return ret;

	if(wait_ack_head(&head, delay_time) == 0)
	{
		return ret;
	}else{
		return -1;
	}
}




LIST_HEAD(ack_head_list);		//定义并初始化一个链表头
volatile int ack_head_node_num = 0;
/* 需加上互斥锁 */
static pthread_mutex_t ack_head_list_lock = PTHREAD_MUTEX_INITIALIZER;



/* 等待该应答包 */
int wait_ack_head(struct check_head *head, int delay_time)
{	
	int ret;
	
	pthread_mutex_lock(&send_mtx);

	struct timespec to;
	to.tv_sec = time(NULL) + delay_time;
	to.tv_nsec = 0;
		
loop:

	ret = pthread_cond_timedwait(&send_cond,&send_mtx, &to);	//等待条件.超时退出
	if(ret == ETIMEDOUT)
	{				
		pthread_mutex_unlock(&send_mtx);//临界区数据操作完毕，释放互斥锁
		return -1;		//超时退出
	}

	struct recv_head_node *pos,*n;

	printf("1\r\n");	
	pthread_mutex_lock(&ack_head_list_lock);
	/* 比较下应答包是否是一样的 */
	list_for_each_entry_safe_reverse(pos,n,&ack_head_list,list, struct recv_head_node)
	{
		if(pos->head.key == head->key)		//相等
		{
			list_del(&(pos->list));
			ack_head_node_num --;
			free(pos);			
			pthread_mutex_unlock(&ack_head_list_lock);
			goto out1;
		}
	}
	pthread_mutex_unlock(&ack_head_list_lock);

	printf("2\r\n");

	/* 不相等 继续等待 */
	to.tv_sec = time(NULL) + delay_time ;
	to.tv_nsec = 0;
	goto loop;	

out1:
	printf("3\r\n");
	pthread_mutex_unlock(&send_mtx);//临界区数据操作完毕，释放互斥锁

	return 0;
}



void add_ack_list(struct check_head *head)
{
	struct recv_head_node *pos,*n;
	struct recv_head_node *node;

	/* 不要存放太多节点信息 */
	pthread_mutex_lock(&ack_head_list_lock);
	if(ack_head_node_num > 50)
	{		
		list_for_each_entry_safe_reverse(pos,n,&ack_head_list,list, struct recv_head_node)
		{
			list_del(&(pos->list));
			free(pos);
			ack_head_node_num --;
			goto out1;
		}
	}
out1:

	node = malloc(sizeof(struct recv_head_node));
	if(node == NULL)
		goto out2;
	
	memcpy(&(node->head), head, sizeof(struct check_head));
	list_add(&(node->list), &ack_head_list);
	ack_head_node_num ++;
	
out2:
	pthread_mutex_unlock(&ack_head_list_lock);

}




/* 唤醒发送队列 */
void recv_ack_wakeup(struct check_head *head)
{
#if 1
	pthread_mutex_lock(&send_mtx);//需要操作head这个临界资源，先加锁，

	add_ack_list(head);
	
	pthread_cond_broadcast(&send_cond);		//唤醒所有线程

	pthread_mutex_unlock(&send_mtx);//解锁
#endif
}

















/* 通过服务器中转的方式往某个客户端发送消息 */
int send_client_data(char *name, char *data, int len)
{
	__send_client_data(name, data, len, 0, 0);
}


/* 阻塞发送 */
int send_client_data_2(char *name, char *data, int len, int delay_time)
{
	__send_client_data(name, data, len, 1, delay_time);
}

