
/***********************************************************
文件名	:	debug.c
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.13

说明:
	必要的调试函数

***********************************************************/


#define __DEBUG_C_

#include "all.h"

#include "debug.h"

/***************************************
函数名: set_app_printf
功能: 是否开启app 级别的打印信息
		1 开启
		0 关闭
***************************************/
void set_app_printf(int mode)
{
	printf_app_flg = mode;
}


/***************************************
函数名: set_dbg_printf
功能: 是否开启dbg级别的打印信息
		1 开启
		0 关闭
***************************************/
void set_dbg_printf(int mode)
{
	printf_dbg_flg = mode;
}


/***************************************
函数名: set_sync_printf
功能: 是否开启sync级别的打印信息
		1 开启
		0 关闭
***************************************/
void set_sync_printf(int mode)
{
	printf_sync_flg = mode;
}

/***************************************
函数名: set_aes256
功能:  是否开启AES256 加密方式
		1 开启
		0 关闭
***************************************/
void set_aes256(int mode)
{
	aes256_flg = mode;
}

/***************************************
函数名: printf_client
功能:  打印客户端信息 
		1 开启
		0 关闭
***************************************/
void printf_client(struct sockaddr_in *cli_addr)
{

	dbg_printf("client info : ");
	dbg_printf("%s : %d \n", inet_ntoa(cli_addr->sin_addr), 
                               ntohs(cli_addr->sin_port));

}

