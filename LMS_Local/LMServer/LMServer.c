#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "LMSqlite.h"
#include "Connect.h"
#include "User.h"


//给线程处理函数传递参数的结构体
typedef struct{
    int acceptfd;
    struct sockaddr_in clientaddr;
    MSG msg;
    sqlite3 *db[2];
}FD_MSG;

//线程处理函数
void *thread_fun(void *arg)
{
    FD_MSG fd_msg = *(FD_MSG *)arg;
    int bytes;

    //保存账户名
    char Account[N] = "";

    while(1)
    {
        memset(&fd_msg.msg, 0, sizeof(MSG));
        printf(FONT_COLOR_YELLOW"[%s:%d]"COLOR_NONE, inet_ntoa(fd_msg.clientaddr.sin_addr), ntohs(fd_msg.clientaddr.sin_port));

        //判断客户端是否退出
        if((bytes = RecvMsg(fd_msg.acceptfd, &fd_msg.msg)) == 0)
        {
            printf(FONT_COLOR_YELLOW"已退出\n"COLOR_NONE);
            close(fd_msg.acceptfd);
            break;
        }

        char sqlcmd[N] = "";

        printf("OptCode = %d\n", fd_msg.msg.OptCode);
        switch(fd_msg.msg.OptCode)
        {
            case 1:
                printf(FONT_COLOR_YELLOW"注册信息\n"COLOR_NONE);
                printf("Account:%s\nPasswd:%s\n", fd_msg.msg.Account, fd_msg.msg.text);
                do_regist(fd_msg.acceptfd, fd_msg.msg, fd_msg.db[0]);
                break;
            case 2:
                printf(FONT_COLOR_YELLOW"登录信息\n"COLOR_NONE);
                printf("Account:%s\nPasswd:%s\n", fd_msg.msg.Account, fd_msg.msg.text);
                strcat(Account, fd_msg.msg.Account);
                do_login(fd_msg.acceptfd, fd_msg.msg, fd_msg.db[0]);
                break;
            case 3:
                printf(FONT_COLOR_YELLOW"管理员登录信息\n"COLOR_NONE);
                printf("Account:%s\nPasswd:%s\n", fd_msg.msg.Account, fd_msg.msg.text);
                do_manage_login(fd_msg.acceptfd, fd_msg.msg, fd_msg.db[0]);
                break;
            

            case 4:
                printf(FONT_COLOR_YELLOW"藏书查询[按书名]\n"COLOR_NONE);
                printf("bookname:%s\n", fd_msg.msg.text);
                sprintf(sqlcmd, "select * from book where bookname like '%%%s%%'", fd_msg.msg.text);
                book_search(fd_msg.acceptfd, fd_msg.msg, fd_msg.db[1], sqlcmd);
                break;
            case 5:
                printf(FONT_COLOR_YELLOW"藏书查询[按作者]\n"COLOR_NONE);
                printf("author:%s\n", fd_msg.msg.text);
                sprintf(sqlcmd, "select * from book where author like '%s'", fd_msg.msg.text);
                book_search(fd_msg.acceptfd, fd_msg.msg, fd_msg.db[1], sqlcmd);
                break;
            /*
            case 6:
                printf(FONT_COLOR_YELLOW"藏书查询[按类别]\n"COLOR_NONE);
                printf("class:%s\n", fd_msg.msg.text);
                sprintf(sqlcmd, "select * from book where class like '%s'", fd_msg.msg.text);
                book_search(fd_msg.acceptfd, fd_msg.msg, fd_msg.db[1], sqlcmd);
                break;
                */
            

            case 7:
                printf(FONT_COLOR_YELLOW"借阅信息\n"COLOR_NONE);
                printf("Account:%s\nbookname:%s\n", Account, fd_msg.msg.text);
                sprintf(sqlcmd, "select * from book where bookname = '%s'", fd_msg.msg.text);
                //printf("%s\n", sqlcmd);
                memset(fd_msg.msg.Account, 0, N);
                strcat(fd_msg.msg.Account, Account);
                //memset(Account, 0, N);
                book_borrow(fd_msg.acceptfd, fd_msg.msg, fd_msg.db[1], fd_msg.db[0], sqlcmd);
                break;
            case 8:
                printf(FONT_COLOR_YELLOW"还书信息\n"COLOR_NONE);
                printf("Account:%s\nbookname:%s\n", Account, fd_msg.msg.text);
                sprintf(sqlcmd, "select * from return where account = '%s' and bookname = '%s'", Account, fd_msg.msg.text);
                //printf("%s\n", sqlcmd);
                memset(fd_msg.msg.Account, 0, N);
                strcat(fd_msg.msg.Account, Account);
                //memset(Account, 0, N);
                book_return(fd_msg.acceptfd, fd_msg.msg, fd_msg.db[1], fd_msg.db[0], sqlcmd);
                break;

            //添加藏书
            case 9:
                printf(FONT_COLOR_YELLOW"添加图书信息\n"COLOR_NONE);
                printf("新增图书：《%s》- %s - %d\n", fd_msg.msg.Account, fd_msg.msg.text, fd_msg.msg.class);
                sprintf(sqlcmd, "select * from book where bookname = '%s'", fd_msg.msg.Account);
                book_add(fd_msg.acceptfd, fd_msg.msg, fd_msg.db[1], sqlcmd);
                break;
            //删除藏书
            case 10:
                printf(FONT_COLOR_YELLOW"删除图书信息\n"COLOR_NONE);
                printf("删除图书：《%s》\n", fd_msg.msg.text);
                sprintf(sqlcmd, "select * from book where bookname = '%s'", fd_msg.msg.text);
                book_del(fd_msg.acceptfd, fd_msg.msg, fd_msg.db[1], sqlcmd);
                break;
            //图书代还
            case 11:
                printf(FONT_COLOR_YELLOW"代还信息\n"COLOR_NONE);
                printf("代还账号：%s - 代还图书《%s》\n", fd_msg.msg.Account, fd_msg.msg.text);
                sprintf(sqlcmd, "select * from return where account = %s and bookname = '%s'", fd_msg.msg.Account, fd_msg.msg.text);
                book_help(fd_msg.acceptfd, fd_msg.msg, fd_msg.db[1], fd_msg.db[0], sqlcmd);
                break;
        }
    }
}

