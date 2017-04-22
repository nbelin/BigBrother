#ifndef COMMUNCATION_HPP
#define COMMUNCATION_HPP

#include <arpa/inet.h>
#include <sys/socket.h>

#include "marker.hpp"

#include "data.hpp"

class Communication {
public:
    Communication(Data& data);
    virtual ~Communication();
    void sendMessage();
public:
    void update(void);
private:
    int socketId;
    struct sockaddr_in socketAddr;
    static const int BUFFLEN = 1024;
    char buffer [BUFFLEN];
    Data& data;
};

#endif //COMMUNCATION_HPP
