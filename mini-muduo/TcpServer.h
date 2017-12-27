#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <sys/epoll.h>
#include "Declear.h"
#include "Define.h"
#include "IAcceptorCallback.h"
#include <map>

using namespace std;

class TcpServer : public IAcceptorCallback
{
public:
    TcpServer(EventLoop* loop);
    ~TcpServer();
    void start();
    virtual void newConnection(int sockfd);
private:
    struct epoll_event _events[MAX_EVENTS];
    map<int, TcpConnection*> _connections;
    Acceptor* _pAcceptor;
    EventLoop* _loop;
};


#endif
