
/********************************************************
	服务器链表
	支持多台服务器同时工作
*******************************************************/

#include "all.h"
#include "server.h"
#include "server_list.h"

#include "my_list.h"

#include "timer_list.h"

extern  struct json_object *server_conf_json;

//#include "my_list.h"
struct list_head server_list;		//定义并初始化一个链表头

/* 需加上互斥锁 */
static pthread_mutex_t server_lock = PTHREAD_MUTEX_INITIALIZER;


/* 增加定时器 */
void add_server_list(struct server_node *node)
{
	pthread_mutex_lock(&server_lock);
	list_add(&(node->list), &server_list);
	pthread_mutex_unlock(&server_lock);
}


/* 定时器链表初始化 */
void server_list_init(void)
{	
	char str[50];
	struct json_object *server_obj = json_object_object_get(server_conf_json, "server_list");

	if(server_obj == NULL)
		return ;

	INIT_LIST_HEAD(&server_list);
	
	/* 取出所有 */
	__json_object_object_foreach(server_obj, key, val)
	{
		
		struct server_node *server;
		server = (struct server_node *)malloc(sizeof(struct server_node));

		if(server == NULL)
		{
			return ;
		}
		
		/* 取出所有元素 */
		__json_key_to_string(val, "name", server->name, 100);
		__json_key_to_string(val, "ip", server->ip, 50);
		__json_key_to_int(val, "port", &(server->port));
		
		//加入到链表中
		add_server_list(server);
	}

	printf_server_list();

	/* 服务器群同步信号 */
	server_list_sync_init();
}



/* 打印定时器链表 */
void printf_server_list(void)
{
	pthread_mutex_lock(&server_lock);
	
	struct server_node *pos,*n;
	/* 遍历*/
	list_for_each_entry_safe_reverse(pos,n,&server_list, list, struct server_node)
	{
		printf("name is [%s]\r\n", pos->name);
		printf("ip   is [%s]\r\n", pos->ip);
		printf("port is [%d]\r\n", pos->port);
		printf("\r\n");
	}

	pthread_mutex_unlock(&server_lock);
}




/* 发送同步信号 */
void server_list_sync(void *pdata)
{
	pthread_mutex_lock(&server_lock);
	
	struct server_node *pos,*n;
	/* 遍历*/
	list_for_each_entry_safe_reverse(pos,n,&server_list, list, struct server_node)
	{
		int ret;
		struct sockaddr_in servaddr;
		struct check_head head;
		char sendbuf[1024];
		int send_len;

	    servaddr.sin_family = AF_INET;
	    servaddr.sin_addr.s_addr = inet_addr(pos->ip);
	    servaddr.sin_port = htons(pos->port);

		compages_head(&(head), _aff_server_sync_);

		memcpy(sendbuf, &head, sizeof(head));
		send_len = sizeof(head);

		ret = p2p_sendto(server_info.sockfd, sendbuf, send_len, 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
		if(ret == -1){
			perror("sendto ");
		}
	}

	pthread_mutex_unlock(&server_lock);

	time_t timep;
	time (&timep);
	printf("%s server_list_sync\r\n",asctime(gmtime(&timep)));
	
	struct my_timer timer;
	timer.sec = 15;
	timer.handle = server_list_sync;
	add_my_timer(&timer);
}


/* 服务器群同步信号 */
void server_list_sync_init(void)
{
	struct my_timer timer;
	timer.sec = 15;
	timer.handle = server_list_sync;
	add_my_timer(&timer);
}

