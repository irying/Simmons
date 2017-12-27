#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>

#include "Acceptor.h"
#include "Channel.h"
#include "IAcceptorCallback.h"
#include "EventLoop.h"

using namespace std;

Acceptor::Acceptor(EventLoop* loop)
:_listenfd(-1)
,_pAcceptChannel(NULL)
,_pCallback(NULL)
,_loop(loop)
{}

Acceptor::~Acceptor()
{}

void Acceptor::start()
{
    _listenfd = createAndListen();
    _pAcceptChannel = new Channel(_loop, _listenfd);
    _pAcceptChannel->setCallback(this);
    _pAcceptChannel->enableReading();
}

int Acceptor::createAndListen()
{
    int on = 1;
    _listenfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    fcntl(_listenfd, F_SETEL, O_NONBLOCK);
    setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    servaddr.sin_family = PF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(11111);
    
    if (-1 == bind(_listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) {
        cout<<"bind error, errno:"<<errno<<enl;
    }
    
    if(-1 == listen(_listenfd, MAX_LISTENFD))
    {
        cout << "listen error, errno:" << errno << endl;
    }
    
    return _listenfd;
}

void Acceptor::run(int socket)
{
    int connfd;
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(struct sockaddr_in);
    connfd = accept(_listenfd, (sockaddr*)&cliaddr, (socklen_t*)&clilen);
    if(connfd > 0)
    {
        cout << "new connection from "
        << "[" << inet_ntoa(cliaddr.sin_addr)
        << ":" << ntohs(cliaddr.sin_port) << "]"
        << " new socket fd:" << connfd
        << endl;
    }
    else
    {
        cout << "accept error, connfd:" << connfd
        << " errno:" << errno << endl;
    }
    fcntl(connfd, F_SETFL, O_NONBLOCK);
    
    _pCallBack->newConnection(connfd);
}

void Acceptor::setCallback(IAcceptorCallback* pCallBack)
{
    _pCallBack = pCallBack;
}
