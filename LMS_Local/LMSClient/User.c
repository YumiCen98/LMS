#include "User.h"

//注册
void regist(int sockfd, MSG msg)
{
    SendMsg(sockfd, &msg);
    memset(&msg, 0, sizeof(MSG));
    RecvMsg(sockfd, &msg);
    if(strcmp(msg.text, "regist_success") == 0)
    {
        printf(FONT_COLOR_YELLOW"[注册成功]\n"COLOR_NONE);
        return ;
    }
    else if(strcmp(msg.text, "user_exist") == 0)
    {
        printf(FONT_COLOR_RED"[用户已注册，请登录]\n"COLOR_NONE);
        return ;
    }
    //else if(strcmp(msg.text, "regist_fail") == 0)
    else
    {
        printf(FONT_COLOR_RED"[注册失败]\n"COLOR_NONE);
        return ;
    }
}

//登录
int login(int sockfd, MSG msg)
{
    SendMsg(sockfd, &msg);
    memset(&msg.text, 0, N);
    RecvMsg(sockfd, &msg);
    if(strcmp(msg.text, "login_success") == 0)
    {
        printf(FONT_COLOR_YELLOW"[登录成功]\n"COLOR_NONE);
        return 0;
    }
    else if(strcmp(msg.text, "Ac_or_Pd_error") == 0)
    {
        printf(FONT_COLOR_RED"[账号或密码有误]\n"COLOR_NONE);
        return 1;
    }
    else
    {
        printf(FONT_COLOR_RED"[登录失败]\n"COLOR_NONE);
        return -1;
    }
}

//管理员登录
int manage_login(int sockfd, MSG msg)
{
    //printf("MAccount:%s - MPasswd:%s\n", msg.Account, msg.text);
    SendMsg(sockfd, &msg);
    memset(&msg.text, 0, N);
    RecvMsg(sockfd, &msg);
    if(strcmp(msg.text, "manager_login_success") == 0)
    {
        printf(FONT_COLOR_YELLOW"[管理登录成功]\n"COLOR_NONE);
        return 0;
    }
    else if(strcmp(msg.text, "Ac_or_Pd_error") == 0)
    {
        printf(FONT_COLOR_RED"[账号或密码有误]\n"COLOR_NONE);
        return 1;
    }
    else
    {
        printf(FONT_COLOR_RED"[管理登录失败]\n"COLOR_NONE);
        return -1;
    }
}
