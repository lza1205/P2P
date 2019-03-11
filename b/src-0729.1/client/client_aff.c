#define __CLIENT_AFF_C_

#include "all.h"
#include "client.h"


extern struct json_object *sys_info_json;


void aff_server_get_client_ack(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	printf("aff_server_get_client_ack \r\n");
	del_net_data_list(buf, len);

	add_client_info_list(buf + sizeof(struct check_head));
}



void aff_client_recv_data(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	printf("aff_client_recv_data \r\n");
	printf("recv data is [%s] \r\n", buf + sizeof(struct check_head) 
										+ sizeof(struct proto_c_send_data));
	/* 发送应答数据 */
	
	struct proto_c_send_data proto, *recv_proto;
	struct check_head *head;

	int ret;
	char sendbuf[1204];
	int send_len;

	struct sockaddr_in serveraddr;

	serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(sys_cfg.serverip);
    serveraddr.sin_port = htons(sys_cfg.ser_port);

	/* 不要重新构造包头 */
	head = buf;
	head->affairs = _aff_client_send_data_ack_;

	recv_proto = buf + sizeof(struct check_head);
	
	__strcpy(&(proto.dest_name), recv_proto->src_name, USER_NAME_LEN);
	__strcpy(&(proto.src_name), sys_cfg.myname, USER_NAME_LEN);

	memcpy(sendbuf, head, sizeof(struct check_head));
	memcpy(sendbuf + sizeof(head), &proto, sizeof(proto));
	send_len = sizeof(head) + sizeof(proto);

	ret = p2p_sendto(sys_cfg.sockfd, (char *)sendbuf, send_len, 0, (struct sockaddr *)&serveraddr,
		   sizeof(serveraddr));
}


void aff_client_send_data_ack(int sockfd, char *buf, int len, struct sockaddr_in *clientaddr)
{
	printf("aff_client_send_data_ack \r\n");
	
	del_net_data_list(buf, len);
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
		0,
		NULL,
	},
};




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

