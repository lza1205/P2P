

/********************************************************
音频传输的listen 处理
********************************************************/

#include "all.h"
#include "client.h"


void *video_listen_phread(void *pdata)
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

	video_head_t *s_head = (video_head_t *)(buf + sizeof(struct check_head) + sizeof(struct proto_c_send_data));


	printf("s_head->type %d  \r\n", s_head->type);

	if(s_head->type == _video_talk_request_)
	{
		video_talk_phread(listen);
	}else{
		video_file_phread(listen);
	}
}




/*
处理此次 listen
*/
void video_listen(listen_t *listen)
{
	printf("video_listen \r\n");
	listen_pthread(listen, video_listen_phread);
}


