

#include "all.h"
#include "server.h"



/**************************************
函数名: recv_pthread
功能:		接收线程
***************************************/
void *server_recv_pthread(void *pdata)
{	
	int i;
	
	struct sockaddr_in clientaddr;
//	  struct message recvbuf;
	char recvbuf[UDP_PACK_MAX_SIZE];
	struct check_head *head;
	int ret;
	int clilen;

	/* 获取 sockert 套签字 */
	int sockfd = server_info.sockfd;			/* 获取fd */

	app_printf("recv_pthread %d... \n", sockfd);
	while(1)
	{
		
		clilen = sizeof(clientaddr);
		ret = p2p_recvfrom(sockfd, (char *)recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&clientaddr, (socklen_t *)&clilen);


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

//			printf_client(&clientaddr);

			/* 权限检查 */

//			dbg_printf("addr %p", val);

			for(i = 0; aff_table[i].aff_fun != NULL; i++)
			{
				if(aff_table[i].aff == head->affairs)
				{
					aff_table[i].aff_fun(sockfd, recvbuf, ret, &clientaddr);
				}
			}
		}
	}
}


/*******************************************
函数名:	__init_socket
功能:		初始化服务器socket
*********************************************/

int __init_socket(unsigned int port)
{
    int sockfd;
    struct sockaddr_in serveraddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    return sockfd;
}





/*******************************************
函数名:	check_Active_pthread
功能:		主要是用于检查登陆用户的活跃状态，
			对于不活跃者，T 出
*********************************************/
void *check_Active_pthread(void *pdata)
{
	struct value *v, *found;
	struct key *kk;
	struct hashtable_itr *itr;
	
	while(1){

//		pthread_mutex_lock(&hashtable_lock);
		if (hashtable_count(h_table) > 0)
		{
			itr = hashtable_iterator(h_table);
			do {
				v = hashtable_iterator_value(itr);
				kk = hashtable_iterator_key(itr);

				if(v->cli_info.login_cnt > 0){
					v->cli_info.login_cnt -= 30;
				}else{
					app_printf("remove %s\r\n", kk->name);

					/* 在数据库中标记为不在线 */
					sql_table_update_string("device", "name", kk->name, "is_online", "offline");
					sql_table_update_string("system", "name", kk->name, "is_online", "offline");
					sql_table_update_string("network", "name", kk->name, "is_online", "offline");
					sql_table_update_string("other", "name", kk->name, "is_online", "offline");
					
					if (NULL == (found = __remove_some(h_table,kk))) {
            			app_printf("BUG: key not found for removal\n");
        			}
				}
			} while (hashtable_iterator_advance(itr));
			free(itr);
		}
//		pthread_mutex_unlock(&hashtable_lock);
		
		sleep(SYNC_TIME);
	}
}


/***********************************************
函数名: server_sock_init
功能:		初始化服务器的socket 线程 
***********************************************/
void init_server_sock_pthread(void)
{
	pthread_t recv_thread;
    pthread_attr_t recv_thread_attr;

	server_info.sockfd = __init_socket(SERVER_SYNC_PORT);
	/* 创建子进程 */
	pthread_attr_init(&recv_thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&recv_thread_attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&recv_thread, &recv_thread_attr, server_recv_pthread, NULL) < 0)
	{
		perror("pthread_create");
	}

#if 1
	/* 检查客户端是否活跃 */
	pthread_attr_init(&recv_thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&recv_thread_attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&recv_thread, &recv_thread_attr, check_Active_pthread, NULL) < 0)
	{
		perror("pthread_create");
	}
#endif
}


