//
//  unp.h
//  unp
//
//  Created by Ｈaochi Ｗu on 5/4/13.
//  Copyright (c) 2013 MyOrganizationName. All rights reserved.
//

#ifndef unp_unp_h
#define unp_unp_h

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "stdlib.h"
#include <sys/types.h>

const int MAXLINE = 1204;
const int LISTENQ_BACKLOG = 1024;
const in_port_t ECHO_SERVER_PORT = 9877;

inline
void err_quit(const char* info, ...){
    va_list va;
    va_start(va, info);
	fprintf(stderr, info, va);
    fprintf(stderr, "\n");
    va_end(va);
    exit(-1);
}

struct sockaddr;
template<typename T>
sockaddr* SA(T paddr)
{
	return reinterpret_cast<sockaddr*>(paddr);
}


#endif
