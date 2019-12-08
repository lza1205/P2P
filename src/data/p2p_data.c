/***********************************************************
文件名	:	p2p_data.h
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.13

说明:
		构建网络数据

***********************************************************/


#include "all.h"

#include <time.h>
#include <stdio.h>

#include <openssl/bn.h>


/***************************************
函数名: get_sys_time
功能: 获取系统时间
***************************************/

void get_sys_time(struct tm *timenow)
{
	time_t	 now;		  //实例化time_t结构
	time(&now);
	//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now

	timenow = gmtime(&now);

	//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
//	printf("Local	time   is	%s\n",asctime(timenow));
	//上句中asctime函数把时间转换成字符，通过printf()函数输出
}


int __crc(char *buf, int len)
{
	int i;
	int crc = 0x55;
	
	for(i = 0; i < len; i++)
	{
		crc = crc ^ buf[i];
	}
	return crc;
}

/* 对头部进行数据校验 */
int check_head_crc(struct check_head *head)
{
#if 0
	int crc = 0;
	crc = __crc((char *)head, sizeof(struct check_head) - sizeof(int));

//	printf("recv crc is %d \r\n", head->crc);

//	printf("crc is %d \r\n", crc);
	
	if(crc == head->crc)
		return 0;
	return -1;
#endif
	return 0;
}


/* 修改头部的事务标识。同时重新填写校验 */
void update_head(struct check_head *head, unsigned int air)
{
	head->affairs = air;
	head->crc = __crc((char *)head, sizeof(struct check_head) - sizeof(int));
}

int dump_flg = 0;
void dump_data(char *buf, int len)
{
	if(dump_flg)
	{
		hexdump(stdout, "== data ==",  
                    (char *)buf,  
                    len);
	}
}

/***************************************
函数名: __compages_head
功能: 构造头部
***************************************/
volatile unsigned int key_cnt = 0;

void __compages_head(struct check_head *head, unsigned int air, char *name, unsigned int passwd)
{
	
	struct tm timenow;
	/* 获取系统时间 */
	get_sys_time(&timenow);
	
	head->affairs = air;
/*
	head->type = sys_cfg.sys_type;
	memcpy(head->MID, sys_cfg.mastername, sizeof(sys_cfg.mastername));
*/
	memcpy(head->name, name, USER_NAME_LEN);	
	head->passwd = passwd;
//	memcpy(head->version, CLIENT_VERSION, strlen(CLIENT_VERSION));
/*
	head->timecnt[0] = timenow.tm_sec;
	head->timecnt[1] = timenow.tm_min;
	head->timecnt[2] = timenow.tm_hour;
	head->timecnt[3] = timenow.tm_mday;
	head->timecnt[4] = timenow.tm_mon;
	head->timecnt[5] = timenow.tm_year;
*/
	key_cnt ++;

//	printf("key_cnt %d \r\n", key_cnt);
	head->key = key_cnt;

	/* 加个包校验 */
	head->crc = __crc((char *)head, sizeof(struct check_head) - sizeof(int));

//	printf("crc is %d \r\n", head->crc);
//	memset(head->zero, 0, sizeof(head->zero));

/*
	aes256_data((char *)buf_in, (char *)head, len_s,
				aes256_key, sizeof(aes256_key), AES_ENCRYPT);
*/
}

/***************************************
函数名: __strcpy
功能: 安全级别的字符串拷贝函数
***************************************/

int __strcpy(char *dest, const char *src, int len)
{
	if(strlen(src) > len)
		return 0;
	strcpy(dest, src);
	return strlen(src);
}



/*********************************************
字符串模拟大数字。实现运算
**********************************************/
char* __strrev(char* s)  
{  
	/* h指向s的头部 */	
	char* h = s;	  
	char* t = s;  
	char ch;  
  
	/* t指向s的尾部 */	
	while(*t++){};	
	t--;	/* 与t++抵消 */  
	t--;	/* 回跳过结束符'\0' */	
  
	/* 当h和t未重合时，交换它们所指向的字符 */	
	while(h < t)  
	{  
		ch = *h;  
		*h++ = *t;	  /* h向尾部移动 */  
		*t-- = ch;	  /* t向头部移动 */  
	}  
  
	return(s);	
}  
	

