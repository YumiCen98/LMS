#ifndef __USER_H_
#define __USER_H_

#include <string.h>
#include "LMSqlite.h"
#include "Connect.h"

void do_regist(int sockfd, MSG msg, sqlite3 *db);

void do_login(int sockfd, MSG msg, sqlite3 *db);

void do_manage_login(int sockfd, MSG msg, sqlite3 *db);

void book_search(int sockfd, MSG msg, sqlite3 *db, char *sqlcmd);

void book_borrow(int sockfd, MSG msg, sqlite3 *book_db, sqlite3 *user_db, char *sqlcmd);

void book_return(int sockfd, MSG msg, sqlite3 *book_db, sqlite3 *user_db, char *sqlcmd);

void book_add(int sockfd, MSG msg, sqlite3 *db, char *sqlcmd);

void book_del(int sockfd, MSG msg, sqlite3 *db, char *sqlcmd);

void book_help(int sockfd, MSG msg, sqlite3 *book_db, sqlite3 *user_db, char *sqlcmd);

#endif