#define __CLIENT_AFF_C_

#include "all.h"
#include "client.h"
#include "my_list.h"

extern struct json_object *sys_info_json;


/* 服务器返回客户端节点信息 */
void aff_server_get_client_ack(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	struct proto_s_client_info *recv_proto;

	del_net_data_list(buf, len);

	recv_proto = (struct proto_s_client_info *)(buf + sizeof(struct check_head));

	/* 在线才添加到链表中 */
	if(recv_proto->status == 0)
	{
		/* 添加到节点链表 */
		add_client_node(recv_proto);
		/* 添加到P2P链表 */
		add_p2p_test_node(recv_proto);
	}

	recv_ack_wakeup();		//唤醒发送队列
	client_inface.recv_node_info(sockfd, buf, len, clientaddr);
}


/* 收到服务器转发过来的数据 */
void aff_client_recv_data(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	/* 发送应答数据 */	
	struct proto_c_send_data proto, *recv_proto;
	struct check_head *head, ack_head;

	int ret;
	char sendbuf[1204];
	int send_len;

	struct sockaddr_in serveraddr;

	serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(sys_cfg.serverip);
    serveraddr.sin_port = htons(sys_cfg.ser_port);

	/* 不要重新构造包头 */
	head = (struct check_head *)buf;
	memcpy(&ack_head, head, sizeof(struct check_head));	//记录这个包头。等会要做验证
	ack_head.affairs = _aff_client_send_data_ack_;

	recv_proto = (struct proto_c_send_data*)(buf + sizeof(struct check_head));
	
	__strcpy(proto.dest_name, recv_proto->src_name, USER_NAME_LEN);
	__strcpy(proto.src_name, sys_cfg.myname, USER_NAME_LEN);

	memcpy(sendbuf, &ack_head, sizeof(struct check_head));
	memcpy(sendbuf + sizeof(struct check_head), &proto, sizeof(proto));
	send_len = sizeof(struct check_head) + sizeof(proto);

	ret = p2p_sendto(sys_cfg.sockfd, (char *)sendbuf, send_len, 0, (struct sockaddr *)&serveraddr,
		   sizeof(serveraddr));

	/* 检查下包头。之前是否接收过 */
	if(check_recv_head(head) == 0)
	{
		client_inface.recv_stun(sockfd, buf, len, clientaddr);

		/* 把数据添加到接收队列中 */
		if(recv_list_flg == 1)
		{
			add_recv_data_list(buf, len, clientaddr);
		}
	}
}



