/***********************************************************
文件名	:	command.c
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.15

说明:	
	命令行的核心代码

***********************************************************/




#include "all.h"

char *gp_comman_tag[10];		//命令行参数
int g_comman_num;		//命令行参数个数


/********************************************
函数名: __str_cmp
功能:		比较两个字符串是否完全相等
*********************************************/

int __str_cmp(char *str1, char *str2)
{
	int i = 0;
	while(((*str2) != '\0') && ((*str1) != '\0'))
	{
		if((*str1) != (*str2))
			return 0;
		str1 ++;
		str2 ++;
		i ++;
	}
	if(((*str2) == '\0') && (((*str1) == '\0')))
	{
		return i;
	}
	return 0;
}



/********************************************
函数名: __str_cmp
功能:		对用户输入的字符串进行格式化
			分解参数
*********************************************/

void command_format(char *str)
{
	char *tmp_p;
	int i;
	int err = -1;
	
	tmp_p = str;
	i = 0;

	if((*str) == '\n')
		return;
	if((*str) == '\r')
		return;
	
	g_comman_num = 0;
	gp_comman_tag[i++] = tmp_p;
	while((*tmp_p) != '\0')
	{
		if(((*tmp_p) == ' ') || ((*tmp_p) == '\r') || ((*tmp_p) == '\n'))
		{
			if((*tmp_p) == '\r')
				tmp_p ++;
			(*tmp_p) = '\0';
			gp_comman_tag[i++] = tmp_p + 1;
			g_comman_num ++;
		}
		tmp_p ++;
	}

	
	for(i = 0; gt_comman[i].name != NULL; i++)	//一个个比对
	{
		//dbg_printf("%s %s \r\n", gp_comman_tag[0], gt_comman[i].name);
		err = strcmp(gp_comman_tag[0], gt_comman[i].name);
		if(err == 0)
		{
			break;
		}
	}
	if(err == 0)
		gt_comman[i].com_fun();
	else
		app_printf("command err \r\n");
}


