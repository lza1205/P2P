
/***********************************************************
文件名	:	debug.h
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.13

说明:
	必要的调试函数

***********************************************************/

#ifndef __DEBUG_H_
#define __DEBUG_H_

#ifdef __DEBUG_C_
#define __DEBUG_EXT_
#else
#define __DEBUG_EXT_ extern
#endif

#include "all.h"

__DEBUG_EXT_	int printf_app_flg;
__DEBUG_EXT_	int printf_dbg_flg;
__DEBUG_EXT_	int printf_sync_flg;

/* 是否开启AES256 加密 */
__DEBUG_EXT_	int aes256_flg;


void set_app_printf(int mode);
void set_dbg_printf(int mode);
void set_sync_printf(int mode);
void set_aes256(int mode);
void printf_client(struct sockaddr_in *cli_addr);


#endif


