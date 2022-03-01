#include "User.h"

void do_regist(int sockfd, MSG msg, sqlite3 *db)
{
    char sqlcmd[N];
    char date[N] = "";
    int ret;

    getdate(date);
    sprintf(sqlcmd, "insert into user values('%s', '%s', '%s')", msg.Account, msg.text, date);

    memset(msg.text, 0, N);
    if((ret = do_sqlinsert(db, sqlcmd)) == 0)
    {
        strcat(msg.text, "regist_success");
        printf(FONT_COLOR_YELLOW"<user_success>\n"COLOR_NONE);
    }
    else if(ret == 1)
    {
        strcat(msg.text, "user_exist");
        printf(FONT_COLOR_YELLOW"<user_exist>\n"COLOR_NONE);
    }
    else
    {
        strcat(msg.text, "regist_fail");
        printf(FONT_COLOR_RED"<regist_fail>\n"COLOR_NONE);
    }

    SendMsg(sockfd, &msg);
    //memset(&msg, 0, sizeof(MSG));
}

void do_login(int sockfd, MSG msg, sqlite3 *db)
{
    int ret;
    char sqlcmd[N];
    sprintf(sqlcmd, "select * from user where account = '%s' and passwd = '%s'", msg.Account, msg.text);

    memset(msg.text, 0, N);
    if((ret = do_check_key(db, msg, sqlcmd)) == 0)
    {
        strcat(msg.text, "login_success");
        printf(FONT_COLOR_YELLOW"<login_success>\n"COLOR_NONE);
    }
    else if(ret == 1)
    {
        strcat(msg.text, "Ac_or_Pd_error");
        printf(FONT_COLOR_YELLOW"<Ac_or_Pd_error>\n"COLOR_NONE);
    }
    else
    {
        strcat(msg.text, "login_fail");
        printf(FONT_COLOR_YELLOW"<login_fail>\n"COLOR_NONE);
    }
    
    SendMsg(sockfd, &msg);
}

void do_manage_login(int sockfd, MSG msg, sqlite3 *db)
{
    int ret;
    char sqlcmd[N];
    sprintf(sqlcmd, "select * from manager where account = '%s' and passwd = '%s'", msg.Account, msg.text);

    memset(msg.text, 0, N);
    if((ret = do_check_key(db, msg, sqlcmd)) == 0)
    {
        strcat(msg.text, "manager_login_success");
        printf(FONT_COLOR_YELLOW"<manager_login_success>\n"COLOR_NONE);
    }
    else if(ret == 1)
    {
        strcat(msg.text, "Ac_or_Pd_error");
        printf(FONT_COLOR_YELLOW"<Ac_or_Pd_error>\n"COLOR_NONE);
    }
    else
    {
        strcat(msg.text, "manager_login_fail");
        printf(FONT_COLOR_YELLOW"<manager_login_fail>\n"COLOR_NONE);
    }
    
    SendMsg(sockfd, &msg);
}

void book_search(int sockfd, MSG msg, sqlite3 *db, char *sqlcmd)
{
    //char sqlcmd[N];
    //使用模糊搜索，找出任何位置包含该字符串的记录，这里的%需要用%转义
    //sprintf(sqlcmd, "select * from book where bookname like '%%%s%%'", msg.text);
    do_search(sockfd, db, msg, sqlcmd);
}

void book_borrow(int sockfd, MSG msg, sqlite3 *book_db, sqlite3 *user_db, char *sqlcmd)
{
    do_borrow(sockfd, msg, book_db, user_db, sqlcmd);
}

void book_return(int sockfd, MSG msg, sqlite3 *book_db, sqlite3 *user_db, char *sqlcmd)
{
    do_return(sockfd, msg, book_db, user_db, sqlcmd);
}

void book_add(int sockfd, MSG msg, sqlite3 *db, char *sqlcmd)
{
    do_add(sockfd, msg, db, sqlcmd);
}

void book_del(int sockfd, MSG msg, sqlite3 *db, char *sqlcmd)
{
    do_del(sockfd, msg, db, sqlcmd);
}

void book_help(int sockfd, MSG msg, sqlite3 *book_db, sqlite3 *user_db, char *sqlcmd)
{
    do_help(sockfd, msg, book_db, user_db, sqlcmd);
}
