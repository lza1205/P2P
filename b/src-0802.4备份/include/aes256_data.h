
/***********************************************************
文件名	:	aes256_data.c
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.15

说明:	
	本文件作为AES256 加密。key 为 aes256_key
	需调用 aes256_key_init 进行初始化并测试
	aes256_data_enc		加密
	aes256_data_dec		解密

***********************************************************/

#ifndef _AES256_DATA_H_
#define _AES256_DATA_H_

#ifdef _AES256_DATA_C_
#define _AES256_DATA_EXT_
#else
#define _AES256_DATA_EXT_ extern
#endif


/* 初始化aes256 key */
void aes256_key_init(void);

/* 加密数据。数据长度必须是16 的倍数 */
void aes256_data(char *in, char *out, int len, char *rkey, 
	int rkey_len, int enc);

/* 密匙 */
_AES256_DATA_EXT_ char aes256_key[32];






#endif



