
#define __SERVER_AFF_C_

#include "all.h"
#include "server.h"


/*
节点登录
*/
void aff_clientt_login(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	struct value *currentuser = NULL;
	char version[50];

	struct check_head *head = (struct check_head *)buf;
#if 0
	dbg_printf("name %s  \npasswd %d \n",
				head->name, head->passwd);
	dbg_printf("recv data : [%s]\n",buf + sizeof(struct check_head));
#endif

	/* 获取登陆者信息 */
	unsigned char name[30];
	__buf_to_str(name, head->name, USER_NAME_LEN);
	printf("name is %s\r\n", name);
	sql_table_insert("device", "name", name);
	sql_table_update_int("device", "name", name, "passwd", head->passwd);
	sql_table_update_string("device", "name", name, "ip", inet_ntoa(clientaddr->sin_addr));
	sql_table_update_int("device", "name", name, "port", ntohs(clientaddr->sin_port));
	sql_table_update_int("device", "name", name,"login_cnt", LOGIN_CNT_INIT);

	/* 向客户端应答数据 */
	char send_buf[1204];
	int send_len;
	struct proto_s_login_ack ack;
	ack.ack = 0;

//	head->affairs = _aff_server_login_ack_;
	update_head(head, _aff_server_login_ack_);

	memcpy(send_buf, head, sizeof(struct check_head));
	send_len = sizeof(struct check_head);
	memcpy(send_buf + send_len, &ack, sizeof(struct proto_s_login_ack));
	send_len += sizeof(struct proto_s_login_ack);

	p2p_sendto(sockfd, (char *)send_buf, send_len, 0, (struct sockaddr *)clientaddr,
		   sizeof(struct sockaddr));
	
}


/*
节点心跳包
*/
void aff_client_sync(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	struct check_head *head = (struct check_head *)buf;

	unsigned char name[30];
	__buf_to_str(name, head->name, USER_NAME_LEN);

	sql_table_update_int("device", "name", name,"login_cnt", LOGIN_CNT_INIT);
	sql_table_update_string("device", "name", name, "ip", inet_ntoa(clientaddr->sin_addr));
	sql_table_update_int("device", "name", name, "port", ntohs(clientaddr->sin_port));

	return ;
}


