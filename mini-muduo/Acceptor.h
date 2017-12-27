#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "Declear.h"
#include "Define.h"
#include "IChannelCallback.h"

class Acceptor : public IChannelCallback
{
public:
    Acceptor(EventLoop *loop);
    ~Acceptor();
    
    void start();
    void setCallback(IAcceptorCallback * pCallback);
    void virtual run(int sockfd);
private:
    int createAndListen();
    int _listenfd;
    Channel* _pAcceptChannel;
    IAcceptorCallback* _pCallback;
    EventLoop* _loop;
};

#endif
