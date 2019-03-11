
/*******************************************************
	个人实现定时任务
********************************************************/

#include "all.h"
#include "timer_list.h"

LIST_HEAD(timer_list);		//定义并初始化一个链表头

/* 需加上互斥锁 */
static pthread_mutex_t timer_lock;// = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutexattr_t attr; 



int add_my_timer(struct my_timer *timer)
{
	struct my_timer *new_timer;
	new_timer = (struct my_timer *)malloc(sizeof(struct my_timer));
	if(new_timer == NULL)
		return -1;

	memcpy(new_timer, timer, sizeof(struct my_timer));

	pthread_mutex_lock(&timer_lock);
	list_add(&(new_timer->list), &timer_list);
	pthread_mutex_unlock(&timer_lock);
}


void my_timer_pthread(void *pdata)
{
	while(1)
	{
		pthread_mutex_lock(&timer_lock);
	
		struct my_timer *pos,*n;
		/* 比较下应答包是否是一样的 */
		list_for_each_entry_safe(pos,n,&timer_list, list, struct my_timer)
		{
			if(pos->sec > 1)
			{
				pos->sec --;
			}else{
				pos->handle(pos->arg);
				//删除
				list_del(&(pos->list));
				free(pos);
			}
		}

		pthread_mutex_unlock(&timer_lock);

		sleep(1);
	}
}


static void debug_my_timer(void);

void my_timer_init(void)
{
	int ret;  
    if(( ret = pthread_mutexattr_init(&attr)) != 0){  
        fprintf(stderr, "create mutex attribute error. msg:%s", strerror(ret));  
        exit(1);  
    }  
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);  
    pthread_mutex_init(&timer_lock, &attr);


	pthread_t recv_thread;
    pthread_attr_t recv_thread_attr;

	/* 定时器链表 */
	pthread_attr_init(&recv_thread_attr);		//初始化进程属性
	pthread_attr_setdetachstate(&recv_thread_attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&recv_thread, &recv_thread_attr, my_timer_pthread, NULL) < 0)
	{
		perror("pthread_create");
	}

//	debug_my_timer();
}


static void debug_my_timer_handle(void *pdata)
{
	printf("hello \r\n");

	time_t timep;
	time (&timep);
	printf("%s \r\n",asctime(gmtime(&timep)));
	
	struct my_timer timer;
	timer.sec = 2;
	timer.handle = debug_my_timer_handle;
	add_my_timer(&timer);
}

static void debug_my_timer(void)
{
	struct my_timer timer;
	timer.sec = 2;
	timer.handle = debug_my_timer_handle;
	add_my_timer(&timer);
}