/* 服务器转发过来的应答包 */
void aff_client_send_data_ack(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{	
	del_net_data_list(buf, len);

	/* 应答.唤醒等待线程 */
	recv_ack_wakeup((struct check_head *)buf);
	
	client_inface.recv_stun_ack(sockfd, buf, len, clientaddr);
}


/* 请求发送打洞包 */
void aff_server_p2p_hole(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	char send_buf[512];
	int send_len;
	struct check_head head;
	struct proto_s_client_info *info = (struct proto_s_client_info *)(buf + sizeof(struct check_head));

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(info->ip);
	addr.sin_port = htons(atoi(info->port));

	compages_head(&(head), _aff_client_hole_);
	memcpy(send_buf, &head, sizeof(struct check_head));
	send_len = sizeof(struct check_head);

	printf("send hole \r\n");
	
	p2p_sendto(sockfd, (char *)send_buf, send_len, 0, (struct sockaddr *)&addr,
		   sizeof(struct sockaddr));
}





void aff_client_hole(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	printf("recv hole \r\n");
}


void aff_client_p2p_test(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	char send_buf[512];
	int send_len;
//	printf("recv p2p test !\r\n");

	struct check_head head;
	struct sockaddr_in addr;

	compages_head(&(head), _aff_client_p2p_test_ack_);
	memcpy(send_buf, &head, sizeof(struct check_head));
	send_len = sizeof(struct check_head);

//	printf("send p2p test ack \r\n");
	
	p2p_sendto(sockfd, (char *)send_buf, send_len, 0, (struct sockaddr *)clientaddr,
		   sizeof(struct sockaddr));
}

void aff_client_p2p_test_ack(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
//	printf("recv p2p test ack !\r\n");
	struct check_head *head;

	head = (struct check_head *)(buf);
	
	/* 添加到P2P链表 */
	add_p2p_sync_node(head);
}


void aff_client_p2p_sync(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	char send_buf[512];
	int send_len;
//	printf("recv p2p sync !\r\n");

	struct check_head head;
	struct sockaddr_in addr;

	compages_head(&(head), _aff_client_p2p_sync_ack_);
	memcpy(send_buf, &head, sizeof(struct check_head));
	send_len = sizeof(struct check_head);

//	printf("send p2p sync ack \r\n");
	
	p2p_sendto(sockfd, (char *)send_buf, send_len, 0, (struct sockaddr *)clientaddr,
		   sizeof(struct sockaddr));
}

void aff_client_p2p_sync_ack(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
//	printf("recv p2p sync ack !\r\n");
	struct check_head *head;

	head = (struct check_head *)(buf);
	
	/*更新P2P 链表 */
	update_p2p_sync_node(head);
}



/* 收到服务器转发过来的数据 */
void aff_client_p2p_data(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	/* 发送应答数据 */	
	struct proto_c_send_data proto, *recv_proto;
	struct check_head *head, ack_head;

	int ret;
	char sendbuf[512];
	int send_len;

	/* 不要重新构造包头 */
	head = (struct check_head *)buf;
	memcpy(&ack_head, head, sizeof(struct check_head));
	ack_head.affairs = _aff_client_p2p_data_ack_;

	recv_proto = (struct proto_c_send_data*)(buf + sizeof(struct check_head));
	
	__strcpy(proto.dest_name, recv_proto->src_name, USER_NAME_LEN);
	__strcpy(proto.src_name, sys_cfg.myname, USER_NAME_LEN);

	memcpy(sendbuf, &ack_head, sizeof(struct check_head));
	memcpy(sendbuf + sizeof(struct check_head), &proto, sizeof(proto));
	send_len = sizeof(struct check_head) + sizeof(proto);

	ret = p2p_sendto(sys_cfg.sockfd, (char *)sendbuf, send_len, 0, (struct sockaddr *)clientaddr,
		   sizeof(struct sockaddr));

	/* 检查下包头。之前是否接收过 */
	if(check_recv_head(head) == 0)
	{
		client_inface.recv_turn(sockfd, buf, len, clientaddr);
		/* 把数据添加到接收队列中 */
		if(recv_list_flg == 1)
		{
			add_recv_data_list(buf, len, clientaddr);
		}
	}
}

/* 服务器转发过来的应答包 */
void aff_client_p2p_data_ack(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{	
	del_net_data_list(buf, len);

	/* 应答.唤醒等待线程 */
	recv_ack_wakeup((struct check_head *)buf);

	client_inface.recv_turn_ack(sockfd, buf, len, clientaddr);
}


struct client_aff_info client_aff_table[] = {

	{
		_aff_server_get_client_ack_,
		aff_server_get_client_ack
	},
	{
		_aff_client_send_data_,
		aff_client_recv_data,
	},
	{
		_aff_client_send_data_ack_,
		aff_client_send_data_ack,
	},

	{
		_aff_server_p2p_start_hole_,
		aff_server_p2p_hole
	},
	{
		_aff_client_hole_,
		aff_client_hole,
	},
	{
		_aff_client_p2p_test_,
		aff_client_p2p_test,
	},
	{
		_aff_client_p2p_test_ack_,
		aff_client_p2p_test_ack,
	},
	{
		_aff_client_p2p_sync_,
		aff_client_p2p_sync,
	},
	{
		_aff_client_p2p_sync_ack_,
		aff_client_p2p_sync_ack,
	},
	{
		_aff_client_p2p_data_,
		aff_client_p2p_data,
	},
	{
		_aff_client_p2p_data_ack_,
		aff_client_p2p_data_ack,
	},
	{
		0,
		NULL,
	},
};


/* --------------------------------------------------------------------- */
LIST_HEAD(recv_head_list);		//定义并初始化一个链表头
static pthread_mutex_t recv_head_list_lock = PTHREAD_MUTEX_INITIALIZER;
volatile int recv_head__node_num = 0;



/* 对接收到的数据做个处理。过滤掉相同的数据包 */
/*
返回0 表示之前没有接收过这个数据
*/
int check_recv_head(struct check_head *head)
{
#if 1
	struct recv_head_node *pos,*n;
	
	pthread_mutex_lock(&recv_head_list_lock);
	
	list_for_each_entry_safe_reverse(pos,n, &recv_head_list, list, struct recv_head_node)
	{
		if(recv_head__node_num > 50)
		{
			list_del(&(pos->list));
			free(pos);
			recv_head__node_num --;
		}
		if(memcmp(&(pos->head), head, sizeof(struct check_head)) == 0)
		{
			/* 相等。说明之前已经接收过这个数据了。应该丢弃 */
			pthread_mutex_unlock(&recv_head_list_lock);
			return -1;
		}
	}

	struct recv_head_node *r_head;
	r_head = malloc(sizeof(struct recv_head_node));
	if(r_head != NULL)
	{
		memcpy(&(r_head->head), head, sizeof(struct check_head));
		/* 加到链表中 */
		list_add(&(r_head->list), &recv_head_list);
		recv_head__node_num ++;

		pthread_mutex_unlock(&recv_head_list_lock);
		return 0;
	}

	pthread_mutex_unlock(&recv_head_list_lock);
	return -1;
	
#endif
}






/***************************************
函数名: GetIP
功能: 获取局域网IP
***************************************/

void GetIP(void)
{
  int socket_fd;
//  struct sockaddr_in *sin;
  struct ifreq *ifr;
  struct ifconf conf;
  char buff[512];
  int num;
  int i;
  socket_fd = socket(AF_INET,SOCK_DGRAM,0);
  conf.ifc_len = 512;
  conf.ifc_buf = buff;
  ioctl(socket_fd,SIOCGIFCONF,&conf);
  num = conf.ifc_len / sizeof(struct ifreq);
  ifr = conf.ifc_req;
//  printf("num=%d\n",num);
  for(i=0;i<num;i++)
  {
    struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr->ifr_addr);

    ioctl(socket_fd,SIOCGIFFLAGS,ifr);
    if(((ifr->ifr_flags & IFF_LOOPBACK) == 0) && (ifr->ifr_flags & IFF_UP))
    {
    	if(strcmp(ifr->ifr_name, "apcli0") == 0){
			__strcpy(sys_cfg.lan_ip, inet_ntoa(sin->sin_addr), IP_STRING_MAX);
//      		printf("%s(%s)\n",ifr->ifr_name,inet_ntoa(sin->sin_addr));
			break;
		}
		__strcpy(sys_cfg.lan_ip, inet_ntoa(sin->sin_addr), IP_STRING_MAX);
//      printf("%s(%s)\n",ifr->ifr_name,inet_ntoa(sin->sin_addr));
    }
    ifr++;
  }
}

int init_client_socket(unsigned short port)
{
    struct sockaddr_in clientaddr;

    if ((sys_cfg.sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    bzero(&clientaddr, sizeof(clientaddr));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientaddr.sin_port = htons(port);		//CLIENT_PORT
    
	/* 将结构体其余的都清零 */
	bzero(&(clientaddr.sin_zero), 8);

    if (bind(sys_cfg.sockfd, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

	sys_cfg.lan_port = port;

	GetIP();
	
	printf("my lan_ip: %s\nport: %d\n", sys_cfg.lan_ip, sys_cfg.lan_port);

	return sys_cfg.sockfd;	
}



int get_mac(char *eth, char *mac)
{
    struct ifreq ifreq;
    int sock = 0;
//    char mac[32] = "";

    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("error sock");
        return 2;
    }

    strcpy(ifreq.ifr_name, eth);
    if(ioctl(sock,SIOCGIFHWADDR,&ifreq) < 0)
    {
        perror("error ioctl");
        return 3;
    }

    int i = 0;
    for(i = 0; i < 6; i++){
        sprintf(mac+3*i, "%02X:", (unsigned char)ifreq.ifr_hwaddr.sa_data[i]);
    }
    mac[strlen(mac) - 1] = 0;
    printf("MAC: %s\n", mac);

	return 0;
}

