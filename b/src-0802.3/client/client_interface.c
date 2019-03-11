
#define __CLIENT_INTERFACE_C__


#include "all.h"
#include "client.h"



void default_recv_stun(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	struct proto_c_send_data *recv_proto;
	recv_proto = (struct proto_c_send_data *)(buf + sizeof(struct check_head));
	
	printf("recv data : \r\n");
	printf("%s send to %s data is [%s] \r\n", recv_proto->src_name, recv_proto->dest_name,
							buf + sizeof(struct check_head) + sizeof(struct proto_c_send_data));
}

void default_recv_node_info(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	//printf("get client's info is ok \r\n");
	struct proto_s_client_info *recv_proto;
	recv_proto = (struct proto_s_client_info *)(buf + sizeof(struct check_head));
	
	if(recv_proto->status != 0)
	{
		printf("%s is offline \r\n", recv_proto->name);
	}else{
		printf("%s is online \r\n", recv_proto->name);
	}
}

void default_recv_stun_ack(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	printf("recv ack, send is ok \r\n");
	
	struct proto_c_send_data *recv_proto;
	recv_proto = (struct proto_c_send_data *)(buf + sizeof(struct check_head));
	
	printf("%s send to %s ack \r\n", recv_proto->src_name, recv_proto->dest_name);
}


void default_data_send_err(struct net_data *net_dat)
{
	printf("send failed! \r\n");
}

void default_recv_turn(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	char *recv_data;
	recv_data = (char *)(buf + sizeof(struct check_head));
	
	printf("recv turn data : [%s] \r\n", recv_data);
}

void default_recv_turn_ack(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	printf("recv turn ack, send is ok \r\n");
	
	struct proto_c_send_data *recv_proto;
	recv_proto = (struct proto_c_send_data *)(buf + sizeof(struct check_head));
	
}


struct client_interface_ops client_inface = {
	.recv_stun = default_recv_stun,
	.recv_node_info = default_recv_node_info,
	.recv_stun_ack = default_recv_stun_ack,
	.data_send_err = default_data_send_err,

	.recv_turn = default_recv_turn,	
	.recv_turn_ack = default_recv_turn_ack,
};





/* 设置自己的处理方法 */
void set_recv_turn(
	void (*recv_turn)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
)
{
	client_inface.recv_turn = recv_turn;
}


/* 设置自己收到中转ack 的方法 */
void set_recv_turn_ack(
	void (*recv_turn_ack)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
)
{
	client_inface.recv_turn_ack = recv_turn_ack;
}

/* 设置自己获取节点信息的处理方法 */
void set_recv_node_info(
	void (*recv_node_info)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
)
{
	client_inface.recv_node_info = recv_node_info;
}

/* 设置自己的发送失败处理函数 */
void set_data_send_err(
	void (*data_send_err)(struct net_data *net_dat)
)
{
	client_inface.data_send_err = data_send_err;
}


/* 设置接收到stun(P2P数据包)的处理方法 */
void set_recv_stun(
	void (*recv_stun)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
)
{
	client_inface.recv_stun = recv_stun;
}


/* 设置接收到stun ack(P2P 应答包)的处理方法 */
void set_recv_stun_ack(
	void (*recv_stun_ack)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
)
{
	client_inface.recv_stun_ack = recv_stun_ack;
}



/* 设置是否采用接收队列 */
void set_recv_list(int mode)
{
	recv_list_flg = mode;
}


/* 
全局初始化 
*/
int p2p_client_init(void)
{
	/* 开启打印信息 */
	set_app_printf(1);
	set_dbg_printf(1);
	set_sync_printf(1);

	/* AES256 加密初始化 */
//	aes256_key_init();
	set_aes256(1);			//开启 AES 256 加密方式
	
	/* 检查下配置文件 */
	get_config_for_json();

	/* 初始化socket */
    init_client_socket((unsigned short)sys_cfg.my_port);

	memcpy(sys_cfg.now_version, CLIENT_VERSION, sizeof(sys_cfg.now_version));
	printf("\nclient version :         %s\n\n", sys_cfg.now_version);
	
	/* 初始化网络数据队列 */
	init_net_data_list();

	/* 启动udp 通信线程 */
	init_network_pthread();

	/* P2P 线程初始化 */
	p2p_thread_init();

	/* 初始化命令行 */
	client_command_init();
}


/*
发送函数
只管发送。不管对方是否能收到
*/
int sendto_node_unreliable(char *name, char *buf, int len)
{
	int ret;

	ret = p2p_send_data(name, buf, len);
	if(ret <= 0)
	{
		/* 发送失败的。要将改用户从P2P队列删除 */
		del_p2p_sync_node(name);
		
		ret = send_client_data(name, buf, len);
		get_client_info(name);
	}
	return ret;
}


/*
可靠的发送。会进入阻塞
*/
int __sendto_node_reliable(char *name, char *buf, int len, int delay_time)
{
	int ret;
	int r;

	ret = p2p_send_data_2(name, buf, len, &r, delay_time);

	if(ret <= 0)
	{
		/* 发送失败的。要将改用户从P2P队列删除 */
		del_p2p_sync_node(name);
		
		ret = send_client_data_2(name, buf, len, delay_time);
		get_client_info(name);
	}
	return ret;
}


/*
可靠的发送。会进入阻塞
*/
int sendto_node_reliable(char *name, char *buf, int len, int delay_time)
{
	__sendto_node_reliable(name, buf, len, 20);
}

