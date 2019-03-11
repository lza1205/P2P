
/*******************************************************
语音通话
********************************************************/

#include "all.h"
#include "client.h"


/*
通话
*/
void sound_talk(listen_t *listen)
{
	/* 启动一个录音发送函数 */
	
	
	while(1)
	{
		
	}
}



/*----------------------------------------------------------------*/
/*
启动一个线程来处理此次通话
*/
void *sound_talk_phread(listen_t *listen)
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
		
		listen_close(listen);
		return 0;
	}

	/* 回复对方。可以开始通话了 */

	/* 正式通话 */
	sound_talk(listen);

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
void *sound_talk_to_phread(void *pdata)
{
	int ret;
	char buf[1204];
	struct sockaddr_in clientaddr;
	listen_t *listen;
	listen = (struct listen *)pdata;

	/* 开启这个listen  */
	listen_start(listen);

	/* 发送会话请求 */
	

	/* 等待对方确认 */

	/* 正式通话 */
	sound_talk(listen);

	listen_close(listen);
	return 0;
}



void sound_talk_to(char *name)
{
	listen_t *listen;
	listen = create_listen(name, _proto_c_audio_);
	listen_pthread(listen, sound_talk_to_phread);
}



