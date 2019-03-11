
/***********************************************************
文件名	:	my_socket.c
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.13

说明:
	对TCP/IP 协议栈近一步封装

***********************************************************/


#include "all.h"


#include "aes256_data.h"

#include <openssl/aes.h>  
#include <openssl/rand.h>  




/***************************************
函数名: aes256_recvfrom
功能: 从TCP/IP 协议栈里获取 UDP 数据，同时
		对数据进行AES256 解密
***************************************/
int aes256_recvfrom(int fd, void *buf, int len, int flags, struct sockaddr *addr, socklen_t *addr_len)
{
	int ret;

	int ss,len_s;
	char buff_in[UDP_PACK_MAX_SIZE];

	len_s = len;
	if((ss = (len % 16)) != 0){
		len_s = len + (16 - ss);
	}

	if(len_s > UDP_PACK_MAX_SIZE){
		printf("recvfrom len_s > UDP_PACK_MAX_SIZE %d", len_s);
		return -1;
	}

	ret = recvfrom(fd, buf, len_s, flags, addr, addr_len);
	
	if((ss = (ret % 16)) != 0){
		printf("ret no 16x != 0\n");
		return -1;
	}
	
	memcpy(buff_in, buf, ret);
	aes256_data(buff_in, buf, ret, 
				aes256_key, sizeof(aes256_key), AES_DECRYPT);

	return ret;
}


/***************************************
函数名: aes256_sendto
功能: 往TCP/IP 协议栈里发送 UDP 数据，同时
		对数据进行AES256 加密，会自动补齐
		为 16 的倍数
***************************************/
int aes256_sendto(int fd, void *buf, int len,
		       int flags, struct sockaddr *addr,
		       socklen_t addr_len)
{
	int ret;
	int ss, len_s;
	char buff_in[UDP_PACK_MAX_SIZE];
	char buff_out[UDP_PACK_MAX_SIZE];

	len_s = len;

	memcpy(buff_in, buf, len);
	
	if((ss = (len % 16)) != 0){
		memset(&buff_in[len], 0, 16);
		len_s = len + (16 - ss);
	}
	
	if(len_s > UDP_PACK_MAX_SIZE){
		printf("recvfrom len_s > UDP_PACK_MAX_SIZE %d", len_s);
		return -1;
	}
	
	aes256_data(buff_in, buff_out, len_s, 
				aes256_key, sizeof(aes256_key), AES_ENCRYPT);	
	ret = sendto(fd, buff_out, len_s, flags, addr, addr_len);
//	printf("sendto len %d ret %d\n", len_s, ret);

	return ret;
}


/***************************************
函数名: p2p_recvfrom
功能: 往TCP/IP 协议栈里发送 P2P 数据，
		同时根据设置决定是否需要解密
***************************************/
int p2p_recvfrom(int fd, void *buf, int len, int flags, struct sockaddr *addr, socklen_t *addr_len)
{
	int ret;
	if(aes256_flg)
	{
		ret = aes256_recvfrom(fd, buf, len, flags, addr, addr_len);
	}
	else
	{
		ret = recvfrom(fd, buf, len, flags, addr, addr_len);
	}
	if(ret == -1){
		perror("sendto ");
	}
	return ret;
}

/***************************************
函数名: p2p_sendto
功能: 往TCP/IP 协议栈里发送 P2P 数据，
		同时根据设置决定是否需要加密
***************************************/
int p2p_sendto(int fd, void *buf, int len,
		       int flags, struct sockaddr *addr,
		       socklen_t addr_len)
{
	int ret;
	
	if(aes256_flg)
	{
		ret = aes256_sendto(fd, buf, len, flags, addr, addr_len);
	}
	else
	{
		ret = sendto(fd, buf, len, flags, addr, addr_len);
	}
	if(ret == -1){
		perror("sendto ");
	}
	return ret;
}


/***************************************
函数名: p2p_sendto
功能: 构建服务器socket 套签字
***************************************/
#if 0
void create_server_socket(struct sockaddr_in *servaddr)
{
	memset(servaddr, 0, sizeof(struct sockaddr_in));
	
	servaddr->sin_family = AF_INET;
    servaddr->sin_addr.s_addr = inet_addr(sys_cfg.serverip);
    servaddr->sin_port = htons(sys_cfg.ser_port);

}
#endif


