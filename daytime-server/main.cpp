//
//  main.cpp
//  daytime-server
//
//  Created by Ｈaochi Ｗu on 5/6/13.
//  Copyright (c) 2013 MyOrganizationName. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "unp.h"

using std::cout;
using std::endl;
using std::cerr;

const int Q = 5;
int daytime_server(int argc, const char** argv){
	int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    time_t ticks;
    ssize_t n = 0;
    int status = 0;
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int v = 1;
    status = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(int));
    if(status < 0)
    {
        err_quit(" setsockopt fail: %s", strerror(errno));
    }

    bzero(&servaddr, sizeof(sockaddr_in));
	servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);
 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
// if bind to a specific ip, just fill s_addr with that ip.
//    if(! inet_pton(AF_INET, "192.168.1.111", &servaddr.sin_addr.s_addr))
//    {
//        err_quit("inet_pton(192.168.1.111) failed");
//    }


        // without the '::', it will conflict with STL bind
    status = ::bind(listenfd, SA(&servaddr), sizeof(sockaddr_in));
    if(status < 0)
    {
        err_quit("bind fail: %s", strerror(errno));
    }
    status = listen(listenfd, Q);
    if(status < 0)
    {
        err_quit(strerror(errno));
    }
    
    for(int client_id = 0;;++client_id){
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(sockaddr_in);
        char addrbuf[ INET_ADDRSTRLEN];
     
        connfd = accept(listenfd, SA(&client_addr), &client_addr_len);
        if(connfd < 0)
        {
        	err_quit("accept error, %s", strerror(errno));
        }
        cout << client_id << "  comes in client " << inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, addrbuf, sizeof(client_addr)) << ":" << ntohs(client_addr.sin_port) << endl;
        ticks = time(NULL);
        bzero(buff, MAXLINE);
        snprintf(buff, sizeof(buff), "%.24s\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));
        n = recv(connfd, buff, MAXLINE, 0);
        if(n <=0)
        {
        	cerr << "receive nothing for client return " << n << endl;
        }
        else{
                //buff[n] = 0;
                //cout << "receive " << buff << endl;
        }
        close(connfd);
    }
}


int main(int argc, const char * argv[])
{
	std::cout << "this is daytime server" << endl;
    daytime_server(argc, argv);
    return 0;
}

