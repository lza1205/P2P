#define __SERVER_C_

#include "all.h"
#include "server.h"

void server_command_init(void);

int main(int argc, char *argv[])
{
	
	char command[1024];
	char *str;

	/* 开启打印信息 */
	set_app_printf(1);
	set_dbg_printf(1);
	set_sync_printf(1);
	
	/* 创建哈希表 */
	h_table = create_hashtable(16, hashfromkey, equalkeys);
	if(NULL == h_table){
		app_printf("create_hashtable err \n");
		return -1;
	}

	/* AES256 加密 */
	aes256_key_init();
//	set_aes256(1);			//开启 AES 256 加密方式

	/* 数据库初始化 */
	service_sql_init();
//	__mysql_debug();	/* 测试 */

	/* 服务器socket 初始化 */
	init_server_sock_pthread();

	/* 初始化命令行 */
	server_command_init();

	/* TCP 子线程 */
	server_updata_init();

//	__mysql_debug();

	while(1)
	{
        str = fgets(command, 1024, stdin);
		if(str != NULL)
		{
	        command_format(command);
			printf("server @: $ ");
		}

	}
	
	return 0;
}


