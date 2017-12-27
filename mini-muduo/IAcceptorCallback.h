#ifndef IACCEPTORCALLBACK_H
#define IACCEPTORCALLBACK_H

class IAcceptorCallback
{
public:
    void virtual newConnection(int sockfd){};
};

#endif
