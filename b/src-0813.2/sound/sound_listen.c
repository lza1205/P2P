

/********************************************************
音频传输的listen 处理
********************************************************/

#include "all.h"
#include "client.h"


void *sound_listen_phread(void *pdata)
{
	int ret;
	char buf[1204];
	struct sockaddr_in clientaddr;
	listen_t *listen;
	listen = (struct listen *)pdata;

	/* 开启这个listen  */
	listen_start(listen);

	printf("recv_from_listen \r\n");

	ret = recv_from_listen(listen, (struct sockaddr_in *)&clientaddr, buf, 1204, 300);

	if(ret == -1)
	{
		printf("%p recv is err \r\n", listen);
		listen_close(listen);
		return;		//时间到了退出
	}

	sound_head_t *s_head = (sound_head_t *)(buf + sizeof(struct check_head) + sizeof(struct proto_c_send_data));


	printf("s_head->type %d  \r\n", s_head->type);

	if(s_head->type == _sound_talk_request_)
	{
		sound_talk_phread(listen);
	}else{
		sound_file_phread(listen);
	}
}




/*
处理此次 listen
*/
void sound_listen(listen_t *listen)
{
	printf("sound_listen \r\n");
	listen_pthread(listen, sound_listen_phread);
}


