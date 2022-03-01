#ifndef __LOGINUI_H_
#define __LOGINUI_H_

#include "Connect.h"

//用户登陆后界面
void User_Centen(int sockfd, MSG msg);

//管理员登陆后界面
void manage_Centen(int sockfd, MSG msg);

#endif