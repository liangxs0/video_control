
//主函数
#include"commond.h"
#include "dev.h"
#include "print.h"
#include "net_sever.h"
#include "camera.h"



void str_err(const char* name){
	fprintf(stderr,"%s:%s\n",name,strerror(errno));
	exit(1);
}
int main(int argc,char** argv){
	if(argc < 3){
		fprintf(stderr,"usage :%s [dev] [port]\n",argv[0]);
		exit(0);
	}
	camera(&argv[1],&argv[2]);
		return 0;
}


