
#include "net_sever.h"

void init_web(int fd)
{
	char buff[4096];
	sprintf(buff,"HTTP/1.0 200 OK\r\n"
				"Connection: Keep-Alive\r\n"
				"Server: Network camera\r\n"
				"Cache-Control: no-cache,no-store,must-revalidate,pre-check=0,max-age=0\r\n"
				"Pragma: no-cache\r\n"
				"Content-Type: multipart/x-mixed-replace;boundary=KK\r\n"
				);

	if(write(fd,buff,strlen(buff)) != strlen(buff)){
		fprintf(stderr, "write HTTP Error%s\n", strerror(errno));
		return ;
	}
}	
//消息报头
void init_Whead(int fd,ssize_t size)
{
	char buff[4096];
	sprintf(buff,"\r\n--KK\r\n"
		"Content-Type: image/jpeg\n"
		"Content-Length: %lu\n\n", size);
	if(write(fd,buff,strlen(buff)) != strlen(buff)){
		fprintf(stderr, "write head error:%s\n",strerror(errno));
		return;
	}
}

//服务函数
void do_sever(int fd)
{
	//传输获得的图片信息
	init_web(fd);
	// pi[0] = fd;
	while(1){
		init_Whead(fd,buffer[okindex].length);
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond,&mutex);
		print_picture(fd,tmp_buf,buffer[okindex].length);
		pthread_mutex_unlock(&mutex);
	}
}

int net_sever(char** port)
// int net_sever()
{

	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		fprintf(stderr, "socket error:%s\n",strerror(errno));
		exit(1);
	}
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(*port));
	saddr.sin_addr.s_addr = INADDR_ANY;
	bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	//初始化线程
	pthread_init();
	//开始监听
	listen(sockfd,0);

	pthread_t th;
	int err = pthread_create(&th,&attr,do_forma,NULL);
	if(err < 0){
		fprintf(stderr, "do_forma create error:%s\n",strerror(errno));
	}
	while(1){
		//服务端等待客户端开启
		int connfd = accept(sockfd,0,0);
		if(connfd < 0){
			printf("connfd < 0\n");
		}
		create(connfd);
	}
	close(sockfd);

	return 0;
}