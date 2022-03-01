#include "Operation.h"

//查询图书
void book_search(int sockfd, MSG msg)
{
    int Opt = 0;
    //保存账户名
    char Account[N] = "";
    strcat(Account, msg.Account);

NEXT:
    printf(FONT_COLOR_YELLOW"[图书查询界面]\n"COLOR_NONE);
    printf(" \
            ********************\n \
            ****1、按书名查找****\n \
            ****2、按作者查找****\n \
            **3、按类别(待完善)**\n \
            *******0、返回******\n \
            ********************\n");

    strcat(msg.Account, Account);
    printf(FONT_COLOR_BLUE"input:>"COLOR_NONE);
    scanf("%d", &Opt);
    switch(Opt)
    {
        case 0:
            return ;
            break;
        case 1:
            msg.OptCode = 4;
            printf(FONT_COLOR_BLUE"书名(不需要书名号):>"COLOR_NONE);
            scanf("%s", msg.text);
            break;
        case 2:
            msg.OptCode = 5;
            printf(FONT_COLOR_BLUE"作者:>"COLOR_NONE);
            scanf("%s", msg.text);
            break;
        case 3:
        /*
            msg.OptCode = 6;
            printf("1、文学 2、技术 3、科学\n");
            printf(FONT_COLOR_BLUE"选择类别:>"COLOR_NONE);
            scanf("%s", msg.text);
        */
            printf(FONT_COLOR_RED"待完善，重新选择\n"COLOR_NONE);
            goto NEXT;
            break;
        default:
            printf(FONT_COLOR_RED"请输入正确的序号\n"COLOR_NONE);
            goto NEXT;
            break;
    }
    //printf("Account_Search = %s\n", msg.Account);
    SendMsg(sockfd, &msg);
    memset(&msg, 0, sizeof(MSG));
    RecvMsg(sockfd, &msg);

    if(strcmp(msg.text, "book_search_fail") == 0)
    {
        printf(FONT_COLOR_RED"[藏书查询失败]\n"COLOR_NONE);
    }
    else if(strcmp(msg.text, "no_book") == 0)
    {
        printf(FONT_COLOR_RED"[结果为空]\n"COLOR_NONE);
    }
    else
    {
        printf(FONT_COLOR_YELLOW"[藏书查询成功]\n"COLOR_NONE);
        printf("%s\n", msg.text);
    }

    goto NEXT;
}

//借阅图书
void book_borrow(int sockfd, MSG msg)
{
    int Opt;
    int choose;
    msg.OptCode = 7;

    printf(FONT_COLOR_YELLOW"[借阅界面]\n"COLOR_NONE);
    //printf("Account_Borrow = %s\n", msg.Account);

    printf(FONT_COLOR_BLUE"借阅书名(不需要书名号):>"COLOR_NONE);
    scanf("%s", msg.text);
    SendMsg(sockfd, &msg);
    memset(&msg.text, 0, N);

    RecvMsg(sockfd, &msg);
    if(strcmp(msg.text, "book_borrow_fail") == 0)
    {
        printf(FONT_COLOR_RED"[藏书剩余查询失败]\n"COLOR_NONE);
        return ;
    }
    else if((atoi(msg.text)) == 0)
    {
        printf(FONT_COLOR_RED"[该书已被借完]\n"COLOR_NONE);
        return ;
    }
    else if((strcmp(msg.text, "no_book")) == 0)
    {
        printf(FONT_COLOR_RED"[没有该书]\n"COLOR_NONE);
        return ;
    }
    else if((atoi(msg.text)) > 0)
    {
        printf(FONT_COLOR_YELLOW"[图书剩余%d本]\n"COLOR_NONE, atoi(msg.text));
NEXT:
        printf(FONT_COLOR_BLUE"是否借阅[1.是 2.否]\n"COLOR_NONE);
        memset(msg.text, 0, N);
        printf(FONT_COLOR_BLUE"input:>"COLOR_NONE);
        //fgets(choose, sizeof(choose), stdin);
        //choose[strlen(choose)-1] = '\0';
        scanf("%d", &choose);

        switch(choose)
        {
            case 1:
                strcat(msg.text, "yes");
                SendMsg(sockfd, &msg);
                memset(msg.text, 0, N);
                RecvMsg(sockfd, &msg);
                //printf("%s\n", msg.text);
                if(strcmp(msg.text, "borrowed") == 0)
                {
                    printf(FONT_COLOR_YELLOW"[借阅成功，在还书界面可查看借阅记录]\n"COLOR_NONE);
                    return ;
                }
                else
                {
                    printf(FONT_COLOR_YELLOW"[借阅失败，请重试]\n"COLOR_NONE);
                    return ;
                }
                break;
            case 2:
                strcat(msg.text, "no");
                SendMsg(sockfd, &msg);
                memset(msg.text, 0, N);
                break;
            default:
                printf(FONT_COLOR_YELLOW"[输入有误，重新输入]\n"COLOR_NONE);
                goto NEXT;
                break;
        }
        
        return ;
    }
}

