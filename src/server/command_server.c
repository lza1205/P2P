/***********************************************************
文件名	:	command_server.c
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.15

说明:	
	服务器支持的命令行

***********************************************************/


#include "all.h"
#include "server.h"
#include "command.h"


extern char *gp_comman_tag[10];		//命令行参数


void printf_client_info(void)
{
	int i;
//    struct key *kk;
    struct value *v;
    struct hashtable_itr *itr;

    itr = hashtable_iterator(h_table);
    i = 0;
    if (hashtable_count(h_table) > 0)
    {
        do {
//            kk = hashtable_iterator_key(itr);
            v = hashtable_iterator_value(itr);

			app_printf("[%d] name                  %s\r\n", i, v->cli_info.name);
			app_printf("[%d] passwd               %d\r\n", i, v->cli_info.passwd);
			app_printf("[%d] version              %s\r\n", i, v->cli_info.version);
			app_printf("[%d] ip                   %s\r\n", i, v->cli_info.ip);
			app_printf("[%d] port                 %d\r\n", i, v->cli_info.port);
			app_printf("[%d] lan_ip               %s\r\n", i, v->cli_info.lan_ip);
			app_printf("[%d] lan_port             %d\r\n", i, v->cli_info.lan_port);
						
			app_printf("[%d] login_cnt            %d\r\n", i, v->cli_info.login_cnt);
			app_printf("\n");
			 i++;
        } while (hashtable_iterator_advance(itr));
    }
	free(itr);
}


static void __updata_client_for_name(char *name)
{
	struct sockaddr_in remote;
	struct value *currentuser = NULL;
	struct check_head head;

	/* 从哈希表中找到内容 */
	currentuser = insert_hashtable_value(name);

	if(NULL == currentuser)
	{
		dbg_printf("no found %s \r\n", name);
		return ;
	}
	
	remote.sin_family = AF_INET;
	remote.sin_port = htons(currentuser->cli_info.port);
	remote.sin_addr.s_addr = inet_addr(currentuser->cli_info.ip);

	compages_head(&head, _aff_server_updata_);
	
    p2p_sendto(server_info.sockfd, &head, sizeof(head), 0, 
						(struct sockaddr *)&remote, sizeof(struct sockaddr));

}



void __updata_client_for_all(void);

static void updata_client(void)
{
//	int fd;
	char *name = gp_comman_tag[1];
#if 0
	if((fd = open(path, O_RDWR)) == -1){
		perror(path);
		return -1;
	}
#endif
	dbg_printf("update...\n");
	
	if(strcmp(name, "all") == 0)		//如果是升级全部的话
	{
		__updata_client_for_all();	//升级全部
	}else{
		__updata_client_for_name(name);
	}
}


extern char updata_client_version[100];


void __setenv(void)
{
	if(strcmp(gp_comman_tag[1], "version") == 0)
	{
		strcpy(updata_client_version, gp_comman_tag[2]);
	}
}

void __printenv(void)
{
	printf("----------------------------------\r\n");
	printf("env: \r\n");
	printf("version = %s\r\n", updata_client_version);
	printf("----------------------------------\r\n");
}

extern int dump_flg;
void __dump(void)
{
	if(strcmp(gp_comman_tag[1], "on"))
	{
		dump_flg = 1;
	}else if(strcmp(gp_comman_tag[1], "off")){
		dump_flg = 0;
	}
}


struct command_t gt_comman_server[] = {
	{
		.name 		= "cli_info",
		.com_fun 	= printf_client_info,
		.tag_num	= 0,
		.tag_p		= NULL,
	},
	{
		.name		= "updata_client",
		.com_fun 	= updata_client,
		.tag_num	= 0,
		.tag_p		= NULL,
	},
	{
		.name		= "setenv",
		.com_fun 	= __setenv,
		.tag_num	= 0,
		.tag_p		= NULL,
	},
	{
		.name		= "print",
		.com_fun 	= __printenv,
		.tag_num	= 0,
		.tag_p		= NULL,
	},
	{
			.name		= "dump",
			.com_fun	= __dump,
			.tag_num	= 0,
			.tag_p		= NULL,
			.help		= "dump recv data",
	},
	{
		.name		= NULL,
		.com_fun 	= NULL,
		.tag_num	= 0,
		.tag_p		= NULL,
	},
};


void server_command_init(void)
{
	gt_comman = gt_comman_server;
}


