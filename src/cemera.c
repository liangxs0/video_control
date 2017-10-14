//摄像头函数接口
#include "camera.h"

void camera(char** arg)
{
	//打开摄像头文件
	camera_fd = open(*arg,O_RDWR|O_NONBLOCK);
	//初始化驱动的设备s
	init_dev();
	//打开摄像头
	cam_on();
	//打印设备信息
	get_dev_info();
	//获取一帧的数据
	get_frame();
	//关闭摄像头
	cam_off();
	//卸载设备
	uninit();
}