/**
  ****************************(C) COPYRIGHT 2021 Peng****************************
  * @file       main
  * @brief      协程测试
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Jan-1-2021      Peng            1. 完成
  *
  @verbatim
  ==============================================================================
  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2021 Peng****************************
  */
#include <iostream>
#include "./src/schedule.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
//socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
//read
#include <unistd.h>
//wrap.h
#include "./src/wrap.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>  // snprintf
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>

using namespace msocket;

// 协程逻辑测试
int cotest(){

	co_go(
		[]
		{
			std::cout << "coroutines 1 - 1 - 1" << std::endl;
			co_yield(); // 此处的yield会执行出错
			co_go(
				[]
				{
					std::cout << "coroutines 1 - 2 - 1" << std::endl;
					co_yield();
					std::cout << "coroutines 1 - 2 - 2" << std::endl;
				}
				);
			std::cout << "coroutines 1 - 1 - 2" << std::endl;
		}
	);

	co_go(
		[]
		{
			std::cout << "coroutines 2 - 1 - 1" << std::endl;
			co_go(
				[]
				{
					std::cout << "coroutines 2 - 2 - 1" << std::endl;
					co_yield();
					std::cout << "coroutines 2 - 2 - 2" << std::endl;
				}
				);
			std::cout << "coroutines 2 - 1 - 2" << std::endl;
		}
		);
	std::cout << "end" << std::endl;
}

//往socket中写数据
ssize_t msend(int _sockfd , const void* buf, size_t count)
{
	//忽略SIGPIPE信号
	size_t sendIdx = ::send(_sockfd, buf, count, MSG_NOSIGNAL);
	if (sendIdx >= count){
		return count;
	}
	co_yield();
	return msend( _sockfd , (char *)buf + sendIdx, count - sendIdx);
}

//从socket中读数据
ssize_t mread(int _sockfd , void* buf, size_t count)
{
	auto ret = ::read(_sockfd, buf, count);
	if (ret >= 0){
		return ret;
	}
	if(ret == -1 && errno == EINTR){
		return mread(_sockfd , buf, count);
	}
	std::cout << "##########yield" << std::endl;
	co_yield();
	return ::read(_sockfd, buf, count);
}

/*
第一次携程切换50%概率会重新执行前面的操作
正确用法：使用协程嵌套协程
*/

static int times = 0;
int test(){
	co_go( 
		[](){
			for(int i = 0; i<16 ; i++){
				std::cout << "start produce" << i << std::endl;
				co_go( 
					[i](){
						/************************************************************/
						std::cout << "++++++co" << i << "start." << std::endl;
						char buf[1024];
						int j = 1;
						while(j-->0){
							/* socket */
							char buf[4096]; 
							int cfd = 0;
							int ret = 0;
							struct sockaddr_in serv_addr; //服务器地址结构
							serv_addr.sin_family = AF_INET;
							serv_addr.sin_port = htons(7103); 
							inet_pton(AF_INET,"211.67.19.195",&serv_addr.sin_addr.s_addr);
							cfd = Socket(AF_INET,SOCK_STREAM,0); //创建socket
							ret = Connect(cfd ,(struct sockaddr *)&serv_addr ,sizeof(serv_addr) ); //连接服务器端
							
							// 设置非阻塞
							bool block = 1;
							setsockopt(cfd, IPPROTO_TCP, TCP_NODELAY,
									&block, static_cast<socklen_t>(sizeof block));

							//服务器地址结构
							char serv_ip[32];
							// std::cout << inet_ntop(AF_INET,&serv_addr.sin_addr.s_addr,serv_ip,sizeof(serv_ip)) << std::endl;

							msend(cfd , "ping" ,strlen("ping"));

							mread(cfd ,buf ,sizeof(buf));

							close(cfd);
						}
						std::cout << "------co" << i << "all done." << std::endl;
						std::cout << "times" << times << std::endl;
						times +=1 ;
						std::cout << "start run" << i << std::endl;
						co_yield();
						std::cout << "end run" << i << std::endl;
					}
				);
				/************************************************************/
				std::cout << "end produce" << i << std::endl;
			}
		}
	);
	
	std::cout << " produce co done." << std::endl;
}

