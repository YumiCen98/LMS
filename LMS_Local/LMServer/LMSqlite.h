#ifndef __LMSQLITE_H_
#define __LMSQLITE_H_

#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "Connect.h"

#define USERDBNAME "user.db"
#define BOOKDBNAME "book.db"

//定义颜色代码
#define COLOR_NONE          "\033[0m"       //恢复默认
#define FONT_COLOR_YELLOW   "\033[1;33m"    //黄色高亮
#define FONT_COLOR_RED      "\033[1;31m"    //红色高亮

//获取当前时间
void getdate(char data[]);

//打开/创建数据库
sqlite3 *do_sqlopen(const char *dbname, sqlite3 *db);

//创建表
void do_createtable(sqlite3 *db, const char *sqlcmd);

//插入表
int do_sqlinsert(sqlite3 *db, const char *sqlcmd);

//检查记录是否存在
int do_check_key(sqlite3 *db, MSG msg, const char *sqlcmd);

//更新数据库
void sql_update(sqlite3 *db, const char *sqlcmd);

//搜索查询回调函数
int search_callback(void *arg, int f_num, char **f_value, char **f_name);
//搜索
void do_search(int sockfd, sqlite3 *db, MSG msg, const char *sqlcmd);

//借阅查询回调函数
int borrow_callback(void *arg, int f_num, char **f_value, char **f_name);
//借阅
void do_borrow(int sockfd, MSG msg, sqlite3 *book_db, sqlite3 *user_db, const char *sqlcmd);

//归还查询回调函数
int return_callback(void *arg, int f_num, char **f_value, char **f_name);
//归还
void do_return(int sockfd, MSG msg, sqlite3 *book_db, sqlite3 *user_db, const char *sqlcmd);

//添加图书
void do_add(int sockfd, MSG msg, sqlite3 *db, char *sqlcmd);

//删除图书
void do_del(int sockfd, MSG msg, sqlite3 *db, char *sqlcmd);

//代还图书
void do_help(int sockfd, MSG msg, sqlite3 *book_db, sqlite3 *user_db, char *sqlcmd);

#endif