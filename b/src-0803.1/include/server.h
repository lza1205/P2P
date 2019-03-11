
#ifndef __SERVER_H_
#define __SERVER_H_


#ifdef __SERVER_C_
#define __SERVER_EXT_
#else
#define __SERVER_EXT_ extern
#endif

#include "all.h"
#include "config.h"

#include "server_sql.h"

#include "server_recv.h"

#include "hashtable.h"
#include "hashtable_itr.h"

#include "p2p_hashtable.h"

#include "server_aff.h"

#include "server_updata.h"


#define compages_head(head, air) __compages_head(head, air, server_info.myname, server_info.passwd)


struct __server_info{
	int sockfd;			/* ·þÎñÆ÷Ì×Ç©×Ö */
	char myname[20];
	unsigned int passwd;
};



__SERVER_EXT_  struct hashtable *h_table;
__SERVER_EXT_  struct __server_info server_info;




#endif

