#include "communication.hpp"

#include "../config/def_comm.hpp"

#include <cstdio>
#include <cstring>
#include <cassert>
#include <unistd.h>

Communication::Communication(Data& data) : data(data)
{
    socketId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    assert (socketId >= 0);

    memset((char *) &socketAddr, 0, sizeof(socketAddr));
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(SERVER_PORT);

    if (inet_aton(SERVER_IP, &socketAddr.sin_addr) == 0) {
        fprintf(stderr, "inet_aton() failed\n");
        assert(false);
    }
}

Communication::~Communication()
{
    close(socketId);
}


void Communication::sendMessage()
{
    bool sendMessage = false;
    int bufferLen = snprintf(buffer, BUFFLEN-1, "%d", CAMERA_ID);
    for (size_t i=0; i<data.pm.size(); ++i) {
        PositionMarker &pm = data.pm[i];
        if (pm.hasBeenFound()) {
            float angle;
            int distance;
            pm.toWorld(&angle, &distance);

            int result = snprintf(buffer + bufferLen, BUFFLEN-1-bufferLen, " %d %f %d %f",
                                  pm.pmID, angle, distance, pm.confidence);
            if (result < 0) {
                fprintf(stderr, "Communication::sendMessage snprintf() failed\n");
            }
            bufferLen += result;
            sendMessage = true;
        }
    }

    if (sendMessage) {
        buffer[BUFFLEN-1] = '\0';
        //send the message
        std::cout << "SEND (id=" << data.image.id << "): " << buffer << "\n";
        if (sendto(socketId, buffer, strlen(buffer) , 0 , (struct sockaddr *) &socketAddr, sizeof(socketAddr)) == -1) {
            fprintf(stderr, "Communication::sendMessage sendTo() failed\n");
        }
    }
}


void Communication::update(void) {
    sendMessage();
}
