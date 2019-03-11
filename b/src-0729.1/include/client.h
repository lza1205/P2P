
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
	int sockfd;			/* ��ǩ�� */
	
	int wan_port;			/* �����˿� */
	char wan_ip[IP_STRING_MAX];		/* ����IP */
	
	int my_port;		/* ���ض˿ں� */
	
	int ser_port;		/* �������˿ں� */
	int tcp_port;		/* �����õ�tcp �˿ں� */
	char serverip[IP_STRING_MAX];	/* �������˿ں� */

	int lan_port;			/* �������˿ں� */
	char lan_ip[IP_STRING_MAX];		/* ������IP */


	char now_version[USER_NAME_LEN];	/* �汾�� */

	char passwd[USER_NAME_LEN];		/* ���� */
	char myname[USER_NAME_LEN];		/* �ҵ����� */

	char *log_path;				/* ��־�ļ�·�� */
};

__CLIENT_EXT_ struct sys_config sys_cfg;


int get_mac(char *eth, char *mac);



#endif

