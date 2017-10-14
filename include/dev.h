#ifndef _DEV_H_
#define _DEV_H_
#include "print.h"
#include"commond.h"

int camera_fd;			//摄像头文件的文件描述符

typedef struct {
	void * start;		//映射后的物理首地址
	ssize_t length;		//存储映射的内存长度
}Videobuf;

Videobuf* buffer;		//存储映射的数组

int bufs_num;	//记录缓存的数量

int counter;	//记录当前捕获了多少张图像

int okindex;	//记录已经刷新好的缓存的索引号

char * tmp_buf;	//二级缓存地址

int on_off;		//标示摄像头是否打开

extern int init_dev(void);

extern int uninit(void);

extern int get_dev_info(void);

extern int cam_on(void);

extern int cam_off(void);

extern int get_frame(void);

#endif
