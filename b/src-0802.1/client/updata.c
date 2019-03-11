
#include "all.h"
#include "client.h"


#define UPDATE_PATH "agent_client_new"



int __cli_update_handle(int sock_fd)
{
	ssize_t n, ret;
	char buf[512];
	int fd;
	int w_ret;
	
	if((fd = open(UPDATE_PATH,  O_CREAT | O_RDWR, 0777)) == -1){
		perror(UPDATE_PATH);
		return -1;
	}
	
	while((n = read(sock_fd, buf, sizeof(buf))) > 0){
		ret = write(fd, buf, n);
		if(ret < 0)
			perror("write");
	}
/*
	if(n < 0 && errno == EINTR)
		goto again;
	else if(n < 0)
		printf("str_echo: read err!\r\n");
*/
#if 0	
	w_ret = system(MTD_WRITE_UPDATE_CMD);
	if((w_ret != -1) && (w_ret != 127)){
		printf("%s  ok\n", MTD_WRITE_UPDATE_CMD);
	}else{
		printf("%s  err\n", MTD_WRITE_UPDATE_CMD);
		return -1;
	}
#endif	
	close(fd);
	
	printf("str_echo out!\r\n");
	return 0;
}



int send_update_req(int sock_fd)
{
	int ret;
	struct check_head head;
	
	/* 构造包头 */
	compages_head(&head, _aff_client_updata_ack_);
	
	if((ret = send(sock_fd, &head, sizeof(struct check_head), 0)) == -1){
		perror("send UPDATE ");
	}
	return ret;
}

void *__client_update(void *pdata)
{
	int sockfd;

	/*  连接者的主机信息 */
	struct sockaddr_in their_addr;	

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		/*  如果socket()调用出现错误则显示错误信息并退出 */
		perror("socket");
//		exit(1);
	}

	/*  主机字节顺序 */
	their_addr.sin_family = AF_INET;
	/*  网络字节顺序，短整型 */
	their_addr.sin_port = htons(sys_cfg.tcp_port);
	their_addr.sin_addr.s_addr = inet_addr(sys_cfg.serverip);
	/*  将结构剩下的部分清零*/
	bzero(&(their_addr.sin_zero), 8);
	if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
	{
		/*  如果connect()建立连接错误，则显示出错误信息，退出 */
		printf("connect to %s:%d\r\n", sys_cfg.serverip, sys_cfg.tcp_port);
		perror("connect");
//		exit(1);
	}

	if(send_update_req(sockfd) == -1){
		return NULL;
	}

	if(__cli_update_handle(sockfd) == 0){
		close(sockfd);

		exit(0xa);
	}
	
//	update_flg = 1;

	close(sockfd);

	return NULL;
}



/* 升级程序 */
void client_updata_init(void)
{
	pthread_t cli_update_thread;
    pthread_attr_t cli_update_thread_attr;

	/* 创建子进程 */
	pthread_attr_init(&cli_update_thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&cli_update_thread_attr, PTHREAD_CREATE_DETACHED);
//	  if (pthread_create(&recv_thread, &recv_thread_attr, recv_pthread, NULL) < 0)
	if (pthread_create(&cli_update_thread, &cli_update_thread_attr, __client_update, NULL) < 0)
	{
		perror("pthread_create");
	}
	
}

