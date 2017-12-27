#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "Declear.h"
#include "IChannelCallback.h"

class TcpConnection : public IChannelCallback
{
public:
    TcpConnection(int sockfd, EventLoop* loop);
    ~TcpConnection();
    
    void virtual run(int sockfd);
private:
    int _sockfd;
    Channel* _pChannel;
    EventLoop* _loop;
};

#endif
