
#ifndef __CLIENT_INTERFACE_H__
#define __CLIENT_INTERFACE_H__

#ifdef __CLIENT_INTERFACE_C__
#define __CLIENT_INTERFACE_EXT__
#else
#define __CLIENT_INTERFACE_EXT__
#endif

#include "all.h"
#include "client.h"

struct client_interface_ops{
	/* 接收到服务器中转的数据 */
	void (*recv_turn)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr);	
	/* 接收到服务器中转的应答包 */
	void (*recv_turn_ack)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr); 
	/* 服务器返回节点信息 */
	void (*recv_node_info)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr);

	/* 发送失败 */	
	void (*data_send_err)(struct net_data *net_dat);

	
	/* 接收到P2P的数据 */
	void (*recv_stun)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr);	
	/* 接收到P2P的应答包 */
	void (*recv_stun_ack)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr);

	/* 监听到数据的处理 */
	void (*listen_msg)(listen_t *listen);
	void (*listen_file)(listen_t *listen);
	void (*listen_audio)(listen_t *listen);
	void (*listen_voide)(listen_t *listen);
	
};


__CLIENT_INTERFACE_EXT__ struct client_interface_ops client_inface;



/*---------------------------------------------------------
p2p_client_init
	初始化
---------------------------------------------------------*/
int p2p_client_init(void);


/*---------------------------------------------------------
client_command_init
	client 简单的命令行初始化。该函数已经在 
	p2p_client_init 中被调用
---------------------------------------------------------*/
void client_command_init(void);



/*---------------------------------------------------------
set_data_send_err
	设置发送数据失败的回调方法
---------------------------------------------------------*/
void set_data_send_err(
	void (*data_send_err)(struct net_data *net_dat)
);



/*---------------------------------------------------------
set_recv_node_info
	设置接收到客户端节点的回调方法
	从服务器上获得P2P节点信息后。会使用该
	回调方法
---------------------------------------------------------*/
void set_recv_node_info(
	void (*recv_node_info)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
);



/*---------------------------------------------------------
set_recv_stun_ack
	收到turn 的应答包的回调方法
---------------------------------------------------------*/
void set_recv_turn_ack(
	void (*recv_stun_ack)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
);



/*---------------------------------------------------------
set_recv_stun_ack
	收到turn 数据包的回调方法
---------------------------------------------------------*/
void set_recv_turn(
	void (*recv_stun)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
);




/*---------------------------------------------------------
set_recv_stun
	设置接收到stun(P2P数据包)的处理方法 
---------------------------------------------------------*/
void set_recv_stun(
	void (*recv_stun)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
);



/*---------------------------------------------------------
sendto_node_unreliable 
	不可靠的数据传输。返回值大于0  表示发送成功
	但是不保证对方一定能收到。类似UDP
	立即返回

参数
	name	目标名称
	buf		数据指针
	len		数据长度
---------------------------------------------------------*/
int sendto_node_unreliable(char *name, unsigned proto, char *buf, int len);

/*---------------------------------------------------------
sendto_node_reliable 
	相对可靠的数据传输。返回值大于0  表示发送成功
	而且对方一定是收到了
	该函数会等待对方的应答信号。因而，返回值大于0
	则意味着对方一定收到了
	如果对方无应答。休眠10秒后将会返回-1

参数
	name	目标名称
	buf		数据指针
	len		数据长度
	delay_time	休眠时间。但是实际上休眠时间是20秒不变
---------------------------------------------------------*/
int sendto_node_reliable(char *name, unsigned proto, char *buf, int len, int delay_time);



/*---------------------------------------------------------
set_recv_list

	初始化接收队列
	
参数:
	mode 	1 初始化接收队列
			0 销毁接收队列
---------------------------------------------------------*/
void set_recv_list(int mode);


/*---------------------------------------------------------
recv_node_data
	从网络数据队列中取出一个数据包
	
参数:
	buf		数据指针
	len		准备接收多少字节
	addr	对方地址信息
	delay_time	为 -1 则表示无数据一直休眠
			大于0 表示无数据则休眠一段时间。然后返回
---------------------------------------------------------*/
int recv_node_data(char *buf, int len, struct sockaddr_in *addr, int delay_time);


#endif