int test2(){
	for(int i = 0; i<16 ; i++){
		std::cout << "start produce" << i << std::endl;
		co_go( 
			[i](){
				// std::cout << "start run" << i << std::endl;
				co_yield();
				// std::cout << "end run" << i << std::endl;
				 /************************************************************/
				std::cout << "++++++co" << i << "start." << std::endl;
				char buf[1024];
				int j = 1;
				while(j-->0){
					/* socket */
					char buf[4096]; 
					int cfd = 0;
					int ret = 0;
					struct sockaddr_in serv_addr; //服务器地址结构
					serv_addr.sin_family = AF_INET;
					serv_addr.sin_port = htons(7103); 
					inet_pton(AF_INET,"211.67.19.195",&serv_addr.sin_addr.s_addr);
					cfd = Socket(AF_INET,SOCK_STREAM,0); //创建socket
					ret = Connect(cfd ,(struct sockaddr *)&serv_addr ,sizeof(serv_addr) ); //连接服务器端
					
					// 设置非阻塞
					bool block = 1;
					setsockopt(cfd, IPPROTO_TCP, TCP_NODELAY,
							&block, static_cast<socklen_t>(sizeof block));

					//服务器地址结构
					char serv_ip[32];
					// std::cout << inet_ntop(AF_INET,&serv_addr.sin_addr.s_addr,serv_ip,sizeof(serv_ip)) << std::endl;

					msend(cfd , "ping" ,strlen("ping"));

					mread(cfd ,buf ,sizeof(buf));

					close(cfd);
				}
				std::cout << "------co" << i << "all done." << std::endl;
				std::cout << "times" << times << std::endl;
				times +=1 ;
				co_yield();
			}
		 );
		 /************************************************************/
		 std::cout << "end produce" << i << std::endl;
	}
	std::cout << " produce co done." << std::endl;
}

void ntest( int  i){
	char buf[1024];
	int j = 1;
	while(j-->0){
		// std::cout<<"connection:"<<j<<std::endl;
		/* socket */
		char buf[4096]; 
		int cfd = 0;
		int ret = 0;
		struct sockaddr_in serv_addr; //服务器地址结构
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(8099); 
		inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr.s_addr);
		cfd = Socket(AF_INET,SOCK_STREAM,0); //创建socket
		ret = Connect(cfd ,(struct sockaddr *)&serv_addr ,sizeof(serv_addr) ); //连接服务器端
		
		// 设置非阻塞
		bool block = 1;
		setsockopt(cfd, IPPROTO_TCP, TCP_NODELAY,
				&block, static_cast<socklen_t>(sizeof block));

		//服务器地址结构
		char serv_ip[32];
		// std::cout << inet_ntop(AF_INET,&serv_addr.sin_addr.s_addr,serv_ip,sizeof(serv_ip)) << std::endl;

		write(cfd , "ping" ,strlen("ping"));

		read(cfd ,buf ,sizeof(buf));
		// std::cout << buf << std::endl;

		close(cfd);
	}
	std::cout << "------co" << i << "all done." << std::endl;
}
/*
经测试，协程的实现存在问题：
++++++co0start.
++++++co1start.
++++++co0start.
------co1all done.
------co0all done.
++++++co2start.
++++++co3start.
++++++co2start.
------co3all done.
------co2all done.
++++++co4start.
 produce co done.
end
协程恢复后会重新开始运行
待优化。
*/
int main()
{
	test();

	// test2();

	// cotest();

	// for(int i = 0;i<20;i++){
	// 	ntest( i );
	// }
	
	std::cout << "end" << std::endl;
	std::cout << "times" << times << std::endl;
}