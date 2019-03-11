/***********************************************************
文件名	:	p2p_json.h
作者		:	Faker
版本号	:	1.0
日期		:	2015.05.13

说明:
		公共的json 功能函数

***********************************************************/

#ifndef __P2P_JSON_H_
#define __P2P_JSON_H_

#include "all.h"

/*
http://twobit.blog.chinaunix.net/uid-23412956-id-3784975.html

*/


typedef struct _key_obj{
    char *key;
    struct json_object *obj;
    struct json_object *parent_obj;

}key_obj;



#define __json_object_object_foreach(obj,key,val) \
 char *key; struct json_object *val; struct lh_entry *entry; \
 for(entry = json_object_get_object(obj)->head; (entry ? (key = (char*)entry->k, val = (struct json_object*)entry->v, entry) : 0); entry = entry->next)


void printf_json(const char *json_str);
int __json_tokener_parse(char *str, struct json_object *new_obj);
int __json_key_to_string(struct json_object *new_obj, char *key, char *str, int len);
int __json_key_to_int(struct json_object *new_obj, char *key, int *val);


int get_top_node(const char *str, char *top_node_buf, int buf_len);
int json_get_obj(struct json_object *obj, const char *str, key_obj* output_obj);
int json_set_value(struct json_object *root_obj, const char *str, const char *val);
const char * json_get_value(struct json_object *root_obj, const char *str);



int save_json_to_file(const char *json_str, char *path);

#endif


