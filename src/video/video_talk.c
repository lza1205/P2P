
/*******************************************************
语音通话
********************************************************/

#include "all.h"
#include "client.h"


int video_talk_end_flg;

extern int frames_size;

void *video_talk_send_phread(void *pdata)
{
	//char buff[3760];
	listen_t *listen;
	listen = (struct listen *)pdata;
	
	char *buff;

	video_talk_end_flg = 0;
	/* 录音初始化 */
	alsa_capture_init();

	
	buff = malloc(frames_size);
	if(buff == NULL)
		goto out1 ;

	printf("You can speak \r\n");
	while(1)
	{
		if(alsa_capture(buff) > 0)
		{
			send_video_talk(listen->name, buff, frames_size);
			printf(".......\r\n");
		}else{
			printf("_______\r\n");
		}
		
		/* 发送结束 */
		if(video_talk_end_flg == 1)
		{
			send_video_talk_end(listen->name);
			printf("send_video_talk_end \r\n");
			break;
		}
	}

	free(buff);
	
out1:
	alsa_capture_close();

}

/*
通话
*/
void video_talk(listen_t *listen)
{
	/* 启动一个录音发送函数 */
	listen_pthread(listen, video_talk_send_phread);
	
	char buf[4096];
	video_head_t *s_head;

	int ret;

	alsa_playback_init();

	while(1)
	{
		ret = recv_video_talk(listen, buf, sizeof(buf));
		if(ret == -1)
		{
			printf("%p recv is err \r\n", listen);
			return ;		//时间到了退出
		}

		/* 发送结束 */
		if(video_talk_end_flg == 1)
		{
			alsa_playback_close();
			return ;
		}
		
		s_head = (video_head_t *)(buf + sizeof(struct check_head) + sizeof(struct proto_c_send_data));

		/* 对方是否准备好了 */
		switch(s_head->type)
		{
			case _video_talk_data_:
			{
				alsa_playback(buf + sizeof(struct check_head) + sizeof(struct proto_c_send_data) + sizeof(video_head_t));
				break;
			}
			case _video_talk_stop_:
			{
				printf("recv _video_talk_stop_ ! \r\n");
				video_talk_end_flg == 1;
				
				alsa_playback_close();
				return ;
			}
			printf("??????????????????? \r\n");
		}
	}

}



/*----------------------------------------------------------------*/
/*
启动一个线程来处理此次通话
*/
void *video_talk_phread(listen_t *listen)
{
	int ret;
	char buf[1204];
	struct sockaddr_in clientaddr;

	ret = 0;
	/* 调用回调函数 */
	if(client_inface.listen_audio != NULL)
		client_inface.listen_audio(listen);

	
	/* 回复对方拒绝通话 */
	if(ret != 0)
	{
		send_video_talk_ack(listen->name, 1);
		listen_close(listen);
		return 0;
	}

	/* 回复对方。可以开始通话了 */
	send_video_talk_ack(listen->name, 0);	

	/* 正式通话 */
	video_talk(listen);

	listen_close(listen);
	return 0;
}




/*---------------------------------------------------------*/
/*
主动发起语音通话请求
*/



/*
启动一个线程来处理此次通话
*/
void *video_talk_to_phread(void *pdata)
{
	int ret;
	struct sockaddr_in clientaddr;
	listen_t *listen;
	listen = (struct listen *)pdata;

	/* 开启这个listen  */
	listen_start(listen);

	/* 发送会话请求 */
	video_talk_request(listen->name);

	/* 等待对方确认 */
	/* 等待对方确认 */
	if(recv_video_talk_ack(listen) != 0)
	{
		printf("recv_video_file_noack \r\n");
		goto out1;
	}

	/* 正式通话 */
	video_talk(listen);

out1:

	listen_close(listen);
	return 0;
}



void video_talk_to(char *name)
{
	listen_t *listen;
	listen = create_listen(name, _proto_c_audio_);
	listen_pthread(listen, video_talk_to_phread);
}



