#ifndef __CONNECT_H_
#define __CONNECT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define N 128

//定义颜色代码
#define COLOR_NONE          "\033[0m"       //恢复默认
#define FONT_COLOR_YELLOW   "\033[1;33m"    //黄色高亮
#define FONT_COLOR_RED      "\033[1;31m"    //红色高亮

#define ERR_LOG(errmsg) do{ \
    perror(errmsg); \
    printf(FONT_COLOR_RED"[%s - %s - %d]\n"COLOR_NONE, __FILE__, __func__, __LINE__); \
    exit(1); \
}while(0)

typedef struct{
    int OptCode;
    int class;
    char Account[N];
    char text[N];
}MSG;

//创建服务器
int CreateServer(void);

//等待客户端连接
int WaitConnect(int sockfd, struct sockaddr_in *clientaddr);

//发送数据
void SendMsg(int clientfd, MSG *msg);

//接收数据,由于要返回接收字节数，需要客户端的地址信息来判断客户端是否退出
int RecvMsg(int clientfd, MSG *msg);

#endif