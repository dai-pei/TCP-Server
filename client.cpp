#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

#include<arpa/inet.h>

#define MAXLINE 4096

int main(int argc,char** argv)
{
	int sockfd,n;
	char recvline[4096],sendline[4096];
	struct sockaddr_in servaddr;

	if(argc!=2)
	{
		printf("usage:./clinet <ipaddress>\n");
		return 0;
	}

//argc是参数的个数，如果参数个数不等于2,说明ip地址未给出，因此显示提示信息

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
		return 0;
	}

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(6666);
	if(inet_pton(AF_INET,argv[1],&servaddr.sin_addr)<=0){
		printf("inet_pton error for %s\n",argv[1]);
		return 0;
	}

//windows下：
//#include <WS2tcpip.h>
//linux下：
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include<arpa/inet.h>
//函数原型：
//inet_pton：将“点分十进制” －> “二进制整数”
//int inet_pton(int af, const char *src, void *dst);
//这个函数转换字符串到网络地址，第一个参数af是地址簇，第二个参数*src是来源地址，第三个参数* dst接收转换后的数据。
//inet_pton 是inet_addr的扩展，支持的多地址族有下列：
//af = AF_INET
//src为指向字符型的地址，即ASCII的地址的首地址（ddd.ddd.ddd.ddd格式的），函数将该地址转换为in_addr的结构体，并复制在*dst中。
//af = AF_INET6
//src为指向IPV6的地址，函数将该地址转换为in6_addr的结构体，并复制在*dst中。
//如果函数出错将返回一个负值，并将errno设置为EAFNOSUPPORT，如果参数af指定的地址族和src格式不对，函数将返回0
//
//1.把ip地址转化为用于网络传输的二进制数值
//
//int inet_aton(const char *cp, struct in_addr *inp);
//inet_aton() 转换网络主机地址ip(如192.168.1.10)为二进制数值，并存储在struct in_addr结构中，即第二个参数*inp,函数返回非0表示cp主机有地有效，返回0表示主机地址无效。（这个转换完后不能用于网络传输，还需要调用htons或htonl函数才能将主机字节顺序转化为网络字节顺序）
//
//in_addr_t inet_addr(const char *cp);
//inet_addr函数转换网络主机地址（如192.168.1.10)为网络字节序二进制值，如果参数char *cp无效，函数返回-1(INADDR_NONE),这个函数在处理地址为255.255.255.255时也返回－1,255.255.255.255是一个有效的地址，不过inet_addr无法处理;
//
//2.将网络传输的二进制数值转化为成点分十进制的ip地址
//
//char *inet_ntoa(struct in_addr in);
//inet_ntoa 函数转换网络字节排序的地址为标准的ASCII以点分开的地址,该函数返回指向点分开的字符串地址（如192.168.1.10)的指针，该字符串的空间为静态分配的，这意味着在第二次调用该函数时，上一次调用将会被重写（复盖），所以如果需要保存该串最后复制出来自己管理！

	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0){
		printf("connect error: %s(error %d)\n",strerror(errno),errno);
                return 0;
	}

	printf("send msg to server:\n");
	fgets(sendline,4096,stdin);
	if(send(sockfd,sendline,strlen(sendline),0)<0){
		printf("send msg error: %s(error %d)",strerror(errno),errno);
		return 0;
	}
	close(sockfd);
	return 0;

}

