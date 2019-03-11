
/***********************************************************
sound 基础函数
***********************************************************/

#include "all.h"
#include "client.h"


/*------------------------------------------------------------*/
/*
					通话相关的底层函数
*/


/* 向name 发送语音通话请求 */
void talk_request(char *name)
{
	sound_head_t s_head;

	s_head.type = _sound_talk_request_;

	sendto_node_reliable(name, _proto_c_audio_, (char *)&s_head, sizeof(sound_head_t), 20);
}



/* 发送应答 */
void send_sound_talk_ack(char *name, int ack)
{
	sound_head_t s_head;

	if(ack == 0)
		s_head.type = _sound_talk_ack_;
	else
		s_head.type = _sound_talk_noack_;
	
	
	sendto_node_reliable(name, _proto_c_audio_, (char *)&s_head, sizeof(sound_head_t), 20);
}




int recv_sound_talk_ack(listen_t *listen)
{
	int ret;
	char buf[512];
	struct sockaddr_in clientaddr;

	ret = recv_from_listen(listen, (struct sockaddr_in *)&clientaddr, buf, 1204, 10);

	if(ret == -1)
	{
		printf("%p recv is err \r\n", listen);
		return -1;		//时间到了退出
	}

	sound_head_t *s_head = (sound_head_t *)(buf + sizeof(struct check_head) + sizeof(struct proto_c_send_data));

	/* 对方是否准备好了 */
	if(s_head->type == _sound_talk_ack_)
	{
		return 0;
	}else{
		return -1;
	}
}



int sound_talk_cnt;
int send_sound_talk(char *name, char *buf, int len)
{
	sound_head_t s_head;
	char send_buf[len + 20];
	int send_len;

	s_head.type = _sound_talk_data_;
	s_head.num = sound_talk_cnt++;

	send_len = 0;
	memcpy(send_buf, &s_head, sizeof(sound_head_t));
	memcpy(send_buf + sizeof(sound_head_t), buf, len);

	send_len = sizeof(sound_head_t) + len;

	sendto_node_unreliable(name, _proto_c_audio_, send_buf, send_len);
}


int recv_sound_talk(listen_t *listen, char *recv_buf, int len)
{
	int ret;
//	char buf[512];

	struct sockaddr_in clientaddr;

	ret = recv_from_listen(listen, (struct sockaddr_in *)&clientaddr, recv_buf, len, 300);

	if(ret == -1)
	{
		printf("%p recv is err \r\n", listen);
		return -1;		//时间到了退出
	}

	return ret;
}


void send_sound_talk_end(char *name)
{
	sound_head_t s_head;

	s_head.type = _sound_talk_stop_;

	sendto_node_reliable(name, _proto_c_audio_, (char *)&s_head, sizeof(sound_head_t), 20);
}





/*-----------------------------------------------------------*/
/*
				音频文件传输底层函数
*/

void sound_file_request(char *name)
{
	sound_head_t s_head;

	s_head.type = _sound_file_request_;

	sendto_node_reliable(name, _proto_c_audio_, (char *)&s_head, sizeof(sound_head_t), 20);
}

/* 发送应答 */
void send_sound_file_ack(char *name, int ack)
{
	sound_head_t s_head;

	if(ack == 0)
		s_head.type = _sound_file_ack_;
	else
		s_head.type = _sound_file_noack_;
	
	
	sendto_node_reliable(name, _proto_c_audio_, (char *)&s_head, sizeof(sound_head_t), 20);
}



int recv_sound_file_ack(listen_t *listen)
{
	int ret;
	char buf[512];
	struct sockaddr_in clientaddr;

	ret = recv_from_listen(listen, (struct sockaddr_in *)&clientaddr, buf, 1204, 10);

	if(ret == -1)
	{
		printf("%p recv is err \r\n", listen);
		return -1;		//时间到了退出
	}

	sound_head_t *s_head = (sound_head_t *)(buf + sizeof(struct check_head) + sizeof(struct proto_c_send_data));

	/* 对方是否准备好了 */
	if(s_head->type == _sound_file_ack_)
	{
		return 0;
	}else{
		return -1;
	}
}



int sound_file_cnt;
int send_sound_file(char *name, char *buf, int len)
{
	sound_head_t s_head;
	char send_buf[len + 20];
	int send_len;

	s_head.type = _sound_file_data_;
	s_head.num = sound_file_cnt++;

	send_len = 0;
	memcpy(send_buf, &s_head, sizeof(sound_head_t));
	memcpy(send_buf + sizeof(sound_head_t), buf, len);

	send_len = sizeof(sound_head_t) + len;

	sendto_node_unreliable(name, _proto_c_audio_, send_buf, send_len);
}


int recv_sound_file(listen_t *listen, char *recv_buf, int len)
{
	int ret;
//	char buf[512];

	struct sockaddr_in clientaddr;

	ret = recv_from_listen(listen, (struct sockaddr_in *)&clientaddr, recv_buf, len, 300);

	if(ret == -1)
	{
		printf("%p recv is err \r\n", listen);
		return -1;		//时间到了退出
	}

	return ret;
}


void send_sound_file_end(char *name)
{
	sound_head_t s_head;

	s_head.type = _sound_file_end_;

	sendto_node_reliable(name, _proto_c_audio_, (char *)&s_head, sizeof(sound_head_t), 20);
}

