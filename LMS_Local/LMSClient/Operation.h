#ifndef __OPERATION_H_
#define __OPERATION_H_

#include "Connect.h"

void book_search(int sockfd, MSG msg);

void book_borrow(int sockfd, MSG msg);

void book_return(int sockfd, MSG msg);

void book_add(int sockfd, MSG msg);

void book_del(int sockfd, MSG msg);

void book_help(int sockfd, MSG msg);

#endif