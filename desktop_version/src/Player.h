#ifndef PLAYER_H
#define PLAYER_H

#include <enet/enet.h>

#include "Packet.h"

enum PlayerConnectionTypes
{
    CONNECTING,
    CONNECTED

};

class Player
{
public:
    ENetPeer* peer;
    std::string name;
    int connection_type;

    int x;
    int y;

    int room_x;
    int room_y;

    int dir;
    int invis;
    int deathseq;
    int gravity;
    int colour;

    float ax;
    float ay;
    float vx;
    float vy;

    std::string uuid;

    Player(ENetPeer* peer, std::string name, int connection_type, std::string uuid);
    Player(const Player& other);
    ~Player(void);

    Player& operator=(const Player& other);

    void send(Packet* packet);
};

#endif /* PLAYER_H */