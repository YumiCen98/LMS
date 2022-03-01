#include "LMSqlite.h"

//获取系统时间
void getdate(char date[])
{
    time_t t;
    struct tm *tp;
    time(&t);
    tp = localtime(&t);
    sprintf(date, "%d-%d-%d %d:%d:%d",
        1900+tp->tm_year, 1+tp->tm_mon, tp->tm_mday,
        tp->tm_hour, tp->tm_min, tp->tm_sec);
}

//创建/打开数据库
//open后需要将数据库句柄返回，否则后面的操作无法进行
sqlite3 *do_sqlopen(const char *dbname, sqlite3 *db)
{
    if(sqlite3_open(dbname, &db) != SQLITE_OK)
    {
        printf(FONT_COLOR_RED"[数据库创建/打开错误]%s\n"COLOR_NONE, sqlite3_errmsg(db));
    }
    else
    {
        printf(FONT_COLOR_YELLOW"[数据库创建/打开成功]\n"COLOR_NONE);
        return db;
    }

}

//创建表
void do_createtable(sqlite3 *db, const char *sqlcmd)
{
    char *errmsg;
    if(sqlite3_exec(db, sqlcmd, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf(FONT_COLOR_RED"[创建表失败]%s\n"COLOR_NONE, errmsg);
    }
    else
    {
        printf(FONT_COLOR_YELLOW"[创建表成功]\n"COLOR_NONE);
    }
}

int do_check_key(sqlite3 *db, MSG msg, const char *sqlcmd)
{
    char *errmsg, **result;
    int nrow, ncolum;

    if(sqlite3_get_table(db, sqlcmd, &result, &nrow, &ncolum, &errmsg) != SQLITE_OK)
    {
        printf(FONT_COLOR_RED"[查询表失败]%s\n"COLOR_NONE, errmsg);
        return -1;
    }
    if(nrow == 0)
    {
        printf(FONT_COLOR_RED"[未查到记录]\n"COLOR_NONE);
        return 1;
    }
    else
    {
        printf(FONT_COLOR_YELLOW"[查询表成功]\n"COLOR_NONE);
        return 0;
    }
}

int do_sqlinsert(sqlite3 *db, const char *sqlcmd)
{
    char *errmsg;

    if(sqlite3_exec(db, sqlcmd, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf(FONT_COLOR_RED"[插入数据失败]%s\n"COLOR_NONE, errmsg);
        if(errno == 2)
        {
            return 1;
        }
        return -1;
    }
    else
    {
        printf(FONT_COLOR_YELLOW"[插入数据成功]\n"COLOR_NONE);
        return 0;
    }
}

void sql_update(sqlite3 *db, const char *sqlcmd)
{
    char *errmsg;

    if(sqlite3_exec(db, sqlcmd, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf(FONT_COLOR_RED"[更新数据库失败]%s\n"COLOR_NONE, errmsg);
    }
    else
    {
        printf(FONT_COLOR_YELLOW"[更新数据库成功]\n"COLOR_NONE);
    }
}

int search_callback(void *arg, int f_num, char **f_value, char **f_name)
{
    int sockfd = *(int *)arg;
    MSG msg;

    sprintf(msg.text, "<<%s>> - 作者:%s - 类别:%s - 剩余馆藏:%s", f_value[0], f_value[1], f_value[2], f_value[3]);
    //printf(FONT_COLOR_YELLOW"[获取数据库成功]\n"COLOR_NONE);
    //printf("%s\n", msg.text);

    SendMsg(sockfd, &msg);
    return 0;
}

void do_search(int sockfd, sqlite3 *db, MSG msg, const char *sqlcmd)
{
    char *errmsg, **result;
    int nrow, ncolum;

    if(sqlite3_get_table(db, sqlcmd, &result, &nrow, &ncolum, &errmsg) != SQLITE_OK)
    {
        printf(FONT_COLOR_RED"[查询表失败]%s\n"COLOR_NONE, errmsg);
        sprintf(msg.text, "book_search_fail");
        SendMsg(sockfd, &msg);
        return ;
    }
    if(nrow == 0)
    {
        printf(FONT_COLOR_YELLOW"[没有该书]\n"COLOR_NONE);
        sprintf(msg.text, "no_book");
        SendMsg(sockfd, &msg);
        return ;
    }
    else
    {
        printf(FONT_COLOR_YELLOW"[查询表成功]\n"COLOR_NONE);
        if(sqlite3_exec(db, sqlcmd, search_callback, (void *)&sockfd, &errmsg) != SQLITE_OK)
        {
            printf(FONT_COLOR_RED"[获取结果失败]%s\n"COLOR_NONE, errmsg);
            sqlite3_free(errmsg);
            sprintf(msg.text, "book_search_fail");
            SendMsg(sockfd, &msg);
            return ;
        }
        else
        {
            printf(FONT_COLOR_YELLOW"[获取结果成功]\n"COLOR_NONE);
            return ;
        }
    }
}

int borrow_callback(void *arg, int f_num, char **f_value, char **f_name)
{
    int sockfd = *(int *)arg;
    MSG msg;
    //if((atoi(f_value[3])) == 0)
    if(strcmp(f_value[3], "0") == 0)
    {
        //sprintf(msg.text, "该书已被借完");
        printf(FONT_COLOR_RED"[该书已被借完]\n"COLOR_NONE);
    }
    else
    {
        sprintf(msg.text, "%s", f_value[3]);
        printf(FONT_COLOR_YELLOW"[该书剩余%d本]\n"COLOR_NONE, atoi(f_value[3]));
    }

    SendMsg(sockfd, &msg);
    return 0;
}

void do_borrow(int sockfd, MSG msg, sqlite3 *book_db, sqlite3 *user_db, const char *sqlcmd)
{
    char *errmsg, **result;
    int nrow, ncolum;
    int ret;

    //printf("account = %s & bookname = %s\n", msg.Account, msg.text);

    //保存书名
    char bookname[N] = "";
    strcat(bookname, msg.text);

    //保存账户
    char Account[N];
    memset(Account, 0, N);
    strcat(Account, msg.Account);
    
    char updatecmd[N] = "";

    //获取系统时间
    char date[N] = "";

    if(sqlite3_get_table(book_db, sqlcmd, &result, &nrow, &ncolum, &errmsg) != SQLITE_OK)
    {
        printf(FONT_COLOR_RED"[查询剩余图书失败]%s\n"COLOR_NONE, errmsg);
        sprintf(msg.text, "book_borrow_fail");
        SendMsg(sockfd, &msg);
        return ;
    }
    if(nrow == 0)
    {
        printf(FONT_COLOR_RED"[没有该书]\n"COLOR_NONE);
        sprintf(msg.text, "no_book");
        SendMsg(sockfd, &msg);
        return ;
    }
    else
    {
        printf(FONT_COLOR_YELLOW"[查询表成功]\n"COLOR_NONE);
        if(sqlite3_exec(book_db, sqlcmd, borrow_callback, (void *)&sockfd, &errmsg) != SQLITE_OK)
        {
            printf(FONT_COLOR_RED"[获取结果失败]%s\n"COLOR_NONE, errmsg);
            sprintf(msg.text, "book_borrow_fail");
            SendMsg(sockfd, &msg);
            return ;
        }
        else
        {
            printf(FONT_COLOR_YELLOW"[获取结果成功]\n"COLOR_NONE);

            //收到确认借阅请求
            RecvMsg(sockfd, &msg);
            //printf("%s\n", msg.text);
            if(strcmp(msg.text, "yes") == 0)
            {
                //将书库的馆藏减1
                getdate(date);
                sprintf(updatecmd, "update book set count=count-1, date='%s' where bookname = '%s'", date, bookname);
                sql_update(book_db, updatecmd);

                getdate(date);
                //printf(FONT_COLOR_YELLOW"[借阅时间:%s]\n"COLOR_NONE, date);
                //printf("account = %s & bookname = %s\n", Account, bookname);
                
                //存在借阅信息。更新
                //(acount text primary key, bookname text, date text, return int)
                memset(updatecmd, 0, N);
                sprintf(updatecmd, "select * from return where account = %s and bookname = '%s'", Account, bookname);
                ret = do_check_key(user_db, msg, updatecmd);
                //printf("do_check_key_ret = %d\n", ret);
                memset(updatecmd, 0, N);
                //if((ret = do_check_key(user_db, msg, updatecmd)) == 1)
                //printf("ret = %d\n", ret);
                if(ret == 0)
                {
                    sprintf(updatecmd, "update return set return=return+1, date='%s' where account = '%s' and bookname = '%s'", date, Account, bookname);
                    //printf("%s\n", updatecmd);
                    sql_update(user_db, updatecmd);
                }
                //不存在借阅信息，插入
                else
                {
                    //(account text primary key, bookname text, date text, return int)
                    //printf("插入信息：%s - %s - %s\n", Account, bookname, date);
                    sprintf(updatecmd, "insert into return values('%s', '%s', '%s', 1)", Account, bookname, date);
                    //printf("%s\n", updatecmd);
                    ret = do_sqlinsert(user_db, updatecmd);
                }

                memset(msg.text, 0, N);
                strcat(msg.text, "borrowed");
                SendMsg(sockfd, &msg);

                //更新用户借阅表，将用户借阅表的代换数加1
            }
            else if(strcmp(msg.text, "no") == 0)
            {
                return ;
            }

            return ;
        }
    }
}

//还书回调函数
int return_callback(void *arg, int f_num, char **f_value, char **f_name)
{
    int sockfd = *(int *)arg;
    MSG msg;
    //if((atoi(f_value[3])) == 0)
    if(strcmp(f_value[3], "0") == 0)
    {
        sprintf(msg.text, "return_finish");
        printf(FONT_COLOR_RED"[该书已归还]\n"COLOR_NONE);
    }
    else
    {
        sprintf(msg.text, "%s", f_value[3]);
        printf(FONT_COLOR_YELLOW"[该书待还%d本]\n"COLOR_NONE, atoi(f_value[3]));
    }

    SendMsg(sockfd, &msg);
    return 0;
}

//还书
void do_return(int sockfd, MSG msg, sqlite3 *book_db, sqlite3 *user_db, const char *sqlcmd)
{
    char *errmsg, **result;
    int nrow, ncolum;
    int ret;

    //printf("account = %s & bookname = %s\n", msg.Account, msg.text);

    //保存书名
    char bookname[N] = "";
    strcat(bookname, msg.text);

    //保存账户
    char Account[N];
    memset(Account, 0, N);
    strcat(Account, msg.Account);
    
    char updatecmd[N] = "";

    //获取系统时间
    char date[N] = "";

    if(sqlite3_get_table(user_db, sqlcmd, &result, &nrow, &ncolum, &errmsg) != SQLITE_OK)
    {
        printf(FONT_COLOR_RED"[查询借阅信息失败]%s\n"COLOR_NONE, errmsg);
        sprintf(msg.text, "search_return_fail");
        SendMsg(sockfd, &msg);
        return ;
    }
    if(nrow == 0)
    {
        printf(FONT_COLOR_RED"[未借阅该书]\n"COLOR_NONE);
        sprintf(msg.text, "no_borrow");
        SendMsg(sockfd, &msg);
        return ;
    }
    else
    {
        printf(FONT_COLOR_YELLOW"[查询表成功]\n"COLOR_NONE);
        if(sqlite3_exec(user_db, sqlcmd, return_callback, (void *)&sockfd, &errmsg) != SQLITE_OK)
        {
            printf(FONT_COLOR_RED"[获取结果失败]%s\n"COLOR_NONE, errmsg);
            sprintf(msg.text, "search_return_fail");
            SendMsg(sockfd, &msg);
            return ;
        }
        else
        {
            printf(FONT_COLOR_YELLOW"[获取结果成功]\n"COLOR_NONE);

            //收到确认还书请求
            RecvMsg(sockfd, &msg);
            //printf("%s\n", msg.text);
            if(strcmp(msg.text, "yes") == 0)
            {
                //将书库的馆藏加1
                getdate(date);
                sprintf(updatecmd, "update book set count=count+1, date='%s' where bookname = '%s'", date, bookname);
                sql_update(book_db, updatecmd);

                getdate(date);
                //printf(FONT_COLOR_YELLOW"[借阅时间:%s]\n"COLOR_NONE, date);
                //printf("account = %s & bookname = %s\n", Account, bookname);
                
                //存在借阅信息。更新
                //(acount text primary key, bookname text, date text, return int)
                memset(updatecmd, 0, N);
                sprintf(updatecmd, "select * from return where account = '%s' and bookname = '%s'", Account, bookname);
                ret = do_check_key(user_db, msg, updatecmd);
                //printf("do_check_key_ret1 = %d\n", ret);
                memset(updatecmd, 0, N);
                //if((ret = do_check_key(user_db, msg, updatecmd)) == 1)
                //printf("ret = %d\n", ret);
                if(ret == 0)
                {
                    sprintf(updatecmd, "update return set return=return-1, date='%s' where account = '%s' and bookname = '%s'", date, Account, bookname);
                    //printf("%s\n", updatecmd);
                    sql_update(user_db, updatecmd);
                    memset(msg.text, 0, N);
                    strcat(msg.text, "returned");
                }
                //不存在借阅信息
                else
                {
                    printf(FONT_COLOR_RED"[不存在借阅信息]\n"COLOR_NONE);
                    memset(msg.text, 0, N);
                    strcat(msg.text, "no_borrow");
                }

                SendMsg(sockfd, &msg);

                //更新用户借阅表，将用户借阅表的代换数减1
            }
            else if(strcmp(msg.text, "no") == 0)
            {
                return ;
            }

            return ;
        }
    }
}

void do_add(int sockfd, MSG msg, sqlite3 *db, char *sqlcmd)
{
    int ret;
    char class[N] = "";
    char date[N] = "";
    getdate(date);

    //printf("%s\n", sqlcmd);
    ret = do_check_key(db, msg, sqlcmd);
    //printf("do_add_ret = %d\n", ret);
    if(ret == 0) //有该书，更新
    {
        sprintf(sqlcmd, "update book set count=count+1, date='%s' where bookname='%s'", date, msg.Account);
        //printf("%s\n", sqlcmd);
        sql_update(db, sqlcmd);
        memset(msg.text, 0, N);
        strcat(msg.text, "add_success");
    }
    else if(ret == 1) //没有该书，插入
    {
        if(msg.class == 1)
        {
            strcat(class, "文学");
        }
        else if(msg.class == 2)
        {
            strcat(class, "技术");
        }
        else if(msg.class == 3)
        {
            strcat(class, "科学");
        }

        getdate(date);
        sprintf(sqlcmd, "insert into book values('%s', '%s', '%s', 1, '%s')", msg.Account, msg.text, class, date);
        //printf("%s\n", sqlcmd);
        do_sqlinsert(db, sqlcmd);
        memset(msg.text, 0, N);
        strcat(msg.text, "add_success");
    }
    else 
    {
        strcat(msg.text, "add_fail");
    }

    SendMsg(sockfd, &msg);
}

void do_del(int sockfd, MSG msg, sqlite3 *db, char *sqlcmd)
{
    int ret;
    char date[N] = "";
    getdate(date);

    //printf("%s\n", sqlcmd);
    ret = do_check_key(db, msg, sqlcmd);
    //printf("do_del_ret = %d\n", ret);
    if(ret == 0) //有该书，更新
    {
        sprintf(sqlcmd, "update book set count=count-1, date='%s' where bookname='%s'", date, msg.text);
        //printf("%s\n", sqlcmd);
        sql_update(db, sqlcmd);
        memset(msg.text, 0, N);
        strcat(msg.text, "del_success");
    }
    else 
    {
        strcat(msg.text, "add_fail");
    }

    SendMsg(sockfd, &msg);
}

void do_help(int sockfd, MSG msg, sqlite3 *book_db, sqlite3 *user_db, char *sqlcmd)
{
    int ret;
    char date[N] = "";
    getdate(date);

    //printf("%s\n", sqlcmd);
    ret = do_check_key(user_db, msg, sqlcmd);
    //printf("do_help_ret = %d\n", ret);
    if(ret == 0) //有借阅记录
    {
        sprintf(sqlcmd, "update return set return=return-1, date='%s' where account = %s and bookname='%s'", date, msg.Account, msg.text);
        //printf("%s\n", sqlcmd);
        sql_update(user_db, sqlcmd);

        sprintf(sqlcmd, "update book set count=count+1, date='%s' where bookname='%s'", date, msg.text);
        //printf("%s\n", sqlcmd);
        sql_update(book_db, sqlcmd);

        memset(msg.text, 0, N);
        strcat(msg.text, "help_success");
    }
    else 
    {
        strcat(msg.text, "help_fail");
    }

    SendMsg(sockfd, &msg);
}

