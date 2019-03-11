
#define __CLIENT_C_

#include "all.h"

#include "client.h"




void client_command_init(void);

int main(int argc, char *argv[])
{
	char command[1024];
	char *str;

	/* 开启打印信息 */
	set_app_printf(1);
	set_dbg_printf(1);
	set_sync_printf(1);

	/* AES256 加密初始化 */
	aes256_key_init();
	set_aes256(1);			//开启 AES 256 加密方式
	
	/* 检查下配置文件 */
	get_config_for_json();

	/* 初始化socket */
    init_client_socket((unsigned short)sys_cfg.my_port);

	memcpy(sys_cfg.now_version, CLIENT_VERSION, sizeof(sys_cfg.now_version));
	printf("\nclient version :         %s\n\n", sys_cfg.now_version);
	
	/* 初始化网络数据队列 */
	init_net_data_list();

	/* 启动udp 通信线程 */
	init_network_pthread();

	/* 初始化命令行 */
	client_command_init();

//	__debug_aff_net();
	
	while(1)
	{
        str = fgets(command, 1024, stdin);
		if(str != NULL)
		{
	        command_format(command);
			printf("client @: $ ");
		}

	}
	return 0;
}


