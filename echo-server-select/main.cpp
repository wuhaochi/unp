//
//  main.cpp
//  echo-server-select
//
//  Created by Ｈaochi Ｗu on 5/25/13.
//  Copyright (c) 2013 MyOrganizationName. All rights reserved.
//

#include <iostream>
#include "unp.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "glog/logging.h"

using namespace std;
int main(int argc, const char * argv[])
{
    std::cout << "echo server implemented using select" << endl;
    
    FLAGS_logtostderr = 1;
    google::InitGoogleLogging("using-select-lab");
    
    int listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    LOG_IF(FATAL, listen_socket_fd < 0) << "listen socket creation fail." << endl;
    
    int on = 1;
    setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    sockaddr_in server_addr, client_addr;
    bzero(&server_addr, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(ECHO_SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int fcode = 0;
    
    fcode = ::bind(listen_socket_fd, SA(&server_addr), sizeof(sockaddr_in));
    LOG_IF(ERROR, fcode != 0) << "bind to port " << ECHO_SERVER_PORT << " fail." << strerror(errno) << endl;
    
    fcode = listen(listen_socket_fd, LISTENQ_BACKLOG);
    LOG_IF(ERROR, fcode < -1) << "listen to port " << ECHO_SERVER_PORT << " fail." <<  strerror(errno) << endl;
    
    int maxfd = listen_socket_fd;
    int max_index = -1;
    vector<int> client(FD_SETSIZE);
    client.reserve(FD_SETSIZE);
    for( int i = 0; i < FD_SETSIZE; ++i)
    {
        client[i] = -1;
    }
    
    fd_set allset;
    FD_ZERO(&allset);
    FD_SET(listen_socket_fd, &allset);    
    for(;;)
    {
        fd_set rset = allset;
        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if(nready <= 0 && errno == EINTR)
        {
            LOG_IF(WARNING, errno == EINTR) << "select interrupted." << endl;
			continue;
        }
		LOG_IF(ERROR, nready <= 0) << "select error, " << strerror(errno) << endl;
        
        if( FD_ISSET(listen_socket_fd, &rset))
        {
           // new client connection comes in
            FD_CLR(listen_socket_fd, &rset);
            socklen_t client_addr_len = sizeof(sockaddr_in);
			int connfd = accept(listen_socket_fd, SA(&client_addr), &client_addr_len);
            LOG_IF(ERROR, connfd < 0) << "accept fail." << strerror(errno) << endl;
            typedef __typeof__ client iter;
            iter:: iterator pos = find_if(client.begin(), client.end(), bind2nd(equal_to<int>(), -1));
            LOG_IF(ERROR, pos == client.end() ) << "too many clients" << endl;
        	if( pos == client.end())
            {
                continue;
            }
            *pos = connfd;
            FD_SET(connfd, &allset);
            maxfd = max(connfd, maxfd);
            max_index = max(max_index, (int)distance(client.begin(), pos));
            if( --nready){
                continue; // no more readable descriptor
            }
        }
        
            // nready >= 1
            // loop client connection descriptors.
        for( int i = 0; i <= max_index; ++i)
        {
            ssize_t nwritten = -1;
            ssize_t nread = -1;
            char buf[MAXLINE];
            int connfd = client[i];
            
            if(connfd < 0)
            {
                continue;
            }
            if(! FD_ISSET(connfd, &rset))
            {
            	continue;
            }
            nread = read(connfd, buf, MAXLINE);
            if( nread == 0) // connection closed by client
            {
                if(errno != EINTR)
                {
                    LOG(ERROR) << "read from client error " << strerror(errno) << endl;
                }
                else{
                    LOG(INFO) << "read from client interrupted." << endl;
                }
                close(connfd);
                FD_CLR(connfd, &rset);
                client[i] = -1;
                continue;
            }
            nwritten = write(connfd, buf, nread);
            LOG_IF(WARNING, nwritten != nread) << "write " << nwritten << " chars, " << nread - nwritten << " chars left," << strerror(errno)<< endl;
            if(--nready <= 0)
            {
                break; // no more readable descriptors
            }
        }
        
    }
    LOG(INFO) << "program " << argv[0] << " exit." << endl;
	return 0;
}

