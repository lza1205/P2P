
#ifndef __ALL_H_
#define __ALL_H_


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <Windows.h>
#include <pthread.h>
#include <malloc.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <netdb.h>

#include <semaphore.h>
//#include <asm/semaphore.h>

#include <sys/sem.h>


#include "stdio.h"
#include "stdlib.h"

#include <arpa/inet.h>
//#include <ifaddrs.h>

#include <linux/sockios.h>
#include <linux/if.h>
#include <sys/ioctl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* for memcmp */

#include <pthread.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <stdio.h>


/* json */
#ifdef _OPENWRT_
//#define __STRICT_ANSI__
#include <json-c/json.h>
#include <json-c/json_util.h>
#else
//#define __STRICT_ANSI__
#include <json/json.h>
#include <json/json_util.h>
#endif


#include "config.h"

#include "agent_proto.h"

#include "debug.h"

#include "aes256_data.h"
#include "my_socket.h"

#include "p2p_json.h"

#include "command.h"

#include "p2p_data.h"





#endif

