//
//  main.cpp
//  echo-client
//
//  Created by Ｈaochi Ｗu on 5/13/13.
//  Copyright (c) 2013 MyOrganizationName. All rights reserved.
//

#include <iostream>
#include "unp.h"
#include <unistd.h>
#include <sys/types.h>
#include "netinet/in.h"
#include "arpa/inet.h"

using namespace std;

void sighan(int signo)
{
    cout << "catch signal: " << signo << endl;
    cout.flush();
}

void str_cli(FILE* fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];
    while(true)
    {
        char* user_input = fgets(sendline, MAXLINE, fp);
        if( user_input == NULL)
        {
            if (! feof(fp)) {
                cout << "get user input error, and is not EOF " <<  strerror(errno) << endl;
            }
            else
            {
                cout << "user input EOF" << endl;
            }
            break;
        }
        int sendlen = strlen(sendline);
        int nwrite = 0;
        while (nwrite < sendlen) {
        	int n = write(sockfd, sendline + nwrite, sendlen - nwrite);
            if( n <= 0)
            {
                if (n < 0 && errno == EINTR) {
                    n = 0;
                }
                else{
                    cerr << "server terminated permanently: " << strerror(errno) << endl;
                    break; // fail to write out, give up
                }
            }
            nwrite += n;            
        }
        if(nwrite == sendlen)
        {
                // write to socket ok
        }
        else{
            cerr << "write to server fail" << endl;
        }
        ssize_t n = read(sockfd, recvline, MAXLINE);//it's incorrect, but for small packet size,
                                                    // i believe it can be read in one call
        if (n > 0) {
            recvline[n] = 0;
			cout << recvline ;
            cout.flush();
        }
    }
}

int main(int argc, const char * argv[])
{
    cout << "echo client" << endl;
    if( argc <= 1)
    {
        cerr << "usage echo-client ip [port]" << endl;
        exit(0);
    }
    
	int connfd;
    sockaddr_in servaddr, cliaddr;
    socklen_t serv_len, cli_len;
    serv_len = cli_len = sizeof(sockaddr_in);
    int fcode = 0;
    servaddr.sin_family = AF_INET;
    if(argc == 2)
    {
    	servaddr.sin_port = htons(ECHO_SERVER_PORT);
    }
    else if (argc ==3)
    {
        servaddr.sin_port = htons(atoi(argv[2]));
    }
    fcode =  inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr);
    if(fcode == 0)
    {
        cerr << "bad ip address: " << argv[1] << endl;
        return 0;
    }
    else if(fcode == -1){
        err_quit("inet_pton error: %s", strerror(errno));
    }
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    if(connfd <= 0)
    {
        err_quit("create socket fail: %s", strerror(errno));
    }
    for(int i = 1; i < 32; ++i)
        signal(i, sighan);
    
    cout << "connecting ..." << endl;
    fcode = connect(connfd, SA(&servaddr), serv_len);
    cout << "connect return" << endl;
    if( fcode == EINTR)
    {
        cout << "connec call interrupted" << endl;
        cout.flush();
        return 0;
    }
    else if( fcode != 0)
    {
        cout << "connect error: " << strerror(errno) << endl;
    }
    cout << "setting socket option: SO_LINGER" << endl;
	struct linger linger;
    memset(&linger, 0, sizeof(struct linger));
    linger.l_onoff = true;
    errno = 0;
    if(setsockopt(connfd, SOL_SOCKET, SO_LINGER, &linger, sizeof(struct linger)) )
    {
        cerr << "setsockopt error: " << strerror(errno) << endl;
    }
    else{
        cout << "socket option SO_LINGER set" << endl;
    }
        // shutdown(connfd, SHUT_WR);
    getsockname(connfd, SA(&cliaddr), &cli_len);
    const socklen_t cliaddr_buflen = 1024;
    char cliaddr_buf[cliaddr_buflen];
	if( NULL == inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, cliaddr_buf, cliaddr_buflen))
    {
        err_quit("inet_ntop error: %s", strerror(errno));
    }
    
    cout << "client address is: " <<  cliaddr_buf << endl;
    str_cli(stdin, connfd);
    exit(0);
    return 0;
}