//还书
void book_return(int sockfd, MSG msg)
{
    int Opt, choose;
    msg.OptCode = 8;
    printf(FONT_COLOR_YELLOW"[还书界面]\n"COLOR_NONE);
    printf(FONT_COLOR_BLUE"待还书名(不需要书名号):>"COLOR_NONE);
    scanf("%s", msg.text);
    //printf("Account_Return = %s\n", msg.Account);
    SendMsg(sockfd, &msg);
    memset(&msg, 0, sizeof(MSG));

    RecvMsg(sockfd, &msg);
    if(strcmp(msg.text, "search_return_fail") == 0)
    {
        printf(FONT_COLOR_RED"[藏书归还失败]\n"COLOR_NONE);
        return ;
    }
    else if((atoi(msg.text)) == 0)
    //else if((strcmp(msg.text, "return_finish")) == 0)
    {
        printf(FONT_COLOR_RED"[该书已还完]\n"COLOR_NONE);
        return ;
    }
    else if((strcmp(msg.text, "no_borrow")) == 0)
    {
        printf(FONT_COLOR_RED"[未借阅该书]\n"COLOR_NONE);
    }
    else if((atoi(msg.text)) > 0)
    {
        printf(FONT_COLOR_YELLOW"[该书待还%d本]\n"COLOR_NONE, atoi(msg.text));
NEXT:
        printf(FONT_COLOR_BLUE"是否还书[1.是 2.否]\n"COLOR_NONE);
        memset(msg.text, 0, N);
        printf(FONT_COLOR_BLUE"input:>"COLOR_NONE);
        scanf("%d", &choose);

        switch(choose)
        {
            case 1:
                strcat(msg.text, "yes");
                SendMsg(sockfd, &msg);
                memset(msg.text, 0, N);
                RecvMsg(sockfd, &msg);
                //printf("%s\n", msg.text);
                if(strcmp(msg.text, "returned") == 0)
                {
                    printf(FONT_COLOR_YELLOW"[还书成功]\n"COLOR_NONE);
                    return ;
                }
                else if(strcmp(msg.text, "no_borrow") == 0)
                {
                    printf(FONT_COLOR_YELLOW"[未借阅该书]\n"COLOR_NONE);
                    return ;
                }
                else
                {
                    printf(FONT_COLOR_RED"[还书失败]\n"COLOR_NONE);
                    return ;
                }
                break;
            case 2:
                strcat(msg.text, "no");
                SendMsg(sockfd, &msg);
                memset(msg.text, 0, N);
                break;
            default:
                printf(FONT_COLOR_YELLOW"[输入有误，重新输入]\n"COLOR_NONE);
                goto NEXT;
                break;
        }
        
        return ;
    }
}

//添加藏书
void book_add(int sockfd, MSG msg)
{
    memset(&msg, 0, sizeof(MSG));
    msg.OptCode = 9;
    printf(FONT_COLOR_YELLOW"[新增图书界面]\n"COLOR_NONE);
    printf("【书类编号】1.文学 2.技术 3.科学\n");
    printf(FONT_COLOR_BLUE"输入[书名 作者 书类编号]:>"COLOR_NONE);
    scanf("%s%s%d", msg.Account, msg.text, &msg.class);
/*
    printf(FONT_COLOR_BLUE"输入书名:>"COLOR_NONE);
    scanf("%s", msg.Account);
    printf(FONT_COLOR_BLUE"输入作者:>"COLOR_NONE);
    scanf("%s", msg.text);
    printf(FONT_COLOR_BLUE"输入书类编号:>"COLOR_NONE);
    scanf("%d", &msg.class);
*/
    SendMsg(sockfd, &msg);
    memset(&msg, 0, sizeof(MSG));
    RecvMsg(sockfd, &msg);

    if(strcmp(msg.text, "add_success") == 0)
    {
        printf(FONT_COLOR_YELLOW"[添加图书成功]\n"COLOR_NONE);
        return ;
    }
    else
    {
        printf(FONT_COLOR_RED"[添加图书失败]\n"COLOR_NONE);
        return ;
    }
}

//删除藏书
void book_del(int sockfd, MSG msg)
{
    memset(&msg, 0, sizeof(MSG));
    msg.OptCode = 10;
    printf(FONT_COLOR_YELLOW"[删除图书界面]\n"COLOR_NONE);
    printf(FONT_COLOR_BLUE"输入书名:>"COLOR_NONE);
    scanf("%s", msg.text);

    SendMsg(sockfd, &msg);
    memset(&msg, 0, sizeof(MSG));
    RecvMsg(sockfd, &msg);

    if(strcmp(msg.text, "del_success") == 0)
    {
        printf(FONT_COLOR_YELLOW"[删除图书成功]\n"COLOR_NONE);
        return ;
    }
    else
    {
        printf(FONT_COLOR_RED"[删除图书失败]\n"COLOR_NONE);
        return ;
    }
}

//图书代还
void book_help(int sockfd, MSG msg)
{
    memset(&msg, 0, sizeof(MSG));
    msg.OptCode = 11;

    printf(FONT_COLOR_YELLOW"[代还界面]\n"COLOR_NONE);
    printf(FONT_COLOR_BLUE"借阅账号:>"COLOR_NONE);
    scanf("%s", msg.Account);
    printf(FONT_COLOR_BLUE"借阅书名:>"COLOR_NONE);
    scanf("%s", msg.text);

    SendMsg(sockfd, &msg);
    memset(&msg, 0, sizeof(MSG));
    RecvMsg(sockfd, &msg);

    if(strcmp(msg.text, "help_success") == 0)
    {
        printf(FONT_COLOR_YELLOW"[代还成功]\n"COLOR_NONE);
        return ;
    }
    else
    {
        printf(FONT_COLOR_RED"[代还失败]\n"COLOR_NONE);
        return ;
    }
}
