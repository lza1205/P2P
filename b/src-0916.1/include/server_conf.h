
#ifndef __SERVER_CONF_H__
#define __SERVER_CONF_H__

#ifdef __SERVER_CONF_C__
#define __SERVER_CONF_EXT__
#else
#define __SERVER_CONF_EXT__ extern
#endif

#include "all.h"
#include "server.h"


__SERVER_CONF_EXT__ struct json_object *server_conf_json;


#endif

