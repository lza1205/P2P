
#define __CLIENT_INFO_LIST_C_

#include "all.h"
#include "client.h"
#include "my_list.h"

LIST_HEAD(client_node_list);		//���岢��ʼ��һ������ͷ

volatile int client_node_num = 0;


int __add_client_node(struct proto_s_client_info *info)
{
	struct client_node *node;

	node = malloc(sizeof(struct client_node));
	if (node != NULL)
	{
		strcpy(node->name, info->name);
		strcpy(node->ip, info->ip);
		node->status = info->status;
		node->port = atoi(info->port);

		list_add(&node->list,&client_node_list);
		client_node_num ++;
		return 0;
	}
	return -1;
}

int add_client_node(struct proto_s_client_info *info)
{
	struct client_node *pos,*n;
	
	/* ��Ҫ���̫��ڵ���Ϣ */
	if(client_node_num > 50)
	{
		list_for_each_entry_safe_reverse(pos,n,&client_node_list,list, struct client_node)
		{
			list_del(&(pos->list));
			free(pos);
			client_node_num --;
			goto out1;
		}
	}
out1:
	__add_client_node(info);
}

void printf_client_list(void)
{
	struct client_node *pos; 
	int i = 0;

	list_for_each_entry_reverse(pos,&client_node_list,list, struct client_node)
	{
		i ++;
		printf("[%d] name %s\n",i, pos->name);
		printf("[%d] status %d\n",i, pos->status);
		printf("[%d] ip %s\n",i, pos->ip);
		printf("[%d] port %d\n",i, pos->port);
		printf("\r\n");
	}
}

