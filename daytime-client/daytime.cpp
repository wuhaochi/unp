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
    struct sockaddr_in servaddr;
    
    if( argc != 2){
        err_quit("usage: daytime-client <ipaddress>  (ipv4)");
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    	err_quit("socket error");
    
    bzero(&servaddr, sizeof(sockaddr_in));
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13); // daytime server
    
    int status = inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr);
    if( status <= 0)
        err_quit("inet_pton error for %s", argv[1]);
    
    status = connect(sockfd, SA(&servaddr), sizeof(sockaddr_in));
    
    if( status < 0)
    {
        err_quit("connect error");
    }
    
    while( (n = read(sockfd, recvline, MAXLINE)) > 0){
        recvline[n] = 0;
        if( fputs(recvline, stdout) == EOF){
            err_quit("fputs error");
        }
        fputs("\n", stdout);
    }
    if( n < 0)
    {
        err_quit("read error");
    }
    fputs("exit", stdout);
    return 0;
    
}

int daytime_v6(int argc, const char** argv){
    int rc = 0; // function return code
    int sockfd, n;
    char recvline[MAXLINE + 1];
    struct sockaddr_in6 servaddr;
    
    if( argc != 2){
        err_quit("useage unpv6 <ipaddress>");
    }
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if( sockfd < 0)
        err_quit("socket error");
    
    bzero(&servaddr, sizeof(sockaddr_in6));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port = htons(13);
    rc = inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr);
    if( rc <= 0)
    {
        err_quit("inet_pton error for %s, return code %d", argv[1], rc);
    }
    rc = connect(AF_INET6, SA(&servaddr), sizeof(sockaddr_in6));
    if( rc < 0)
    {
        err_quit("connect error");
    }
    
    while ( (n = read(sockfd, recvline, MAXLINE)) > 0)
    {
        recvline[n] = 0;
        if( fputs(recvline, stdout) == EOF)
            err_quit("fputs error");
    }
    if (n < 0) {
        err_quit("read error");
    }
    
	return 0;
}