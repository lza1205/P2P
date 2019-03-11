

#ifndef __SOUND_PROTO_H__
#define __SOUND_PROTO_H__


/*---------------------------------------------------------*/
/* 协议 */
#define _sound_talk_request_		0x0		/* 请求传输*/
#define _sound_talk_ack_			0x1		/* 接听 */
#define _sound_talk_noack_			0x2		/* 拒绝通话 */
#define _sound_talk_stop_			0x3		/* 停止通话 */
#define _sound_talk_data_			0x4	/* 数据包 */


#define _sound_file_request_		0x100	/* 文件请求 */
#define _sound_file_ack_			0x101		/* 接听 */
#define _sound_file_noack_			0x102		/* 拒绝通话 */
#define _sound_file_end_			0x103	/* 文件结束 */
#define _sound_file_data_			0x104	/* 文件数据 */




typedef struct sound_head{
	unsigned int type;		/* 数据包类型 */			
	unsigned int num;		/* 包序号 */
}sound_head_t;



#endif

