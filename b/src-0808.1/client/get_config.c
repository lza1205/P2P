#include "all.h"
#include "client.h"


/*
功能：计算字符串s的（unsigned int型）长度，不包括'\0'在内
说明：返回s的长度，不包括结束符NULL。
*/
unsigned int STR_strlen(char *str) 
{ 
  unsigned int len = 0; 
  while((*str++) != '\0') 
  len++; 
  return len; 
}

/*
功能：找出str2字符串在str1字符串中第一次出现的位置（不包括str2的串结束符）。
返回值：返回该位置的指针，如找不到，返回空指针。
*/
char *STR_strstr(char *s1, char *s2 )
{ 
  int len2; 
  if ( !(len2 = STR_strlen(s2)) ) 
  return s1; 
  for ( ; *s1; ++s1 )
  { 
    if ( *s1 == *s2 && strncmp((const char*) s1, (const char*)s2, len2 )==0 ) 
    return s1; 
  } 
  return NULL; 
} 

/*******************************************
函数名:	try_hostname
功能:		对主机名称进行解析
*********************************************/

int try_hostname(void)
{
	struct hostent *host;

	if((host=gethostbyname(sys_cfg.serverip)) == NULL) {
		herror("gethostbyname err! >> ");
		return -1;
	}

	__strcpy(sys_cfg.serverip, inet_ntoa(*((struct in_addr *)host->h_addr)), IP_STRING_MAX);
	return 0;
}


#if 0
void str2int(char *str)
{
	char *s = str;
	int i = 0, j = 0;
	
	while((*s) ++){
		j ++;
	}
	
}
#endif

/*******************************************
函数名:	try_hostname
功能:		检查下是否有配置文件
		并取出配置文件的值
*********************************************/

int check_config(void)
{
#if 0
	int fd;
	char buf[1024];
	int n;
	char *str;
	int i;
	char port_buf[10];
	int ret = 0;
	int port;

	if((fd = open("agent_config", O_RDONLY)) != -1){
		if((n = read(fd, buf, sizeof(buf))) <= 0){
			perror("read config :");
			goto scanf_port;
			
		}

		close(fd);
		
		buf[n] = 0;
//		app_printf("config : \n%s", buf);

		i = 0;
		if((str = STR_strstr(buf, "my port = ")) == NULL){
			app_printf("please input your port: ");
			ret = scanf("%d", &port);
			sys_cfg.my_port = port;
			
		}else{
			
			str = str + strnlen("my port = ", 30);
			while(*str != '\n'){
				port_buf[i] = *str;
				i ++;
				str ++;
			}
			port_buf[i] = 0;
			sys_cfg.my_port = atol(port_buf);
		}

		i = 0;
		if((str = STR_strstr(buf, "server port = ")) == NULL){
			app_printf("please input server port: ");
			ret = scanf("%d", &port);
			sys_cfg.ser_port = port;
			
		}else{
			
			str = str + strnlen("server port = ", 30);
			while(*str != '\n'){
				port_buf[i] = *str;
				i ++;
				str ++;
			}
			port_buf[i] = 0;
			sys_cfg.ser_port = atol(port_buf);
		}

		i = 0;
		if((str = STR_strstr(buf, "serverip = ")) == NULL){
			app_printf("please input server ip or Domain Name : ");
			ret = scanf("%s", sys_cfg.serverip);
			try_hostname();
		}else{
			
			str = str + strnlen("serverip = ", 30);
			while(*str != '\n'){
				sys_cfg.serverip[i] = *str;
				i ++;
				str ++;
			}
			try_hostname();
		}

		i = 0;
		if((str = STR_strstr(buf, "name = ")) == NULL){
			app_printf("please input name: ");
			ret = scanf("%s", sys_cfg.myname);
		}else{
			str = str + strnlen("name = ", 30);
			
			while(*str != '\n'){
				sys_cfg.myname[i] = *str;
				i ++;
				str ++;
			}
		}
		
		i = 0;
		if((str = STR_strstr(buf, "passwd = ")) == NULL){
			app_printf("please input your passwd: ");
			ret = scanf("%s", sys_cfg.passwd);
		}else{
			str = str + strnlen("passwd = ", 30);
			while(*str != '\n'){
				sys_cfg.passwd[i] = *str;
				i ++;
				str ++;
			}
		}
		
	}else{ 
		app_printf("no config\n");
scanf_port:		
		app_printf("please input your port: ");
		ret = scanf("%d", &port);
		sys_cfg.my_port = port;
//scanf_serip:
		/*	输入服务器IP 和自己的名称*/
		app_printf("please input server ip or Domain Name : ");
		ret = scanf("%s", sys_cfg.serverip);
		try_hostname();
		
//scanf_name:
		app_printf("please input name: ");
		ret = scanf("%s", sys_cfg.myname);
		
//scanf_passwd:
		app_printf("please input passwd: ");
		ret = scanf("%s", sys_cfg.passwd);
	}

	app_printf("read config:\n%d\n%s\n%d\n%s\n%s\n", sys_cfg.my_port, sys_cfg.serverip, 
					sys_cfg.ser_port, sys_cfg.myname,sys_cfg.passwd);
	return ret;
#endif
}




