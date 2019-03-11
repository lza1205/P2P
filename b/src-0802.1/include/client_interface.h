
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
	void (*recv_stun)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr);	
	/* 接收到服务器中转的应答包 */
	void (*recv_stun_ack)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr); 
	/* 服务器返回节点信息 */
	void (*recv_node_info)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr);

	/* 发送失败 */	
	void (*data_send_err)(struct net_data *net_dat);

	
	/* 接收到P2P的数据 */
	void (*recv_turn)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr);	
	/* 接收到P2P的应答包 */
	void (*recv_turn_ack)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr); 
};


__CLIENT_INTERFACE_EXT__ struct client_interface_ops client_inface;


int p2p_client_init(void);

void client_command_init(void);

void set_data_send_err(
	void (*data_send_err)(struct net_data *net_dat)
);
void set_recv_node_info(
	void (*recv_node_info)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
);
void set_recv_stun_ack(
	void (*recv_stun_ack)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
);
void set_recv_stun(
	void (*recv_stun)(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
);


/*
sendto_node_unreliable 
不可靠的数据传输。返回值大于0  表示发送成功
但是不保证对方一定能收到。类似UDP
立即返回
*/
int sendto_node_unreliable(char *name, char *buf, int len);

/*
sendto_node_reliable 
相对可靠的数据传输。返回值大于0  表示发送成功
而且对方一定是收到了
该函数会等待对方的应答信号。因而，返回值大于0
则意味着对方一定收到了
如果对方无应答。休眠10秒后将会返回-1
*/
int sendto_node_reliable(char *name, char *buf, int len, int delay_time);

/* 
recv_node_data
从网络数据队列中取出一个数据包
delay_time	为 -1 则表示无数据一直休眠
			大于0 表示无数据则休眠一段时间。然后返回
*/
int recv_node_data(char *buf, int len, struct sockaddr_in *addr, int delay_time);


#endif


