//
//  daytime.cpp
//  unp
//
//  Created by Ｈaochi Ｗu on 5/4/13.
//  Copyright (c) 2013 MyOrganizationName. All rights reserved.
//

#include "daytime.h"
#include "unp.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAXLINE 1024

int daytime(int argc, const char** argv){
    int sockfd, n;
    char recvline[ MAXLINE + 1];
    struct sockaddr_in serveaddr;
    
    if( argc != 2){
        err_quit("usage: a.out <ipaddress>");
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    	err_quit("socket error");
    
    bzero(&serveaddr, sizeof(sockaddr_in));
    
    serveaddr.sin_family = AF_INET;
    serveaddr.sin_port = htons(13); // daytime server
    
    if( inet_pton(AF_INET, argv[1], &serveaddr.sin_addr) < 0)
        err_quit("inet_pton error for %s", argv[1]);
    
}