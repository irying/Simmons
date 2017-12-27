#ifndef ICHANNELCALLBACK_H
#define ICHANNELCALLBACK_H

class IChannelCallback
{
public:
    void virtual run(int sockfd){};
};

#endif
