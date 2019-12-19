
#ifndef __SERVER_CONF_H__
#define __SERVER_CONF_H__

#ifdef __SERVER_CONF_C__
#define __SERVER_CONF_EXT__
#else
#define __SERVER_CONF_EXT__ extern
#endif

#include "all.h"
#include "server.h"

#define P2P_SERVER_SQL_AUTO_INSERT		0

//最多支持多少个客户端 0 表示不限制
#define QDY_DEF_CLIENT_NUM	20

//是否需要数据库认证
#define QDY_EN_MYSQL		1


__SERVER_CONF_EXT__ struct json_object *server_conf_json;


#endif

