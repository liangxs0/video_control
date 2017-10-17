//客户端检测部分
//每次检测到新的客户访问时，开启新的线程服务
#ifndef _CLIENTF_H_
#define _CLIENTF_H_
#include "commond.h"
#include "net_sever.h"

int cilent_num;
pthread_mutex_t mutex;//互斥锁
pthread_cond_t cond;//条件变量
pthread_attr_t attr;//属性

extern void pthread_init(void);
extern void create(int fd);
extern void* do_forma(void* arg);
extern void cilent_out(int fd);

#endif