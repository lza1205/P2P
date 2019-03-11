
#ifndef __TIMER_LIST_H__
#define __TIMER_LIST_H__

#include "my_list.h"

struct my_timer{
	int sec;		//秒数
	void *arg;		//参数
	void (*handle)(void *pdata);	//处理函数

	struct list_head list;	/* 链表 */
};


void my_timer_init(void);


#endif

