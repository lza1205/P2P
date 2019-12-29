
#define __SERVER_CLIENT_NODE_LIST_C_

#include "all.h"
#include "server.h"
#include "my_list.h"
#include "server_client_node_list.h"

/* 这些函数涉及到多线程并发问题，不好处理 */
/* 需加上读写锁 */
static pthread_rwlock_t server_client_node_lock;

LIST_HEAD(server_client_node_list);		//定义并初始化一个链表头


void server_client_node_list_init(void)
{
	pthread_rwlock_init(&server_client_node_lock, NULL);
}

int add_server_client_node(void *pfd, char *name)

{
	struct server_client_node *node;

	node = malloc(sizeof(struct server_client_node));
	if (node != NULL)
	{
		//unsigned char name[30];
		//__buf_to_str(name, info->name, 8);
		memcpy(node->name, name, USER_NAME_LEN);
		node->pfd = pfd;
		printf("%s %d %p \r\n", __FILE__, __LINE__, pfd);

		printf("%s %d \r\n", __FILE__, __LINE__);
		pthread_rwlock_wrlock(&server_client_node_lock);
		printf("%s %d \r\n", __FILE__, __LINE__);
		list_add(&node->list,&server_client_node_list);
		printf("%s %d \r\n", __FILE__, __LINE__);
		pthread_rwlock_unlock(&server_client_node_lock);
		printf("%s %d \r\n", __FILE__, __LINE__);
		return 0;
	}
	return -1;
}


void del_server_client_node(void *pfd)
{
	struct server_client_node *pos; 

	printf("%s %d \r\n", __FILE__, __LINE__);
	pthread_rwlock_wrlock(&server_client_node_lock);
	printf("%s %d \r\n", __FILE__, __LINE__);
	list_for_each_entry_reverse(pos,&server_client_node_list,list, struct server_client_node)
	{
		if(pos->pfd == pfd)
		{
			list_del(&pos->list);
			printf("%s %d %p \r\n", __FILE__, __LINE__, pos->pfd);
			free(pos);
			pthread_rwlock_unlock(&server_client_node_lock);//解锁
			return ;
		}
	}
	printf("%s %d \r\n", __FILE__, __LINE__);
	pthread_rwlock_unlock(&server_client_node_lock);//解锁
	return ;

}

void printf_server_client_list(void)
{
	struct server_client_node *pos; 
	int i = 0;

	pthread_rwlock_rdlock(&server_client_node_lock);
	list_for_each_entry_reverse(pos,&server_client_node_list,list, struct server_client_node)
	{
		i ++;
		printf("[%d] name %s\n",i, pos->name);
		printf("\r\n");
	}
	pthread_rwlock_unlock(&server_client_node_lock);//解锁
}



void *find_server_client_socket(char *name)
{
	struct server_client_node *pos; 

	printf("%s %d \r\n", __FILE__, __LINE__);
	pthread_rwlock_rdlock(&server_client_node_lock);
	printf("%s %d \r\n", __FILE__, __LINE__);
	list_for_each_entry_reverse(pos,&server_client_node_list,list, struct server_client_node)
	{
		if(memcmp(pos->name, name, USER_NAME_LEN) == 0)
		{
			pthread_rwlock_unlock(&server_client_node_lock);//解锁
			printf("%s %d %p \r\n", __FILE__, __LINE__, pos->pfd);
			return pos->pfd;
		}
	}
	printf("%s %d \r\n", __FILE__, __LINE__);
	pthread_rwlock_unlock(&server_client_node_lock);//解锁
	return NULL;
}



int find_server_client_name(void * pfd, char *name)
{
	struct server_client_node *pos; 

	printf("%s %d \r\n", __FILE__, __LINE__);
	pthread_rwlock_rdlock(&server_client_node_lock);
	printf("%s %d \r\n", __FILE__, __LINE__);
	list_for_each_entry_reverse(pos,&server_client_node_list,list, struct server_client_node)
	{
		if(pos->pfd == pfd)
		{
			memcpy(name, pos->name, USER_NAME_LEN);
			pthread_rwlock_unlock(&server_client_node_lock);//解锁
			printf("%s %d %p \r\n", __FILE__, __LINE__, pos->pfd);
			return 0;
		}
	}
	printf("%s %d \r\n", __FILE__, __LINE__);
	pthread_rwlock_unlock(&server_client_node_lock);//解锁
	return -1;
}



