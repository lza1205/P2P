
/*******************************************************
音频文件传输
*******************************************************/
#include "all.h"
#include "client.h"


void video_file(listen_t *listen)
{	
	char buf[4096];
	video_head_t *s_head;

	int ret;

	printf("fopen \r\n");

	FILE *fp = fopen("video_file", "w+");
	if(fp == NULL)
	{
		perror("fopen ");
		return ;
	}
	
	printf("fopen \r\n");
	while(1)
	{
		ret = recv_video_file(listen, buf, sizeof(buf));
		if(ret == -1)
		{
			printf("%p recv is err \r\n", listen);
			return ;		//时间到了退出
		}

		s_head = (video_head_t *)(buf + sizeof(struct check_head) + sizeof(struct proto_c_send_data));

		/* 对方是否准备好了 */
		switch(s_head->type)
		{
			case _video_file_data_:
			{
				fwrite(buf + sizeof(struct check_head) + sizeof(struct proto_c_send_data) + sizeof(video_head_t), 
						ret - (sizeof(struct check_head) + sizeof(struct proto_c_send_data) + sizeof(video_head_t)), 1, fp);
				break;
			}
			case _video_file_end_:
			{
				printf("recv new video file ! \r\n");
				fclose(fp);
				return ;
			}
			printf("??????????????????? \r\n");
		}
	}
}


/* 启动一个线程来接收音频文件 */
void *video_file_phread(listen_t *listen)
{
	int ret;
	char buf[1204];
	struct sockaddr_in clientaddr;

	ret = 0;
	/* 调用回调函数 */
	if(client_inface.listen_audio != NULL)
		client_inface.listen_audio(listen);

	printf("send_video_file_ack \r\n");
	/* 回复对方拒绝通话 */
	if(ret != 0)
	{
		send_video_file_ack(listen->name, 1);
		listen_close(listen);
		return 0;
	}

	/* 回复对方。可以开始通话了 */
	send_video_file_ack(listen->name, 0);

	printf("video_file \r\n");

	/* 正式接收音频文件 */
	video_file(listen);

	listen_close(listen);
	return 0;
}



/*---------------------------------------------------------*/
/*
主动发起音频文件传输请求
*/


int video_file_end_flg;

extern int frames_size;
/*
从lasa 中获取音频数据并发送出去
*/
void video_file_send(listen_t *listen)
{
	//char buff[3760];
//	char buff[340];
	char *buff;


	/* 录音初始化 */
	alsa_capture_init();

	buff = malloc(frames_size);
	if(buff == NULL)
		goto out1 ;
	
	video_file_end_flg = 0;

	printf("You can speak \r\n");
	while(1)
	{
		if(alsa_capture(buff) > 0)
			send_video_file(listen->name, buff, frames_size);

		/* 发送结束 */
		if(video_file_end_flg == 1)
		{
			send_video_file_end(listen->name);
			printf("send_video_file_end \r\n");
			break;
		}
	}

	free(buff);
out1:
	alsa_capture_close();
}

/*
启动一个线程来处理此次通话
*/
void *video_file_to_phread(void *pdata)
{
	int ret;
	char buf[1204];
	struct sockaddr_in clientaddr;
	listen_t *listen;
	listen = (struct listen *)pdata;

	/* 开启这个listen  */
	listen_start(listen);

	printf("video_file_request \r\n");

	/* 发送会话请求 */
	video_file_request(listen->name);

	/* 等待对方确认 */
	if(recv_video_file_ack(listen) != 0)
	{
		printf("recv_video_file_noack \r\n");
		goto out1;
	}

	printf("video_file_send \r\n");
	
	/* 正式发送音频文件 */
	video_file_send(listen);

out1:
	listen_close(listen);
	return 0;
}



void video_file_to(char *name)
{
	listen_t *listen;
	listen = create_listen(name, _proto_c_audio_);
	if(listen == NULL)
		printf("NULL !\r\n");
	listen_pthread(listen, video_file_to_phread);
}




/*---------------------------------------------------------*/
/*
			播放音频文件
*/

void play_video_file(char *fname)
{
//	char buf[3760];
//	char buf[340];
	char *buf;	
	int ret;
	
	FILE *fp = fopen(fname, "r+");

	if(fp == NULL){
		perror("fopen ");
		return ;
	}
	
	alsa_playback_init();
	
	buf = malloc(frames_size);
	if(buf == NULL)
		goto out1 ;

	printf("frames_size %d \r\n", frames_size);

	while((ret = fread(buf, frames_size, 1, fp)) > 0)
	{
		alsa_playback(buf);
	}
	
	free(buf);
	
out1:
	alsa_playback_close();

	fclose(fp);
}

