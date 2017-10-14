#include "net_sever.h"

//服务函数
void do_sever(int fd)
{
	//传输获得的图片信息
	int size = 0;
	int fd1 = open("a.jpg",O_RDWR);
	if(fd1 < 0){
		fprintf(stderr,"open picture error:%s\n",strerror(errno));
		exit(1);
	}
	struct stat st;
	stat("a.jpg",&st);
	size = st.st_size;
	printf("size0 = %d",size);
	char pic_buf[1024*1024*2];//2兆
	char buffer[4096];
	sprintf(buffer,"HTTP/1.0 200 OK\r\n"
				"Connection: Keep-Alive\r\n"
				"Server: Network camera\r\n"
				"Cache-Control: no-cache,no-store,must-revalidate,pre-check=0,max-age=0\r\n"
				"Pragma: no-cache\r\n"
				"Content-Type: multipart/x-mixed-replace;boundary=KK\r\n"
				);
	if(write(fd,buffer,strlen(buffer)) != strlen(buffer)){
		fprintf(stderr, "write HTTP Error%s\n", strerror(errno));
		return ;
	}
	//消息报头
	sprintf(buffer,"\r\n--KK\r\n"
		"Content-Type: image/jpeg\n"
		"Content-Length: %d\n\n", size);
	if(write(fd,buffer,strlen(buffer)) != strlen(buffer)){
		fprintf(stderr, "write head error:%s\n",strerror(errno));
		return;
	}
	if(write(fd,"aa",2) != 2){
		fprintf(stderr,"aa error:%s\n",strerror(errno));
	}
	memset(pic_buf,0,sizeof(pic_buf));
	while((size = read(fd1,pic_buf,1024*1024*2)) > 0){
		printf("size = %d\n",size);
		if(write(fd,pic_buf,sizeof(pic_buf)) != size){
			fprintf(stderr,"write error:%s\n",strerror(errno));
		}
	}
	if(size < 0){
		printf("read error\n");
	}
	
	close(fd);
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
	printf("saddr.sin_port:%d\n",saddr.sin_port);
	saddr.sin_addr.s_addr = INADDR_ANY;

	bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	listen(sockfd,0);
	while(1){
		int connfd = accept(sockfd,0,0);
		if(connfd < 0){
			printf("connfd < 0\n");
		}
		do_sever(connfd);
	}
	close(sockfd);

	return 0;
}