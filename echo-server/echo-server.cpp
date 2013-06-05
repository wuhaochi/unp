//
//  main.cpp
//  echo-server
//
//  Created by Ｈaochi Ｗu on 5/12/13.
//  Copyright (c) 2013 MyOrganizationName. All rights reserved.
//

    // unix header
#include "unistd.h"
#include "sys/socket.h"
#include "netinet/in.h"

    //C header

#include <signal.h>

    //C++ header
#include <iostream>

    //User header
#include "unp.h"

const int LISTENQ = 10 * (1<<10); // 10k

using namespace std;

//
//typedef void sighand(ing signo);
//there is a standard sig_t for signal handler function

void signal_handler(int signo)
{
	pid_t pid;
    int stat;
    printf("catch signal %d", signo);
	pid = waitpid(-1, &stat, WNOHANG);
    if( pid > 0)
    {
        printf("child %d terminated, status is %d\n", pid, stat);
    }
    else if( pid == 0)
    {
            // ok, nothing happen
    }
    else if (pid == -1)
    {
        printf(" waitpid error, %s", strerror(errno));
    }
    else{
        printf("wait pid error, but I don't know what's wrong.");
    }
    
	if(EOF ==fflush(stdout))
    {
        printf("flush stdout fail: %s", strerror(errno));
    }
}

void str_echo(int fd){
	ssize_t n;
    char buf[MAXLINE];
again:
    while ( (n = read(fd, buf, MAXLINE)) > 0) {
        if( n == 1) // is command, since no buffer, like \n c-d c-c
        {
            if (buf[0] == 4) // ctrl d
            {
                cerr << "client ask for close" << endl;
                return;
            }
        }
        ssize_t nwritten = write(fd, buf, n);
        cout << "read " << n << "bytes" << endl;
        if( nwritten < 0 && errno == EINTR)
        {
            cerr << "write to client is interrupted." << endl;
            goto again;
        }
        else if (nwritten == n)
        {
            buf[n] = 0;
            cout << "echo success: " << buf << endl;
        }
        else
        {
            err_quit("%s:%d: write error: %s", __FUNCTION__, __LINE__, strerror(errno)); // error
        }
    }
    cout << "chid process: " << getpid() << " is exiting" << endl;
}


int main(int argc, const char * argv[])
{
    std::cout << "echo server\n";
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd == -1)
    {
        err_quit("create listen socket fail");
    }
    bzero(&servaddr, sizeof(sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(ECHO_SERVER_PORT);
    
    if(-1 == ::bind(listenfd, SA(&servaddr), sizeof(sockaddr_in))){
        err_quit("bind error: %s", strerror(errno));
    }
    
    if( listen(listenfd, LISTENQ) == -1)
    {
    	err_quit("listen socket error: %s", strerror(errno));
    }

    sig_t old_signal_handler = signal(SIGCHLD, signal_handler);

    for (;;) {
        clilen = sizeof(cliaddr);
            //  sleep(180);
        connfd = accept(listenfd, SA(&cliaddr), &clilen);
        if( (childpid = fork()) == 0)//child process
        {
            cout << "create new child " << getpid() << endl;
            close(listenfd);
            str_echo(connfd);
            cout << "child exit" << endl;
            exit(0);
        }
        close(connfd);        
    }
    return 0;
}

