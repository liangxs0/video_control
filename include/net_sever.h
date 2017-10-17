//服务器搭建
#ifndef _NET_SEVER_H_
#define _NET_SEVER_H_

#include "commond.h"
#include "dev.h"
#include "print.h"
#include "camera.h"
#include "client.h"

extern int cilent_num;
extern pthread_mutex_t mutex;//互斥锁
extern pthread_cond_t cond;//条件变量
extern pthread_attr_t attr;//属性

//服务端给客户
void do_sever(int fd);
int net_sever(char** potr);
// int net_sever(void);
#endif