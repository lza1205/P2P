
#define __CLIENT_INFO_LIST_C_

#include "all.h"
#include "client.h"
#include "my_list.h"

LIST_HEAD(client_node_list);		//定义并初始化一个链表头



int add_client_info_list(struct proto_s_client_info *info)
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
		return 0;
	}
	return -1;
}



void printf_client_list(void)
{
	struct client_node *pos,*n; 
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


