/***********************************************************
文件名	:	command_client.c
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.15

说明:	
	客户端支持的命令行

***********************************************************/



#include "all.h"
#include "client.h"
#include "command.h"

extern char *gp_comman_tag[10];		//命令行参数
extern int g_comman_num;		//命令行参数个数



/********************************************
函数名: printf_info
功能:		打印自身的信息
*********************************************/

void printf_info(void)
{
	app_printf("version          %s\r\n", sys_cfg.now_version);
	app_printf("app name         %s\r\n", sys_cfg.myname);
	app_printf("passwd           %d\r\n", sys_cfg.passwd);
	
	app_printf("wan ip           %s\r\n", sys_cfg.wan_ip);
	app_printf("wan port         %d\r\n", sys_cfg.wan_port);
	
	app_printf("lan ip           %s\r\n", sys_cfg.lan_ip);
	app_printf("lan port         %d\r\n", sys_cfg.lan_port);
	
	app_printf("server ip        %s\r\n", sys_cfg.serverip);
	app_printf("server port      %d\r\n", sys_cfg.ser_port);
	
}


/********************************************
函数名: __help
功能:		help
*********************************************/

static void __help(void)
{
	int i;
	app_printf("/------------------- Welcome to Faks -------------------/\r\n");
	app_printf("You can enter the following command : \r\n");

	for(i = 0; gt_comman[i].name != NULL; i++)	//一个个比对
	{
		app_printf("[%d]%s       :   %s \r\n", i, gt_comman[i].name, gt_comman[i].help);
	}

	app_printf("/------------------- --------------- -------------------/\r\n");
}



/********************************************
函数名: __get_info
功能:		call 某客户端
*********************************************/

static void __get_info(void)
{
	char buf[8];
	__str_to_buf(buf, gp_comman_tag[1]);
	get_client_info(buf);
}


void __send(void)
{
	char buf[8];
	__str_to_buf(buf, gp_comman_tag[1]);
	sendto_node_unreliable(buf, _proto_c_msg_, gp_comman_tag[2], strlen(gp_comman_tag[2]) + 1);
}


extern int dump_flg;
void __cli_dump(void)
{
	if(strcmp(gp_comman_tag[1], "on"))
	{
		dump_flg = 1;
	}else if(strcmp(gp_comman_tag[1], "off")){
		dump_flg = 0;
	}
}

/* 阻塞发送 */
void ___send(void)
{
	char buf[8];
	__str_to_buf(buf, gp_comman_tag[1]);
	sendto_node_reliable(buf, _proto_c_msg_, gp_comman_tag[2], strlen(gp_comman_tag[2]) + 1, 10);
}


/********************************************
函数名:	__test
功能:		测试
********************************************/
void __test(void)
{
	int ret;
	char buf[1024];
	struct sockaddr_in recv_addr;
	while(1)
	{
		char buf[8];
		__str_to_buf(buf, "1234567890abcdef");
		if(sendto_node_unreliable(buf, _proto_c_msg_, "qwerdf", 7) < 0)
		{
			printf("____!!!! sendto_node_unreliable qwer err \r\n");
		}else{
			printf("____**** sendto_node_unreliable qwer ok \r\n");
		}
		if(sendto_node_reliable(buf, _proto_c_msg_, "qwerdf", 7, 10) < 0)
		{
			printf("____!!!! sendto_node_reliable qwer err \r\n");
		}else{
			printf("____**** sendto_node_reliable qwer ok \r\n");
		}

		__str_to_buf(buf, gp_comman_tag[1]);
		if(sendto_node_unreliable(buf, _proto_c_msg_, "qwerdf", 7) < 0)
		{
			printf("____!!!! sendto_node_unreliable %s err \r\n", gp_comman_tag[1]);
		}else{
			printf("____**** sendto_node_unreliable %s ok \r\n", gp_comman_tag[1]);
		}
		if(sendto_node_reliable(buf, _proto_c_msg_, "qwerdf", 7, 10) < 0)
		{
			printf("____!!!! sendto_node_reliable %s err \r\n", gp_comman_tag[1]);
		}else{
			printf("____**** sendto_node_reliable %s ok \r\n", gp_comman_tag[1]);
		}
#if 0
		ret = recv_node_data(buf, 1024, &recv_addr, 5);
		if(ret < 0)
		{
			printf("____!!!! my recv_node_data is err \r\n");
		}else{
			printf("____**** my_thread_recv is ok \r\n");
		}
#endif
	}
}


extern int sound_file_end_flg;
/* 发送音频文件 */
void __sound_file(void)
{
	char buf[8];
	__str_to_buf(buf, gp_comman_tag[1]);
	sound_file_to(buf);
}

void __sound_file_end(void)
{
	sound_file_end_flg = 1;
}

extern int sound_talk_end_flg;

void __talk(void)
{
	char buf[8];
	__str_to_buf(buf, gp_comman_tag[1]);
	sound_talk_to(buf);
}

void __talk_end(void)
{
	sound_talk_end_flg = 1;
}


void __play(void)
{
	play_sound_file("sound_file");
}

extern void printf_client_list(void);
extern void printf_p2p_test_node_list(void);
extern void printf_p2p_sync_node_list(void);
extern void __printf_net_data_list(void);

/********************************************
客户端命令行
********************************************/
struct command_t gt_comman_client[] = {
	{
		.name 		= "info",
		.com_fun 	= printf_info,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help 		= "show my info",
	},

	{
		.name		= "help",
		.com_fun	= __help,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help 		= "show help",
	},
	
	{
		.name		= "?",
		.com_fun	= __help,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help 		= "show help",
	},

	{
			.name		= "send",
			.com_fun	= __send,
			.tag_num	= 0,
			.tag_p		= NULL,
			.help		= "you can send msg to client",
	},

	{
			.name		= "dump",
			.com_fun	= __cli_dump,
			.tag_num	= 0,
			.tag_p		= NULL,
			.help		= "dump recv data",
	},

/*
	{
			.name		= "client_list",
			.com_fun	= printf_client_list,
			.tag_num	= 0,
			.tag_p		= NULL,
			.help		= "show client listt",
	
		},
		
		{
			.name		= "p2p_test_list",
			.com_fun	= printf_p2p_test_node_list,
			.tag_num	= 0,
			.tag_p		= NULL,
			.help		= "show p2p test list",
	
		},	
		{
			.name		= "p2p_sync_list",
			.com_fun	= printf_p2p_sync_node_list,
			.tag_num	= 0,
			.tag_p		= NULL,
			.help		= "show p2p sync list",
	
		},

	{
		.name		= "call",
		.com_fun	= __get_info,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "you can call client",
	},

	

	{
		.name		= "_send",
		.com_fun	= ___send,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "you can send msg to client",
	},

	{
		.name		= "net_data_list",
		.com_fun	= __printf_net_data_list,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "show network data list",
	},

	{
		.name		= "test",
		.com_fun	= __test,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "test",
	},

	{
		.name		= "sound_file",
		.com_fun	= __sound_file,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "test",
	},
	
	{
		.name		= "sound_file_end",
		.com_fun	= __sound_file_end,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "test",
	},
			
	{
		.name		= "play",
		.com_fun	= __play,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "test",
	},

	{
		.name		= "talk",
		.com_fun	= __talk,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "test",
	},

	{
		.name		= "talk_end",
		.com_fun	= __talk_end,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "test",
	},
	*/

	{
		.name		= NULL,
		.com_fun 	= NULL,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help 		= NULL,
	},
	
};


void client_command_init(void)
{
	gt_comman = gt_comman_client;
}


