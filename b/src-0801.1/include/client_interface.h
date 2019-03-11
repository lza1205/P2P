
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


int sendto_node(char *name, char *buf, int len);

#endif


