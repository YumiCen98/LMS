#include "Connect.h"

//连接服务器
int ConnectServer(char const *ipaddr, char const *port)
{
    int sockfd;
    struct sockaddr_in serveraddr;
    socklen_t addrlen = sizeof(serveraddr);

    //1.创建套接字
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        ERR_LOG("fail to socket");
    }

    //2.填充服务器网络信息结构体
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(ipaddr);
    serveraddr.sin_port = htons(atoi(port));

    //3.向服务器发送连接请求
    if(connect(sockfd, (struct sockaddr *)&serveraddr, addrlen) == -1)
    {
        ERR_LOG("fail to connect");
    }

    return sockfd;
}

//发送数据
void SendMsg(int sockfd, MSG *msg)
{
    if((send(sockfd, msg, sizeof(MSG), 0)) == -1)
    {
        ERR_LOG("fail to send");
    }
    //printf("send success\n");
}

//接收数据
void RecvMsg(int sockfd, MSG *msg)
{
    int bytes;
    if((bytes = recv(sockfd, msg, sizeof(MSG), 0)) == -1)
    {
        ERR_LOG("fail to recv");
    }
    else if(bytes == 0)
    {
        printf(FONT_COLOR_YELLOW"[Server Quit]\n"COLOR_NONE);
        exit(0);
    }
}