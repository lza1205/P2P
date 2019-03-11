
#ifndef __CLIENT_H_
#define __CLIENT_H_


#ifdef __CLIENT_C_
#define __CLIENT_EXT_
#else
#define __CLIENT_EXT_	extern
#endif

#include "all.h"
#include "client_recv.h"
#include "client_aff.h"
#include "client_net_data.h"
#include "get_config.h"

#include "updata.h"

#include "client_node_list.h"

#define compages_head(head, air) __compages_head(head, air, sys_cfg.myname, sys_cfg.passwd)

struct sys_config{
	int sockfd;			/* 套签字 */
	
	int wan_port;			/* 公网端口 */
	char wan_ip[IP_STRING_MAX];		/* 公网IP */
	
	int my_port;		/* 本地端口号 */
	
	int ser_port;		/* 服务器端口号 */
	int tcp_port;		/* 升级用的tcp 端口号 */
	char serverip[IP_STRING_MAX];	/* 服务器端口号 */

	int lan_port;			/* 局域网端口号 */
	char lan_ip[IP_STRING_MAX];		/* 局域网IP */


	char now_version[USER_NAME_LEN];	/* 版本号 */

	char passwd[USER_NAME_LEN];		/* 密码 */
	char myname[USER_NAME_LEN];		/* 我的名字 */

	char *log_path;				/* 日志文件路径 */
};

__CLIENT_EXT_ struct sys_config sys_cfg;


int get_mac(char *eth, char *mac);



#endif


