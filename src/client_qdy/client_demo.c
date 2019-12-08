
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <Windows.h>
#include <pthread.h>
#include <malloc.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <netdb.h>


#include <sys/time.h>

#include <signal.h>


#include "config.h"
#include "agent_proto.h"
#include "qdy_interface.h"

#define SERVER_PORT		8000
#define SERVER_IP		"106.13.62.194"

void *recv_server_data(void *pdata);

int sockfd;
char *device_id ;
//server ip port
struct sockaddr_in servaddr;


//
//

int my_send(char *buf, int ret)
{
	sendto(sockfd, buf, ret, 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
}


int main(int argc,char **argv)
{
	if(argc != 2)
	{
		printf("pls input devide id\r\n");
		return 0;
	}

	device_id = (argv[1]);
	printf("%s\r\n", device_id);
	set_name(device_id, strlen(device_id) + 1);
	
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	set_net_send(my_send);

	login(&servaddr);

	while(1)
	{
		char recvline[1024];
		struct sockaddr_in addrRecv;
		int sizeRecvAddr = sizeof(struct sockaddr_in);

		int ret;
		//接受数据
		ret = recvfrom(sockfd, recvline, 1024, 0, (struct sockaddr*)&addrRecv,(socklen_t*)&sizeRecvAddr);
		qdy_recv_data(recvline, ret);

		sleep(2);
	}

    close(sockfd);
    return 1;
}



void login(struct sockaddr_in *addr)
{
	char recvline[1024];
	struct sockaddr_in addrRecv;
	int sizeRecvAddr = sizeof(struct sockaddr_in);
	struct proto_s_login_ack *ack;
	struct check_head head;
	__compages_head(&head, _aff_client_login_, device_id, 1234, "test");
	//暂时不做 crc 随机数 时间等

	//等待服务器应答
	while(1)
	{
		sendto(sockfd, &head, sizeof(struct check_head), 0, (struct sockaddr *)addr, sizeof(struct sockaddr_in));

		int ret;
		//接受数据
		ret = recvfrom(sockfd, recvline, 1024, 0, (struct sockaddr*)&addrRecv,(socklen_t*)&sizeRecvAddr);
		if(ret < 0)
		{
			sleep(5);
			continue;
		}

		struct check_head *recv_head;
		recv_head = (struct check_head *)recvline;

		if((recv_head->affairs == _aff_server_login_ack_))
				   goto login_ack;

		sleep(5);
	}
	
login_ack:
	
	ack = (struct proto_s_login_ack *)(recvline + sizeof(struct check_head));

	if(ack->ack == 0)
	{
		printf("connet is ok!\r\n");
	}else{
		printf("connet is err!\r\n");
	}
	return ack->ack;		//返回登陆情况

}