int main()
{
    FD_MSG fd_msg;
    char date[N]="";

    //TCP
    int sockfd, clientfd;
    struct sockaddr_in clientaddr;
    MSG msg;
    int bytes;

    //Sqlite3
    sqlite3 *user_db;
    sqlite3 *book_db;
    char *errmsg;
    char sqlcmd[N];

    //创建服务器
    if((sockfd = CreateServer()) < 0)
    {
        printf("create server error\n");
        return -1;
    }

    //打开数据库
    fd_msg.db[0] = do_sqlopen(USERDBNAME, user_db);
    fd_msg.db[1] = do_sqlopen(BOOKDBNAME, book_db);

    //1.创建用户信息表、藏书表、管理员信息表、借阅信息表
    //普通用户信息表
    sprintf(sqlcmd, "create table user(account text primary key, passwd text, date text)");
    //memset(sqlcmd, 0, N);
    //strcat(sqlcmd, "create table user(account char, paawd char)");
    //这个地方用strcat如果不清空字符数组，命令会有错误。使用sprintf不需要清空
    do_createtable(fd_msg.db[0], sqlcmd);

    //管理员信息表
    sprintf(sqlcmd, "create table manager(account text primary key, passwd text, date text)");
    do_createtable(fd_msg.db[0], sqlcmd);
    getdate(date);
    sprintf(sqlcmd, "insert into manager values('admin', '123456', '%s')", date);
    do_sqlinsert(fd_msg.db[0], sqlcmd);

    //借阅信息表
    sprintf(sqlcmd, "create table return(account text, bookname text, date text, return int)");
    do_createtable(fd_msg.db[0], sqlcmd);
    
    //2.创建藏书数据库
    //创建藏书表
    sprintf(sqlcmd, "create table book(bookname text primary key, author text, class text, count int, date text)");
    do_createtable(fd_msg.db[1], sqlcmd);

    //使用对线程实现TCP并发服务器
    pthread_t thread;

    while(1)
    {
        //等待客户端连接，并保存客户端的地址
        if((clientfd = WaitConnect(sockfd, &clientaddr)) < 0)
        {
            printf("connect server error\n");
            return -1;
        }

        //将参数传递到结构体中
        fd_msg.acceptfd = clientfd;
        fd_msg.clientaddr = clientaddr;

        //创建线程
        if(pthread_create(&thread, NULL, thread_fun, &fd_msg) != 0)
        {
            ERR_LOG("fail to pthread_create");
        }

        //设置线程为分离属性，线程退出时释放资源
        pthread_detach(thread);
    }


    return 0;
}
