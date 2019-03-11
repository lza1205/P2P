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
	app_printf("passwd           %s\r\n", sys_cfg.passwd);
	
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
	get_client_info(gp_comman_tag[1]);
}


void __send(void)
{
	sendto_node(gp_comman_tag[1], gp_comman_tag[2], strlen(gp_comman_tag[2]) + 1);
}


extern void printf_client_list(void);
extern void printf_p2p_test_node_list(void);
extern void printf_p2p_sync_node_list(void);


/********************************************
客户端命令行
********************************************/
struct command_t gt_comman_client[] = {
	{
		.name 		= "info",
		.com_fun 	= printf_info,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help 		= "get my info",
	},

	{
		.name		= "help",
		.com_fun	= __help,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help 		= "show help",
	},

	{
		.name		= "client_list",
		.com_fun	= printf_client_list,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "get my info",

	},
	
	{
		.name		= "p2p_test_list",
		.com_fun	= printf_p2p_test_node_list,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "get my info",

	},	
	{
		.name		= "p2p_sync_list",
		.com_fun	= printf_p2p_sync_node_list,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "get my info",

	},	
	{
		.name		= "?",
		.com_fun	= __help,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help 		= "show help",
	},

	{
		.name		= "call",
		.com_fun	= __get_info,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "you can call client",
	},

	{
		.name		= "send",
		.com_fun	= __send,
		.tag_num	= 0,
		.tag_p		= NULL,
		.help		= "you can call client",
	},

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


