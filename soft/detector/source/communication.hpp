#ifndef COMMUNCATION_HPP
#define COMMUNCATION_HPP

#include <arpa/inet.h>
#include <sys/socket.h>

#include "marker.hpp"

class Communication {
public:
    Communication(const int cameraId, const char * const serverIp, const short serverPort);
    virtual ~Communication();
    void prepareMessage(const PositionMarker * pm);
    void sendMessage();
    void resetMessage();
private:
    int cameraId;
    int socketId;
    struct sockaddr_in socketAddr;
    static const int BUFFLEN = 1024;
    char buffer [BUFFLEN];
    int nbMarkers;
    const PositionMarker* posMarkers[4];
};

#endif //COMMUNCATION_HPP
