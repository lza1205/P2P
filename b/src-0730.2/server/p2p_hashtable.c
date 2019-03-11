/***********************************************************
文件名	:	p2p_hashtable.c
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.13

说明:
		服务器的哈希表功能函数

***********************************************************/

#include "all.h"

#include "server.h"

/***********************************************************
功能: 哈希表相关
***********************************************************/
/*****************************************************************************/
DEFINE_HASHTABLE_INSERT(insert_some, struct key, struct value);
DEFINE_HASHTABLE_SEARCH(search_some, struct key, struct value);
DEFINE_HASHTABLE_REMOVE(remove_some, struct key, struct value);
DEFINE_HASHTABLE_ITERATOR_SEARCH(search_itr_some, struct key);
/*****************************************************************************/


/***************************************************
一下两个函数用于移植哈希链表
****************************************************/

/****************************************************************/

unsigned int
hashfromkey(void *ky)
{
    struct key *k = (struct key *)ky;
#if 0
    return (((k->one_ip << 17) | (k->one_ip >> 15)) ^ k->two_ip) +
            (k->one_port * 17) + (k->two_port * 13 * 29);
#else
//	printf("hashfromkey   %d\n", atoi(k->name));
	return atoi(k->name);
#endif
}

int
equalkeys(void *k1, void *k2)
{
	struct key *kk1, *kk2;
	kk1 = (struct key *)k1;
	kk2 = (struct key *)k2;
	
    return (0 == strcmp(kk1->name,kk2->name));
}
/****************************************************************/



/***************************************
函数名: __search_some
功能:从哈希链表中添加元素
***************************************/

struct value *__search_some(struct hashtable *h_table, struct key *k)
{
	return search_some(h_table,k);
}

/***************************************
函数名: __search_some
功能:从哈希链表中删除元素
***************************************/

struct value *__remove_some(struct hashtable *h_table, struct key *k)
{
	return remove_some(h_table,k);
}




/***************************************
函数名: find_hashtable_value
功能:根据KEY 从哈希链表中找到val
		如果没有这个val 则插入
**************************************/

struct value *insert_hashtable_value(char *name)
{
	struct value *currentuser;
	struct value *found;
	struct key *k;

	/* 哈希表 */
	k = (struct key *)malloc(sizeof(struct key));
	if(k == NULL){
//		app_printf("malloc key err\n");
		return NULL;
	}

	__strcpy(k->name, name, USER_NAME_LEN);


	/* 查看下是否有登陆过 */
	if(NULL == (found = search_some(h_table,k)))
	{
//		app_printf("frist\n");
		currentuser = (struct value *)malloc(sizeof(struct value));
		if(currentuser == NULL){
			free(k);
//			app_printf("malloc err\n");
			return NULL;
		}
		/* 插入到哈希表 */
		if (!insert_some(h_table,k,currentuser)){
			free(k);
			free(currentuser);
//			app_printf("insert_some err\n");
			return NULL;
		}
	}else{
		currentuser = found;
		free(k);		//释放内存
//		dbg_printf("find\n");
	}

	if(NULL == currentuser)
	{
//		app_printf("search_some err \n");
		return NULL;
	}

	return currentuser;

}


/***************************************
函数名: fine_value
功能:根据KEY 从哈希链表中找到val
		如果没有这个val 则返回NULL
**************************************/

struct value *fine_value(char *name)
{
	struct key *k;
	struct value *currentuser = NULL;
	struct value *found = NULL;
	
	k = (struct key *)malloc(sizeof(struct key));
	if(k == NULL){
		app_printf("malloc key err\n");
		return NULL;
	}

	__strcpy(k->name, name, USER_NAME_LEN);	


	/* 查看下是否有登陆过 */
	if(NULL == (found = search_some(h_table, k)))
	{
		printf("no login......\n");
		return NULL;		
	}else{
		currentuser = found;
	}

	return currentuser;
}


