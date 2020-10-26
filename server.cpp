#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

#define MAXLINE 4096

//<关于errno.h>
//
//1、C标准库的errno.h 头文件定义了整数变量 errno，它是通过系统调用设置的，在错误事件中的某些库函数表明了什么发生了错误。该宏扩展为类型为 int 的可更改的左值，因此它可以被一个程序读取和修改。
//2、在程序启动时，errno 设置为零，C 标准库中的特定函数修改它的值为一些非零值以表示某些类型的错误。您也可以在适当的时候修改它的值或重置为零。
//3、errno.h 头文件定义了一系列表示不同错误代码的宏，这些宏应扩展为类型为 int 的整数常量表达式。
//
//<关于sys/types.h>
//
//1、中文名称为基本系统数据类型，此头文件还包含适当时应使用的多个基本派生类型。去掉这个头文件后，其他很多头文件中的符号，以及本代码中的很多符号就无法使用了
//
//<关于sys/socket.h>
//
//1、socket编程在linux中主要需要包含的几个头文件：sys/socket.h,netinet/in.h,netdb.h,arpa/inet.h；而在windows中则是几乎仅包括winsock.h
//2、补充，fd指的是file description，是文件描述符，在windows中叫做句柄
//3、这个头文件就是用来进行socket编程的
//
//<关于netinet/in.h和arpa/inet.h>
//
//1、<netinet/in.h>主要定义了一些类型，<arpa/inet.h>主要定义了格式转换函数。arpa目录还有ftp.h, nameser.h, nameser_compat.h, telent.h, tftp.h。netinet目录还有ether.h, icmp6.h, if_ether.h, if_fddr.h, if_tr.h, igmp.h, in_systm.h, ip.h, ip6.h, ip_icmp.h, tcp.h, udp.h。arpa网是因特网的前身。
//2、netinet与arpa这两个目录下的头文件，一般来说，arpa下的都是在应用层的，netinet下的则是传输层，网络层，网络接口层的协议。
//3、想要深入理解的话，需要去/usr/include/下寻找这两个头文件，看一下它们内部是什么
//
//<关于unistd.h>
//
//1、unistd.h是unix std的意思，是POSIX标准定义的unix类系统定义符号常量的头文件，包含了许多UNIX系统服务的函数原型，unistd.h在unix中类似于window中的windows.h


int main(int argc,char** argv)
//  argc是命令行总的参数个数,argv[]/*argv是argc个参数，其中第0个参数是程序的全名，以后的参数命令行后面跟的用户输入的参数
{
	int listenfd,connfd;
	struct sockaddr_in servaddr;
//struct sockaddr和struct sockaddr_in这两个结构体用来处理网络通信的地址。
//sockaddr的缺陷是：sa_data把目标地址和端口信息混在一起了
//struct sockaddr {
//	sa_family_t sin_family;//地址族
//	char sa_data[14]; //14字节，包含套接字中的目标地址和端口信息
//};
//sockaddr_in 结构体：struct sockaddr_in中的in 表示internet，就是网络地址，这只是我们比较常用的地址的结构，属于AF_INET地址族，他非常的常用sockaddr_in结构体解决了sockaddr的缺陷，把port和addr分开储存在两个变量中
//struct sockaddr_in {
//	short int sin_family;
//　　	unsigned short int sin_port;
//     	struct in_addr sin_addr;
//	struct in_addr {
//   		unsigned long s_addr;
//        }
//    	unsigned char sin_zero[8];
//}
//
//sin_port和sin_addr都必须是NBO,一般可视化的数字都是HBO（本机字节顺序)
//
//网络字节顺序NBO（Network Byte Order）：
//	按从高到低的顺序存储，在网络上使用统一的网络字节顺序，可以避免兼容性问题。
//主机字节顺序（HBO，Host Byte Order）：
//	不同的机器HBO不相同，与CPU设计有关，数据的顺序是由cpu决定的,而与操作系统无关。 如Intelx86结构下,short型数0x1234表示为34 12, int型数0x12345678表示为78 56 34 12如IBM power PC结构下,short型数0x1234表示为12 34, int型数0x12345678表示为12 34 56 78

//     	htonl()--"Host to Network Long"
//      ntohl()--"Network to Host Long"
//      htons()--"Host to Network Short"
//      ntohs()--"Network to Host Short"
//
//由于这个原因不同体系结构的机器之间无法通信,所以要转换成一种约定的数序,也就是网络字节顺序,其实就是如同powerpc那样的顺序 。在PC开发中有ntohl和htonl函数可以用来进行网络字节和主机字节的转换。 
//注意，数字所占位数小于或等于一个字节（8 bits）时，不要用htons转换。这是因为对于主机来说，大小尾端的最小单位为字节(byte)。

//sin_zero 初始值应该使用函数 bzero() 来全部置零。一般采用下面语句
//	struct sockaddr_in cliaddr;
//	bzero(&cliaddr,sizeof(cliaddr)); 
//	或者
//	memset(...)
//sockaddr和sockaddr_in的相互关系:
//1、一般先把sockaddr_in变量赋值后，强制类型转换后传入用sockaddr做参数的函数。
//2、sockaddr_in用于socket定义和赋值，sockaddr用于函数参数

	char buff[4096];
	int n;
	
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1)

