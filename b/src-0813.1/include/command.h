/***********************************************************
文件名	:	command.h
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.15

说明:	
	命令行的核心代码

***********************************************************/

#ifndef __COMMAND_H_
#define __COMMAND_H_

#include "all.h"

/* 命令行结构体 */
struct command_t{
	char *name;
	void (*com_fun)(void);
	char tag_num;
	char *tag_p;
	char *help;
//	struct list_head list;
};

struct command_t *gt_comman;


void command_format(char *str);


#endif


