
/*******************************************************
	服务器的配置文件
*******************************************************/

#define __SERVER_CONF_C__

#include "all.h"
#include "server.h"

struct json_object *server_conf_json;


/* 默认配置 */
int server_conf_default(void)
{
	char name[30];
	server_conf_json = json_object_new_object();

	if(NULL == server_conf_json)
		return -1;

	server_info.server_port = 8000;

	//端口号
	json_object_object_add(server_conf_json, "server_port", 
							json_object_new_int(server_info.server_port));

	/* 分布式服务器 */
	{
		struct json_object *yk_obj;
		yk_obj = json_object_new_object();
		if(yk_obj == NULL)
			return -1;
		
		json_object_object_add(server_conf_json, "server_list", 
							yk_obj);
	}

	printf_json((char *)json_object_to_json_string(server_conf_json));

	return 0;
}



void get_server_conf(void)
{
	__json_key_to_int(server_conf_json, "server_port", &(server_info.server_port));
	
}





/*
从json 结构体中获取系统信息
*/
int server_conf_init(void)
{
	int fd;
	
	if((fd = open("server_conf.json", O_RDWR)) == -1){
		perror("open server_conf err \r\n");
		server_conf_default();
		save_json_to_file((char *)json_object_to_json_string(server_conf_json), "server_conf.json");

		return 1;
	}else{
		server_conf_json = json_object_from_file("server_conf.json");
		app_printf("server_conf.json : \r\n");
		printf_json((char *)json_object_to_json_string(server_conf_json));

		get_server_conf();
		return 0;
	}
}




