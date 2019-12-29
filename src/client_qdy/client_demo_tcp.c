
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

#define SERVER_PORT		8080
#define SERVER_IP		"106.13.62.194"

void *recv_server_data(void *pdata);

int sockfd;
char *device_id ;
char *dest_id ;

//server ip port
struct sockaddr_in servaddr;


//
//

int my_send(char *buf, int ret)
{
	send(sockfd, buf, ret, 0);
}


int main(int argc,char **argv)
{
	if(argc != 3)
	{
		printf("pls input devide id  destID\r\n");
		return 0;
	}
	dest_id = (argv[2]);

	device_id = (argv[1]);
	printf("%s\r\n", device_id);
	set_name(device_id, strlen(device_id) + 1);
	set_customer("test", strlen("test") + 1);
	set_passwd(1234);
	set_net_send(my_send);


	/*	连接者的主机信息 */
	struct sockaddr_in their_addr;	

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		/*	如果socket()调用出现错误则显示错误信息并退出 */
		perror("socket");
//		exit(1);
	}

	/*	主机字节顺序 */
	their_addr.sin_family = AF_INET;
	/*	网络字节顺序，短整型 */
	their_addr.sin_port = htons(SERVER_PORT);
	their_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	/*	将结构剩下的部分清零*/
	bzero(&(their_addr.sin_zero), 8);
	if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
	{
		/*	如果connect()建立连接错误，则显示出错误信息，退出 */
		perror("connect");
		exit(1);
	}

	//printf();
	login(&servaddr);

	create_send_thread();
	
	while(1)
	{
		char recvline[1024];
		int ret;
		
		//接受数据
		ret = recv(sockfd, recvline, 1024, 0);
		if(-1 == ret)
		{
			perror("recv");
		}
		qdy_recv_data(recvline, ret);
		//return ;
	}

    close(sockfd);
    return 1;
}


void *__tcp_server_event(void *pdata)
{
	while(1)
	{
		qdy_send_data(dest_id, "ok!", strlen("ok!") + 1);
		sleep(1);
	}
}


void create_send_thread(void)
{
	pthread_t server_thread;
    pthread_attr_t server_thread_attr;

	/* 创建子进程 */
	pthread_attr_init(&server_thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&server_thread_attr, PTHREAD_CREATE_DETACHED);
//	  if (pthread_create(&recv_thread, &recv_thread_attr, recv_pthread, NULL) < 0)
	if (pthread_create(&server_thread, &server_thread_attr, __tcp_server_event, NULL) < 0)
	{
		perror("pthread_create");
	}
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
		send(sockfd, &head, sizeof(struct check_head), 0);

		int ret;
		//接受数据
		ret = recv(sockfd, recvline, 1024, 0);
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






