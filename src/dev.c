//摄像头驱动和开启实现
#include"dev.h"

static void suc_err(int res,char* name){			//出错信息整理
	if(res < 0){
		fprintf(stderr,"%s:%s\n",name,strerror(errno));
		exit(1);
	}
}

static void init_fmt(){								//初始化视频格式
	struct v4l2_format fmt;
	memset(&fmt,0,sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;			//数据流的类型
	fmt.fmt.pix.width = 300;						//图像的宽度
	fmt.fmt.pix.height = 300;						//图像的高度
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;	//彩色空间
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
	int res = ioctl(camera_fd,VIDIOC_S_FMT,&fmt);
	suc_err(res,"format");
}

static void init_mmap(){								//初始化内存映射
	int res;
	struct v4l2_requestbuffers req;						//请求缓存
	memset(&req,0,sizeof(req));
	req.count = 4;//缓存数量
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	res = ioctl(camera_fd,VIDIOC_REQBUFS,&req);
	suc_err(res,"Req_bufs");

	buffer = calloc(req.count,sizeof(Videobuf));			//为buffer分配内存空间（四个）
	struct v4l2_buffer buf;
	for(bufs_num = 0;bufs_num < req.count;bufs_num++)
	{
		memset(&buf,0,sizeof(buf));
		buf.index = bufs_num;							//设置缓存索引号
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.field = V4L2_FIELD_INTERLACED;
		buf.memory = V4L2_MEMORY_MMAP;
		//读取缓存信息
		res = ioctl(camera_fd,VIDIOC_QUERYBUF,&buf);
		suc_err(res,"Query_buf");
		//设置缓存大小
		buffer[bufs_num].length = buf.length;
		//在堆空间中动态分配二级缓存空间
		tmp_buf = (char*)calloc(buffer[okindex].length,sizeof(char));
		//将设备文件的地址映射到用户空间的物理地址
		buffer[bufs_num].start = mmap(NULL,buf.length, PROT_READ|PROT_WRITE,
				MAP_SHARED,camera_fd,buf.m.offset);
		if(buffer[bufs_num].start == MAP_FAILED)
			suc_err(-1,"Mmap");
		else
			suc_err(0,"Mmap");
	}
}

int init_dev(void){
	//初始化视频格式
	init_fmt();
	//初始化内存映射
	init_mmap();
}

int uninit(void){
	int i = 0;
	int res;
	for(i = 0;i<bufs_num;i++){
		res = munmap(buffer[i].start,buffer[i].length);
		suc_err(res,"mummap");
	}
	free(buffer);				//释放掉buffer分配的内存
	free(tmp_buf);
	close(camera_fd);			//关闭摄像头文件描述符
}

int get_dev_info(void){
	//获取当前设备的属性
	struct v4l2_capability cap;
	int res = ioctl(camera_fd,VIDIOC_QUERYCAP,&cap);
	suc_err(res,"get cap");
	//获取当前设备的输出格式
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	res = ioctl(camera_fd,VIDIOC_G_FMT,&fmt);
	suc_err(res,"get fmt");

	//获取当前设备的帧率
	struct v4l2_streamparm parm;
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	res = ioctl(camera_fd,VIDIOC_G_PARM,&parm);
	suc_err(res,"get parm");

	//打印输出设备信息：
	printf("----------------dev_info---------------\n");
	printf("driver:	%s\n",cap.driver);
	printf("card:	%s\n",cap.card);//摄像头的设备名
	printf("bus:	%s\n",cap.bus_info);
	printf("width:	%d\n",fmt.fmt.pix.width);//当前的图像输出宽度
	printf("height:	%d\n",fmt.fmt.pix.height);//当前的图像输出高度
	printf("FPS:	%d\n",parm.parm.capture.timeperframe.denominator);
	printf("------------------end------------------\n");

}

int cam_on(void){	
	int i;
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int res = ioctl(camera_fd,VIDIOC_STREAMON,&type);
	on_off = 1;
	suc_err(res,"open stream");
	//进行一次缓存的刷新
	struct v4l2_buffer buf;
	for(i = 0;i < bufs_num;i++)
	{
		memset(&buf,0,sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		res = ioctl(camera_fd,VIDIOC_QBUF,&buf);
		suc_err(res,"Q_buf_init");
	}
}

int cam_off(void){
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int res = ioctl(camera_fd,VIDIOC_STREAMOFF,&type);
	on_off = 0;
	suc_err(res,"close stream");
}

int get_frame(void){
	struct v4l2_buffer buf;
	int i = 0,res;
	counter++;
	memset(&buf,0,sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	fd_set	readfds;
	FD_ZERO(&readfds);				//文件描述符集清空
	FD_SET(camera_fd,&readfds);		//加入到文件描述符集中
	struct timeval tv;				//设置设备响应时间
	tv.tv_sec = 1;					//秒
	tv.tv_usec = 0;					//微秒
	while(select(camera_fd + 1,&readfds,NULL,NULL,&tv) <= 0){	
		//等待文件描述符准备好（延时1秒）
		fprintf(stderr,"camera busy,Dq_buf time out\n");
		FD_ZERO(&readfds);
		FD_SET(camera_fd,&readfds);
		tv.tv_sec = 1;
		tv.tv_usec = 0;
	}
	res = ioctl(camera_fd,VIDIOC_DQBUF,&buf);
	suc_err(res,"Dq_buf");

	//buf.index表示已经刷新好的可用的缓存索引号
	okindex = buf.index;
	//更新缓存已用大小
	buffer[okindex].length = buf.bytesused;
	//第n次捕获图片:(第n回刷新整个缓存队列-第n个缓存被刷新)
	////printf("Image_%03d:(%d-%d)\n",counter,counter / bufs_num,okindex);

	//把图像放入缓存队列中(入列)
	res = ioctl(camera_fd,VIDIOC_QBUF,&buf);
	suc_err(res,"Q_buf");

	// int fd = open("a.jpg",O_RDWR|O_CREAT|O_TRUNC,0644);
	// suc_err(fd,"open");
	// print_picture(fd,buffer[okindex].start,buffer[okindex].length);
	
	return 0;
}


