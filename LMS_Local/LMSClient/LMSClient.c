#include "Connect.h"
#include "User.h"
#include "Operation.h"
#include "LoginUi.h"

int main(int argc, char const *argv[])
{

    int sockfd, clientfd;
    struct sockaddr_in clientaddr;
    MSG msg;
    int bytes;

    int Opt, ret, i;

    sockfd = ConnectServer();

    while(1)
    {
        memset(&msg, 0, sizeof(MSG));

        //主界面
        printf(" \
            ********************\n \
            ******1、注册******\n \
            ******2、登录******\n \
            ******3、管理******\n \
            ******0、退出******\n \
            ********************\n");

        printf(FONT_COLOR_BLUE"input:>"COLOR_NONE);
        scanf("%d", &Opt);
        switch(Opt)
        {
            case 0:
                exit(0);
            case 1:
                msg.OptCode = 1;
                printf(FONT_COLOR_YELLOW"[注册界面]\n"COLOR_NONE);
ACCOUNT:
                printf(FONT_COLOR_BLUE"11位数字账号:>"COLOR_NONE);
                scanf("%s", msg.Account);

                for(i=0; i<strlen(msg.Account); i++)
                {
                    if(!((msg.Account[i] >= '0') && (msg.Account[i] <= '9')))
                    {
                        printf(FONT_COLOR_RED"[账号为纯数字]\n"COLOR_NONE);
                        memset(msg.Account, 0, N);
                        goto ACCOUNT;
                    }
                }
                if(strlen(msg.Account) != 11)
                {
                    printf(FONT_COLOR_RED"[请输入11位数字账号]\n"COLOR_NONE);
                    memset(msg.Account, 0, N);
                    goto ACCOUNT;
                }
PASSWD:
                printf(FONT_COLOR_BLUE"不小于8位密码:>"COLOR_NONE);
                scanf("%s", msg.text);
                if(strlen(msg.text) < 8)
                {
                    printf(FONT_COLOR_RED"[密码长度不满8位]\n"COLOR_NONE);
                    memset(msg.text, 0, N);
                    goto PASSWD;
                }

                regist(sockfd, msg);
                break;
            case 2:
                msg.OptCode = 2;
                printf(FONT_COLOR_YELLOW"[登录界面]\n"COLOR_NONE);
                printf(FONT_COLOR_BLUE"账号:>"COLOR_NONE);
                scanf("%s", msg.Account);

                printf(FONT_COLOR_BLUE"密码:>"COLOR_NONE);
                scanf("%s", msg.text);

                if(login(sockfd, msg) == 0)
                {
                    User_Centen(sockfd, msg);       
                }
                else
                {
                    printf(FONT_COLOR_RED"登录失败\n"COLOR_NONE);
                }
                break;
            case 3:
                msg.OptCode = 3;
                printf(FONT_COLOR_YELLOW"[管理员登录界面]\n"COLOR_NONE);
                printf(FONT_COLOR_BLUE"[默认账号：admin 密码：123456]\n"COLOR_NONE);
                printf(FONT_COLOR_BLUE"账号:>"COLOR_NONE);
                scanf("%s", msg.Account);

                printf(FONT_COLOR_BLUE"密码:>"COLOR_NONE);
                scanf("%s", msg.text);

                if(manage_login(sockfd, msg) == 0)
                {
                    manage_Centen(sockfd, msg);
                }
                else
                {
                    //printf(FONT_COLOR_RED"管理员登录失败\n"COLOR_NONE);
                }
                break;
            default:
                printf(FONT_COLOR_RED"请输入正确的序号\n"COLOR_NONE);
                break;
        }
    }

    return 0;
}
