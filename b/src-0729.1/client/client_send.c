

#include "all.h"
#include "client.h"

void send_to_server(char *sendbuf, int send_len)
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
void get_client_info(char *name)
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
	__strcpy(&(proto.name), name, USER_NAME_LEN);

	memcpy(sendbuf, &head, sizeof(head));
	memcpy(sendbuf + sizeof(head), &proto, sizeof(proto));
	send_len = sizeof(head) + sizeof(proto);

	ret = p2p_sendto(sys_cfg.sockfd, (char *)sendbuf, send_len, 0, (struct sockaddr *)&serveraddr,
		   sizeof(serveraddr));

	/* 把数据添加到队列中去 */
	add_net_data_list(sendbuf, send_len, &serveraddr);

	return ret;
}


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

	__strcpy(&(proto.dest_name), name, USER_NAME_LEN);
	__strcpy(&(proto.src_name), sys_cfg.myname, USER_NAME_LEN);

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


