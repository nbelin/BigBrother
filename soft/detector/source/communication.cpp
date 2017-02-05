#include "communication.hpp"

#include <cstdio>
#include <cstring>
#include <cassert>
#include <unistd.h>

Communication::Communication(Data& data, Config& config) : cameraId(config.detector_id), nbMarkers(0), data(data)
{
    socketId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    assert (socketId >= 0);

    memset((char *) &socketAddr, 0, sizeof(socketAddr));
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(config.rdv_port);

    if (inet_aton(config.rdv_ip.c_str(), &socketAddr.sin_addr) == 0) {
        fprintf(stderr, "inet_aton() failed\n");
        assert(false);
    }
}

Communication::~Communication()
{
    close(socketId);
}

void Communication::prepareMessage(const PositionMarker * pm)
{
    if (nbMarkers > 3) {
        fprintf(stderr, "Communication::prepareMessage cannot prepare more than 4 PositionMarker\n");
    }
    posMarkers[nbMarkers++] = pm;
}

void Communication::sendMessage()
{
    if (nbMarkers == 0) {
        // don't send anything if there is no information to send!
        return;
    }
    int bufferLen = snprintf(buffer, BUFFLEN-1, "%d", cameraId);
    for (int i=0; i<nbMarkers; ++i) {
        int result = snprintf(buffer + bufferLen, BUFFLEN-1-bufferLen, " %d %f %f %f",
                              posMarkers[i]->pmID, (float)posMarkers[i]->x, (float)posMarkers[i]->size, posMarkers[i]->confidence);
        if (result < 0) {
            fprintf(stderr, "Communication::sendMessage snprintf() failed\n");
        }
        bufferLen += result;
    }
    buffer[BUFFLEN-1] = '\0';

    //send the message
    if (sendto(socketId, buffer, strlen(buffer) , 0 , (struct sockaddr *) &socketAddr, sizeof(socketAddr)) == -1) {
        fprintf(stderr, "Communication::sendMessage sendTo() failed\n");
    }
}

void Communication::resetMessage()
{
    nbMarkers = 0;
}

void Communication::update(void) {
    sendMessage();
    resetMessage();
}