struct json_object *sys_info_json;

char log_path[100] = "./";		//日志文件路径

void __str_mh(char *dest, char *src, char h)
{
	while((*src))
	{
		if((*src) != h)
		{
			*dest = *src;
			dest ++;
		}
		src ++;
	}
}

/* 随机名字 */
void rand_name(void)
{
#if 1
	char mac[20];
	int i;
	srand((int)time(NULL));     //每次执行种子不同，生成不同的随机数
	for(i = 0; i < 4; i++)
	{
		sys_cfg.myname[i] = 'A'+rand()%26;
	}
	sys_cfg.myname[i] = '-';
	get_mac("eth0", mac, &(sys_cfg.myname[i + 1]));
#else
	/* 名字规则
		名字长度8 个字节
		0 1 字节为随机数字
		2 3 4 5 6 7 8 字节为网卡mac地址
	*/
	int i;
	char mac[20], mac_str[20];

	get_mac("eth0", &(sys_cfg.myname[2]), mac_str);

	srand((int)time(NULL));     //每次执行种子不同，生成不同的随机数
	for(i = 0; i < 2; i++)
	{
		sys_cfg.myname[i] = rand()%0xff;
	}
#endif
}



/* 把数字用字符串显示出来 */
void __buf_to_str(char *dest, char *buf, int len)
{
	int i;
	int j = 0;
	char th,tl;
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
}


void __str_to_buf(char *dest, char *str)
{
	char th,tl;
	char _th, _tl;
	int j = 0;
	while((str))
	{
		th = *str;
		str ++;
		tl = *str;
		str ++;

		if(th < 10)
			_th = (th - 0x30);
		else
			_th = (th + 10 - 97);
		
		if(tl < 10)
			_tl = (tl - 0x30);
		else
			_tl = (tl + 10 - 97);

		dest[j] = (th << 4) | tl;
		j ++;
	}
}


/* 默认配置 */
int sys_info_default(void)
{
	char name[30];
	sys_info_json = json_object_new_object();

	if(NULL == sys_info_json)
		return -1;

	rand_name();
	sys_cfg.passwd = 0x12345678;
	strcpy(sys_cfg.serverip, "112.74.197.88");
	sys_cfg.ser_port = 8000;
	sys_cfg.tcp_port = 8080;
	sys_cfg.my_port = 3333;

	json_object_object_add(sys_info_json, "log_path", 
									json_object_new_string("./"));


//	__buf_to_str(name, sys_cfg.myname, 8);
	json_object_object_add(sys_info_json, "name", 
							json_object_new_string(sys_cfg.myname));
	
	json_object_object_add(sys_info_json, "passwd", 
							json_object_new_int(sys_cfg.passwd));
	
	json_object_object_add(sys_info_json, "server_ip", 
								json_object_new_string(sys_cfg.serverip));

	json_object_object_add(sys_info_json, "server_port", 
							json_object_new_int(sys_cfg.ser_port));

	json_object_object_add(sys_info_json, "tcp_port", 
							json_object_new_int(sys_cfg.tcp_port));

	json_object_object_add(sys_info_json, "my_port", 
							json_object_new_int(sys_cfg.my_port));

	json_object_object_add(sys_info_json, "up_total_flow", 
							json_object_new_string("0"));

	json_object_object_add(sys_info_json, "down_total_flow", 
							json_object_new_string("0"));

	printf_json((char *)json_object_to_json_string(sys_info_json));

	return 0;
}



void __get_config_for_json()
{
	char __path[100];
	
	__json_key_to_string(sys_info_json, "name", sys_cfg.myname, USER_NAME_LEN);
	printf("my name is %s\r\n", sys_cfg.myname);
//	__str_to_buf(sys_cfg.myname, name);

	__json_key_to_int(sys_info_json, "passwd", &(sys_cfg.passwd));
	__json_key_to_string(sys_info_json, "server_ip", sys_cfg.serverip, IP_STRING_MAX);
	__json_key_to_int(sys_info_json, "server_port", &(sys_cfg.ser_port));
	__json_key_to_int(sys_info_json, "tcp_port", &(sys_cfg.tcp_port));
	__json_key_to_int(sys_info_json, "my_port", &(sys_cfg.my_port));

	__json_key_to_string(sys_info_json, "log_path", log_path, 100);
	
	sys_cfg.log_path = log_path;
}





/*
从json 结构体中获取系统信息
*/
int get_config_for_json(void)
{
	int fd;
	
	if((fd = open("sys_info.json", O_RDWR)) == -1){
		perror("open sys_info err \r\n");
		sys_info_default();

		//json_object_to_file("sys_info.json", sys_info_json);
		save_json_to_file((char *)json_object_to_json_string(sys_info_json), "sys_info.json");

		return 1;
	}else{
		sys_info_json = json_object_from_file("sys_info.json");
		app_printf("sys_info.json : \r\n");
		printf_json((char *)json_object_to_json_string(sys_info_json));

		__get_config_for_json();
		return 0;
	}
}


