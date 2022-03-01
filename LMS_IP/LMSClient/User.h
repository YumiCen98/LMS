#ifndef __USER_H_
#define __USER_H_

#include <string.h>
#include "Connect.h"

//注册
void regist(int sockfd, MSG msg);

//登录
int login(int sockfd, MSG msg);

//管理员登录
int manage_login(int sockfd, MSG msg);

#endif