//在 Linux 下使用 <sys/socket.h> 头文件中 socket() 函数来创建套接字，原型为：
//int socket(int af, int type, int protocol);
//
//1、af为地址族（Address Family），也就是 IP 地址类型，常用的有 AF_INET 和 AF_INET6。AF 是“Address Family”的简写，INET是“Inetnet”的简写。AF_INET 表示 IPv4 地址，例如 127.0.0.1；AF_INET6 表示 IPv6 地址，例如 1030::C9B4:FF12:48AA:1A2B。
//127.0.0.1是一个特殊IP地址，表示本机地址。
//也可以使用PF前缀，PF 是“Protocol Family”的简写，它和AF是一样的。例如，PF_INET等价于AF_INET，PF_INET6 等价于AF_INET6。
//2、type 为数据传输方式/套接字类型，常用的有 SOCK_STREAM（流格式套接字/面向连接的套接字 TCP）和SOCK_DGRAM（数据报套接字/无连接的套接字 UDP）。
//3、protocol表示传输协议，常用的有IPPROTO_TCP和IPPTOTO_UDP，分别表示TCP传输协议和UDP传输协议
//
//疑问：有了地址类型和数据传输方式，还不足以决定采用哪种协议吗？为什么还需要第三个参数呢？
//回答：一般情况下有了af和type两个参数就可以创建套接字了，操作系统会自动推演出协议类型，除非遇到这样的情况：有两种不同的协议支持同一种地址类型和数据传输类型。如果我们不指明使用哪种协议，操作系统是没办法自动推演的。当使用 IPv4 地址时，参数af的值为PF_INET。如果使用SOCK_STREAM传输数据，那么满足这两个条件的协议只有TCP，因此可以这样来调用socket()函数：
//
//int tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  //IPPROTO_TCP表示TCP协议
//这种套接字称为 TCP 套接字。
//如果使用 SOCK_DGRAM 传输方式，那么满足这两个条件的协议只有 UDP，因此可以这样来调用 socket() 函数：
//int udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  //IPPROTO_UDP表示UDP协议
//这种套接字称为 UDP 套接字。
//
//上面两种情况都只有一种协议满足条件，可以将 protocol 的值设为 0，系统会自动推演出应该使用什么协议，如下所示：
//
//int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);  //创建TCP套接字
//int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);  //创建UDP套接字
//

	{
		printf("create socket error: %s(error: %d)\n",strerror(errno),errno);
		return 0;
	}
//C库函数char *strerror(int errnum)从内部数组中搜索错误号errnum，并返回一个指向错误消息字符串的指针。strerror生成的错误字符串取决于开发平台和编译器。

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(6666);

	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))==-1)
	{
		printf("bing socket error: %s(errno: %d)\n",strerror(errno),errno);
		return 0;
	}

//TCP交互流程为
//
//	服务器：
//	socket() 创建socket
//	bind() 绑定socket和端口号
//	listen() 监听端口号 监听客户端connect()的请求
//	accept() 接收来自客户端的连接请求 接受客户端connect()的请求
//	recv() 从socket中读取字符 读取客户端send()的请求
//	close() 关闭socket
//
//	客户端：
//	socket() 创建socket
//	connect() 连接指定计算机的端口
//	send() 向socket中写入信息
//	close() 关闭socket

//bind函数原型：
//int bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen);
//第1个参数sockfd是用socket()函数创建的文件描述符。
//第2个参数my_addr是指向一个结构为sockaddr参数的指针，sockaddr中包含了地址、端口和IP地址的信息。在进行地址绑定的时候，需要弦将地址结构中的IP地址、端口、类型等结构struct sockaddr中的域进行设置之后才能进行绑定，这样进行绑定后才能将套接字文件描述符与地址等接合在一起。
//第3个参数addrlen是my_addr结构的长度，可以设置成sizeof(struct sockaddr)。使用sizeof(struct sockaddr)来设置套接字的类型和其对已ing的结构。
//bind()函数的返回值为0时表示绑定成功，-1表示绑定失败，errno的错误值如表1所示。

	if(listen(listenfd,10)==-1)
	{
		printf("listen socket error: %s(error: %d)\n",strerror(errno),errno);
		return 0;	
	}

//listen函数原型：
//通过 listen() 函数可以让套接字进入被动监听状态，它的原型为：
//int listen(int sock, int backlog);
//int listen(SOCKET sock, int backlog);
//sock为需要进入监听状态的套接字，backlog为请求队列的最大长度
//所谓被动监听，是指当没有客户端请求时，套接字处于“睡眠”状态，只有当接收到客户端请求时，套接字才会被“唤醒”来响应请求。

	printf("===waiting for client's request===\n");
	while(1)
	{
		if((connfd=accept(listenfd,(struct sockaddr*)NULL,NULL))==-1)
		{
			printf("accept socket error:%s(error %d)",strerror(errno),errno);
			continue;
		}
		n=recv(connfd,buff,MAXLINE,0);

//sock为需要进入监听状态的套接字，backlog 为请求队列的最大长度。
//所谓被动监听，是指当没有客户端请求时，套接字处于“睡眠”状态，只有当接收到客户端请求时，套接字才会被“唤醒”来响应请求。	

		buff[n]='\0';
		printf("recv msg from client: %s\n",buff);
		close(connfd);
	}
	close(listenfd);
	return 0;
}