/*
获取某个节点的信息
*/
void aff_client_get_client(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{	
	int ret;
	char send_buf[1024];
	int send_len;
	char login_cnt[20] = "0";
	struct proto_c_get_lient *proto;
	struct proto_s_client_info proto_ack, proto_tell;
	struct check_head *head = (struct check_head *)buf;

	unsigned char name[30];
	__buf_to_str(name, head->name, USER_NAME_LEN);
	sql_table_update_string("device", "name", name, "ip", inet_ntoa(clientaddr->sin_addr));
	sql_table_update_int("device", "name", name, "port", ntohs(clientaddr->sin_port));

	printf("aff_client_get_client \r\n");

	proto = (struct proto_c_get_lient *)(buf + sizeof(struct check_head));

	__buf_to_str(name, proto->name, USER_NAME_LEN);
	ret = sql_table_select("device", name, "login_cnt", login_cnt);
	if((ret != 0) || (atoi(login_cnt) < 0))
	{
		printf("sql_table_select_string err %s \r\n", proto->name);
		proto_ack.status = 1;	/* 不在线 */
		memcpy(proto_ack.name,  proto->name, USER_NAME_LEN);
	}else{
		proto_ack.status = 0;	/* 在线 */
		sql_table_select("device", name, "ip", (proto_ack.ip));
		sql_table_select("device", name, "port", (proto_ack.port));
		memcpy((proto_ack.name), proto->name, USER_NAME_LEN);
	}

//	head->affairs = _aff_server_get_client_ack_;
	update_head(head, _aff_server_get_client_ack_);
	memcpy(send_buf, head, sizeof(struct check_head));
	memcpy(send_buf + sizeof(struct check_head), &proto_ack, sizeof(proto_ack));
	send_len = sizeof(proto_ack) + sizeof(struct check_head);

	
	p2p_sendto(sockfd, (char *)send_buf, send_len, 0, (struct sockaddr *)clientaddr,
		   sizeof(struct sockaddr));

	/* 如果被呼叫方在线。要通知它往对方发送一个打洞包 */
	if(proto_ack.status == 0)
	{
		struct sockaddr_in addr;

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(proto_ack.ip);
		addr.sin_port = htons(atoi(proto_ack.port));

		proto_tell.status = 0;	/* 在线 */
		strcpy(proto_tell.ip, inet_ntoa(clientaddr->sin_addr));
		sprintf(proto_tell.port, "%d", ntohs(clientaddr->sin_port));
		memcpy((proto_tell.name), head->name, USER_NAME_LEN);

		
//		head->affairs = _aff_server_p2p_start_hole_;
		update_head(head, _aff_server_p2p_start_hole_);

		memcpy(send_buf, head, sizeof(struct check_head));
		memcpy(send_buf + sizeof(struct check_head), &proto_tell, sizeof(proto_tell));
		send_len = sizeof(proto_tell) + sizeof(struct check_head);
		
		
		p2p_sendto(sockfd, (char *)send_buf, send_len, 0, (struct sockaddr *)&addr,
			   sizeof(struct sockaddr));
	}

}


/* 客户端请求服务器转发 */
void aff_client_send_data(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	int ret;
	char send_buf[len];
	int send_len;
	char ip[IP_STRING_MAX], port[20];
	char login_cnt[20] = "0";
	struct proto_c_send_data *proto;
	struct proto_s_client_info proto_ack;
	struct check_head *head = (struct check_head *)buf;

	unsigned char name[30];
	__buf_to_str(name, head->name, USER_NAME_LEN);
	sql_table_update_string("device", "name", name, "ip", inet_ntoa(clientaddr->sin_addr));
	sql_table_update_int("device", "name", name, "port", ntohs(clientaddr->sin_port));


//	printf("aff_client_send_data \r\n");

	proto = (struct proto_c_send_data *)(buf + sizeof(struct check_head));
	unsigned char dest_name[30];
	__buf_to_str(dest_name, proto->dest_name, USER_NAME_LEN);
	ret = sql_table_select("device", dest_name, "login_cnt", login_cnt);
	if((ret != 0) || (atoi(login_cnt) < 0))
	{
//		printf("sql_table_select_string err %s \r\n", dest_name);
		proto_ack.status = 1;	/* 不在线 */
		memcpy((proto_ack.name), proto->dest_name, USER_NAME_LEN);
		goto ack_loop;
	}

//	printf("______ %s send to %s data \r\n", proto->src_name, proto->dest_name);


	sql_table_select("device", dest_name, "ip", (ip));
	sql_table_select("device", dest_name, "port", (port));
	
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(atoi(port));

	/* 转发出去 */
	p2p_sendto(sockfd, (char *)buf, len, 0, (struct sockaddr *)&addr,
		   sizeof(struct sockaddr));

	return ;
ack_loop:

//	head->affairs = _aff_server_get_client_ack_;
	update_head(head, _aff_server_get_client_ack_);
	
	memcpy(send_buf, head, sizeof(struct check_head));
	memcpy(send_buf + sizeof(struct check_head), &proto_ack, sizeof(proto_ack));
	send_len = sizeof(proto_ack) + sizeof(struct check_head);

	
	p2p_sendto(sockfd, (char *)send_buf, send_len, 0, (struct sockaddr *)clientaddr,
		   sizeof(struct sockaddr));

	return ;
}



/* 客户端请求服务器转发 */
void aff_client_stun_ack(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	int ret;
	char send_buf[1024];
	int send_len;
	char login_cnt[20] = "0";
	char ip[IP_STRING_MAX], port[20];
	struct proto_c_send_data *proto;
	struct proto_s_client_info proto_ack;
	struct check_head *head = (struct check_head *)buf;
	
	proto = (struct proto_c_send_data *)(buf + sizeof(struct check_head));
	unsigned char dest_name[30];
	__buf_to_str(dest_name, proto->dest_name, USER_NAME_LEN);

	ret = sql_table_select("device", dest_name, "login_cnt", login_cnt);
	if((ret != 0) || (atoi(login_cnt) < 0))
	{
		return ;
	}

//	printf("______ %s send to %s ack \r\n", proto->src_name, proto->dest_name);

	sql_table_select("device", dest_name, "ip", (ip));
	sql_table_select("device", dest_name, "port", (port));
	
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(atoi(port));

	/* 转发出去 */
	p2p_sendto(sockfd, (char *)buf, len, 0, (struct sockaddr *)&addr,
		   sizeof(struct sockaddr));

	return ;
}

struct aff_info aff_table[] = {
	{
		_aff_client_login_,	/* 登录信号 */
		aff_clientt_login,
	},
	{
		_aff_client_sync_,	/* 同步信号 */
		aff_client_sync,
	},
	{
		_aff_client_get_client_,	/* 获取某个客户端*/
		aff_client_get_client,
	},
	{
		_aff_client_send_data_,
		aff_client_send_data,
	},
	{
		_aff_client_send_data_ack_,
		aff_client_stun_ack,
	},
	{
		0,
		NULL,
	},
};

