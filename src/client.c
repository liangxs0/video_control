//客户端检测实现函数

#include "client.h"

void pthread_init()
{
	//线程设置初始化
	pthread_mutex_init(&mutex,NULL);
	pthread_attr_init(&attr);
	pthread_cond_init(&cond,NULL);
}
void *creat_pthread(void* arg)
{
	do_sever(*((int*)(arg)));//arg -> int arg
}
//开启客户端
void create(int fd)
{
	void *arg = &fd;
	pthread_t ntid;
	int err;
	cilent_num++;
	err = pthread_create(&ntid,NULL,creat_pthread,(void*)arg);
	printf("client_num = %d\n",cilent_num);
	if(err != 0){
		fprintf(stderr,"pthread_create error %s\n",strerror(errno));
		exit(1);
	}
}
void* do_forma(void* arg)
{
	while(1){
		if(cilent_num > 0){
			get_frame();
			pthread_mutex_lock(&mutex);
			memcpy(tmp_buf,buffer[okindex].start,buffer[okindex].length);
			pthread_cond_broadcast(&cond);
			pthread_mutex_unlock(&mutex);
		}
	}
}
//线程退出
void cilent_out(int fd)
{
	close(fd);
	pthread_exit(NULL);
	printf("退出\n");
	cilent_num--;
	printf("cilent_num = %d\n",cilent_num);
}
