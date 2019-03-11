
/*
P2P相关的函数
*/

/********************************************************
函数名:	p2p_thread_init
功能:		初始化同步线程
********************************************************/

void p2p_thread_init(void)
{
    pthread_t p2p_thread;
    pthread_attr_t p2p_thread_attr;

	/* 启动一个线程去尝试P2P 呼叫 */
	pthread_attr_init(&p2p_thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&p2p_thread_attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&p2p_thread, &p2p_thread_attr, p2p_thread_sync, NULL) < 0)
	{
		perror("pthread_create");
	}
}