/*	 
* 高精度数	
* 结果存储在字符串a中，字符串a初始为0  
* 计算结束，将字符串a翻转即为高精度数a,b的和  
*/	  
void __add(char *a, char *b)	  
{	 
	__strrev(a);	
	__strrev(b);	//reverse string b	  
	int la = strlen(a); //cal the length of a	 
	int lb = strlen(b); //cal the length of b	 
	int i = 0, j = 0, k = 0, s = 0, inc = 0;	
	while(i < la && j < la)    
	{	 
			s = (a[i] & 0XF) + (b[j] & 0XF) + inc;	  
			a[k++] = (s % 10) + '0'; //add b to a	 
			inc = s / 10; //store the carry    
			i++; j++;	 
	}	 
	while(i < la) //length of a is greater than b	 
	{	 
			s = (a[i] & 0XF) + inc;    
			a[k++] = (s % 10) + '0';	
			inc = s / 10;	 
			i++;	
	}	 
	while(j < lb) //length of b is greater than a	 
	{	 
			s = (b[j] & 0XF) + inc;    
			a[k++] = (s % 10) + '0';	
			inc = s / 10;	 
			j++;	
	}	 
	if(inc != 0) //at last,if the carry is not 0,store it into a	
			a[k++] = inc + '0';    
	__strrev(a);	
}  



//高精度数减法  
char *__sub(char *ans, char *num1, char *num2)  
{    
    int i, max, len, len1, len2, k;  
  
    len1 = strlen(num1);  
    len2 = strlen(num2);  
    len = len1;  
    max = len;  
    char a[100];  
    char b[100];  
    for(i = 0; i <= len - 1; i++)  
    {  
        a[i] = 0;  
        b[i] = 0;  
    }  
    k=0;  
    for(i = len1 - 1; i >= 0; i--)  
        a[k++] = num1[i] - '0';  
    k = 0;  
    for(i = len2 - 1; i >= 0; i--)  
        b[k++] = num2[i] - '0';  
    for(i = 0; i <= len - 1; i++)  
    {  
        if(a[i] < b[i])  
        {  
            a[i + 1]--;  
            a[i] = a[i] + 10;  
            a[i] = a[i] - b[i];  
        }  
        else   
            a[i] = a[i] - b[i];  
    }  
    while(a[len] == 0 && len > 0 || len >= max)   
        len--;  
    for(i = len; i >= 0; i--)   
        ans[i] = a[i] + '0';  
  
    return __strrev(ans);  
} 


void big_sub(char *sub, char *a, char *b)
{
	char *p;
	BIGNUM  *big_a = BN_new ();
	BIGNUM  *big_b = BN_new ();
	BIGNUM  *big_c = BN_new ();

	BN_dec2bn(&big_a, a);
	BN_dec2bn(&big_b, b);

	BN_sub(big_c, big_a, big_b);

	p = BN_bn2dec(big_c);
	strcpy(sub, p);

	OPENSSL_free(p);

	BN_free(big_a);
	BN_free(big_b);
	BN_free(big_c);
}



void big_add(char *sum, char *a, char *b)
{
	char *p;
	BIGNUM  *big_a = BN_new ();
	BIGNUM  *big_b = BN_new ();
	BIGNUM  *big_c = BN_new ();

	BN_dec2bn(&big_a, a);
	BN_dec2bn(&big_b, b);

	BN_add (big_c, big_a, big_b);

	p = BN_bn2dec(big_c);
	strcpy(sum, p);

	OPENSSL_free(p);

	BN_free(big_a);
	BN_free(big_b);
	BN_free(big_c);
}



void __debug_data(void)
{
	char a[100] = "4563287456921565443";
	char b[100] = "543218524621569756";
	char c[100];

    big_add(c, a, b);
    printf("%s + %s = %s \r\n", a, b, c);
    
    __sub(c, a, b);
    printf("%s - %s = %s \r\n", a, b, c);
}




/* 把数字用字符串显示出来 */
void __buf_to_str(unsigned char *dest, unsigned char *buf, int len)
{
#if 1
	memcpy(dest, buf, len);
#else
	int i;
	int j = 0;
	unsigned char th,tl;
	for(i = 0; i < len; i++)
	{
		th = buf[i]/16;
		tl = buf[i]%16;
		if(th < 10)
			dest[j++] = (th + 0x30);
		else
			dest[j++] = (th - 10 + 97);
		
		if(tl < 10)
			dest[j++] = (tl + 0x30);
		else
			dest[j++] = (tl - 10 + 97);
	}
	dest[j] = 0;
#endif
}


void __str_to_buf(unsigned char *dest, unsigned char *str)
{
#if 1
	memcpy(dest, str, strlen(str) + 1);
#else

	unsigned char th,tl;
	unsigned char _th, _tl;
	int j = 0;
	unsigned char tmp;

	while((*str))
	{
		th = *str;
		str ++;
		tl = *str;
		str ++;

		if(th>='A'&&th<='Z') 
			th += 32;
		if(tl>='A'&&tl<='Z') 
			tl += 32;

		if(th < ('9' + 1))
			_th = (th - 0x30);
		else
			_th = (th + 10 - 97);
//		printf("(%x)", _th);

		if(tl < ('9' + 1))
			_tl = (tl - 0x30);
		else
			_tl = (tl + 10 - 97);

		tmp = (_th << 4);
		tmp = tmp | _tl;

		dest[j] = tmp;
		j ++;
	}
#endif
}


