
#define __SERVER_AFF_C_

#include "all.h"
#include "server.h"
#include "server_client_node_list.h"


/*
节点登录
*/

extern int login_num;


void tcp_aff_clientt_login(void *pfd, char *buf, int len)
{
	int ret;
	struct value *currentuser = NULL;
	struct proto_s_login_ack ack;
	char version[50];
	char passwd[20];
	char send_buf[1204];
	int send_len;

	struct check_head *head = (struct check_head *)buf;
#if 0
	dbg_printf("name %s  \npasswd %d \n",
				head->name, head->passwd);
	dbg_printf("recv data : [%s]\n",buf + sizeof(struct check_head));
#endif

	/* 获取登陆者信息 */
	unsigned char name[30];
	memset(name, 0, 30);
	__buf_to_str(name, head->name, USER_NAME_LEN);
	printf("name is %s\r\n", name);
	dump_data(name,sizeof(name));

	#if P2P_SERVER_SQL_AUTO_INSERT
	sql_table_insert("device", "name", name);
	#endif

	ret = service_sql_select(name, passwd);
	printf("passwd is %s \r\n", passwd);
	/*
	if(atoi(passwd) != head->passwd)
	{
		ack.ack = -1;
		goto out;
	}
	*/

	if(ret == 0)
	{
		sql_table_update_string("device", "name", name, "is_online", "YES");
		printf("%s %d %p \r\n", __FILE__, __LINE__, pfd);
		add_server_client_node(pfd, name);
		ack.ack = 0;
	}else{
		ack.ack = -1;
	}

out:
	/* 向客户端应答数据 */
	

//	head->affairs = _aff_server_login_ack_;
	update_head(head, _aff_server_login_ack_);

	memcpy(send_buf, head, sizeof(struct check_head));
	send_len = sizeof(struct check_head);
	memcpy(send_buf + send_len, &ack, sizeof(struct proto_s_login_ack));
	send_len += sizeof(struct proto_s_login_ack);

	send_cb(pfd, (char *)send_buf, send_len);

	login_num ++;
}



/* 客户端请求服务器转发 */
void tcp_aff_client_send_data(void *pfd, char *buf, int len)
{
	int ret;
	void *fpfd;
	char send_buf[len];
	int send_len;
	char ip[IP_STRING_MAX], port[20];
	char login_cnt[20] = "0";
	struct proto_c_send_data *proto;
	struct proto_s_client_info proto_ack;
	struct check_head *head = (struct check_head *)buf;

	proto = (struct proto_c_send_data *)(buf + sizeof(struct check_head));
	unsigned char dest_name[30];
	memset(dest_name, 0, 30);
	__buf_to_str(dest_name, proto->dest_name, USER_NAME_LEN);
	printf("name is %s\r\n", dest_name);
	dump_data(dest_name,sizeof(dest_name));
	
	/* 找到对方设备*/
	fpfd = find_server_client_socket(dest_name);
	printf("%s %d %p \r\n", __FILE__, __LINE__, fpfd);
	if(fpfd == NULL)
	{
		goto ack_loop;
	}

	/* 转发出去 */
	send_cb(fpfd, (char *)buf, len);

	return ;
	
ack_loop:

//	head->affairs = _aff_server_get_client_ack_;
	update_head(head, _aff_server_get_client_ack_);
	
	send_cb(pfd, (char *)send_buf, send_len);

	return ;
}



/* 客户端请求服务器转发 */
void tcp_aff_client_stun_ack(void *pfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	int ret;
	void *fpfd;
	char send_buf[1024];
	int send_len;
	char login_cnt[20] = "0";
	char ip[IP_STRING_MAX], port[20];
	struct proto_c_send_data *proto;
	struct proto_s_client_info proto_ack;
	struct check_head *head = (struct check_head *)buf;
	
	proto = (struct proto_c_send_data *)(buf + sizeof(struct check_head));
	unsigned char dest_name[30];
	printf("name is %s\r\n", dest_name);
	__buf_to_str(dest_name, proto->dest_name, USER_NAME_LEN);
	printf("name is %s\r\n", dest_name);
	dump_data(dest_name,sizeof(dest_name));

	/* 找到对方设备*/
	fpfd = find_server_client_socket(dest_name);
	printf("%s %d %p \r\n", __FILE__, __LINE__, fpfd);
	if(fpfd == 0)
	{
		goto ack_loop;
	}

	/* 转发出去 */
	send_cb(fpfd, (char *)buf, len);

	return ;

ack_loop:

//	head->affairs = _aff_server_get_client_ack_;
	update_head(head, _aff_server_get_client_ack_);
	
	send_cb(pfd, (char *)send_buf, send_len);

}

struct tcp_aff_info tcp_aff_table[] = {
	{
		_aff_client_login_,	/* 登录信号 */
		tcp_aff_clientt_login,
	},
	{
		_aff_client_send_data_,
		tcp_aff_client_send_data,
	},
	{
		_aff_client_send_data_ack_,
		tcp_aff_client_stun_ack,
	},
	{
		0,
		NULL,
	},
};


void server_tcp_recv(void *iCliFd, char *buf, int iLen)
{
	struct check_head *head;
	int i;
	head = (struct check_head *)buf;

	dump_data(buf,iLen);
	/* 检查长度 */
	if(iLen < sizeof(struct check_head))
		return ;

	/* 对数据包头部进行校验 */
	if(check_head_crc(head) != 0)
		return; 	

	printf("affairs is %d\r\n", head->affairs);

	if(is_max_login())//超过最大登录数量了
	{
		printf("max login !! pls call lianzhian(13510979604) \r\n");
	}else{
		for(i = 0; tcp_aff_table[i].aff_fun != NULL; i++)
		{
			if(tcp_aff_table[i].aff == head->affairs)
			{
				printf("%s %d %p \r\n", __FILE__, __LINE__, iCliFd);
				tcp_aff_table[i].aff_fun(iCliFd, buf, iLen);
			}
		}
	}
}





void update_client_status(void *pfd)
{
	char name[USER_NAME_LEN];
	int ret;

	memset(name, 0, sizeof(name));
	ret = find_server_client_name(pfd, name);
	
	if(ret == 0)
	{
		printf("update sql, %s is off line\r\n", name);
		sql_table_update_string("device", "name", name, "is_online", "NO");
	}

	printf("del client\r\n");
	del_server_client_node(pfd);
}

