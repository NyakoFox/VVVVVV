#include <enet/enet.h>
#include <string>
#include <SDL.h>

#include "Player.h"

#include "Packet.h"

Player::Player(ENetPeer* peer, std::string name, int connection_type, std::string uuid)
{
    this->peer = peer;
    this->name = name;
    this->connection_type = connection_type;
    this->uuid = uuid;

    room_x = -1;
    room_y = -1;
    x = -1;
    y = -1;

    ax = 0;
    ay = 0;

    dir = 0;

    invis = 0;
    deathseq = -1;
    gravity = 0;
    colour = 0;
}

Player::Player(const Player& other)
{
    // copy constructor
    peer = other.peer;
    name = other.name;
    connection_type = other.connection_type;
    uuid = other.uuid;

    room_x = other.room_x;
    room_y = other.room_y;
    x = other.x;
    y = other.y;

    ax = other.ax;
    ay = other.ay;

    dir = other.dir;

    invis = other.invis;
    deathseq = other.deathseq;
    gravity = other.gravity;
    colour = other.colour;
}

Player::~Player(void)
{
}

Player& Player::operator=(const Player& other)
{
    peer = other.peer;
    name = other.name;
    connection_type = other.connection_type;
    uuid = other.uuid;

    room_x = other.room_x;
    room_y = other.room_y;
    x = other.x;
    y = other.y;

    ax = other.ax;
    ay = other.ay;
    vx = other.vx;
    vy = other.vy;

    dir = other.dir;

    invis = other.invis;
    deathseq = other.deathseq;
    gravity = other.gravity;
    colour = other.colour;
    return *this;
}

void Player::send(Packet* packet)
{
    packet->send(peer);
}
