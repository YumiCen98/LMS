#include "LoginUi.h"
#include "Operation.h"

//用户登陆后的界面
void User_Centen(int sockfd, MSG msg)
{
    int Opt, ret;
    //保存账户名
    char Account[N] = "";
    strcat(Account, msg.Account);

NEXT:
    //登录后界面
    printf(" \
        ********************\n \
        ******1、查询******\n \
        ******2、借阅******\n \
        ******3、还书******\n \
        ******0、退出账号******\n \
        ********************\n");

    memset(msg.Account, 0, N);
    strcat(msg.Account, Account);
    //printf("aaa = %s\n", msg.Account);
    printf(FONT_COLOR_BLUE"input:>"COLOR_NONE);
    scanf("%d", &Opt);
    switch(Opt)
    {
        case 0:
            return ;
            break;
        case 1:
            //printf("a_s = %s\n", msg.Account);
            book_search(sockfd, msg);
            break;
        case 2:
            //printf("a_b = %s\n", msg.Account);
            book_borrow(sockfd, msg);
            break;
        case 3:
            //printf("a_r = %s\n", msg.Account);
            book_return(sockfd, msg);
            break;
        default:
            printf(FONT_COLOR_RED"请输入正确的序号\n"COLOR_NONE);
            break;
    }

    memset(msg.Account, 0, N);
    goto NEXT;
}


//管理员登陆后的界面
void manage_Centen(int sockfd, MSG msg)
{
    int Opt, ret;
    //保存账户名
    char Account[N] = "";
    strcat(Account, msg.Account);

NEXT:
    //登录后界面
    printf(" \
        ********************\n \
        *****1、添加藏书*****\n \
        *****2、删除藏书*****\n \
        *****3、图书代还*****\n \
        *****0、退出管理*****\n \
        ********************\n");

    memset(msg.Account, 0, N);
    strcat(msg.Account, Account);
    printf(FONT_COLOR_BLUE"input:>"COLOR_NONE);
    scanf("%d", &Opt);
    switch(Opt)
    {
        case 0:
            return ;
            break;
        case 1:
            book_add(sockfd, msg);
            break;
        case 2:
            book_del(sockfd, msg);
            break;
        case 3:
            book_help(sockfd, msg);
            break;
        default:
            printf(FONT_COLOR_RED"请输入正确的序号\n"COLOR_NONE);
            break;
    }

    memset(msg.Account, 0, N);
    goto NEXT;
}