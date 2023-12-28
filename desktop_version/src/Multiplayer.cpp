#include <enet/enet.h>
#include <string>
#include <vector>

#include <SDL.h>

#include "Multiplayer.h"

#include "Entity.h"
#include "Game.h"
#include "Packet.h"
#include "Player.h"
#include "Script.h"
#include "Vlogging.h"

#include "UtilityClass.h"

// UUIDs...
#ifdef _WIN32
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000
#include <windows.h>
#include <iostream>
#else
#include <uuid/uuid.h>
#endif

namespace multiplayer
{
    bool server = false;
    std::string server_ip = "localhost";
    int server_port = 65432;

    ENetAddress server_address;
    ENetHost* host_instance;
    ENetPeer* peer;

    std::vector<Player> players;

    std::string last_uuid_hack;

    bool should_update_player = false;
    bool should_update_player_movement = false;

    std::string generate_uuid(void)
    {
#ifdef _WIN32
        UUID uuid;
        UuidCreate(&uuid);
        unsigned char* str;
        UuidToStringA(&uuid, &str);
        std::string uuid_str = (char*)str;
        RpcStringFreeA(&str);
        return uuid_str;
#else
        uuid_t uuid;
        uuid_generate_random(uuid);
        char s[37];
        uuid_unparse(uuid, s);
        return std::string(s);
#endif
    }

    void send_player_update_packet()
    {
        should_update_player = false;

        int i = obj.getplayer();
        if (INBOUNDS_VEC(i, obj.entities))
        {
            Packet packet = Packet("player_update", 0);
            packet.write_int(obj.entities[i].colour);
            packet.write_int(obj.entities[i].invis);
            packet.write_int(game.deathseq);

            multiplayer::send_to_server(&packet);
        }
    }

    void send_player_movement_packet()
    {
        should_update_player_movement = false;

        int i = obj.getplayer();
        if (INBOUNDS_VEC(i, obj.entities))
        {
            Packet packet = Packet("player_movement", 0);
            packet.write_int(obj.entities[i].xp);
            packet.write_int(obj.entities[i].yp);
            packet.write_int(game.roomx);
            packet.write_int(game.roomy);
            packet.write_int(game.gravitycontrol);
            packet.write_int(obj.entities[i].dir);
            packet.write_float(obj.entities[i].ax);
            packet.write_float(obj.entities[i].ay);
            packet.write_float(obj.entities[i].vx);
            packet.write_float(obj.entities[i].vy);

            multiplayer::send_to_server(&packet);
        }
    }

    void update_player_entities()
    {
        for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it)
        {
            if (it->peer != peer)
            {
                // Check if this player is in the room we're now in
                if (it->room_x == game.roomx && it->room_y == game.roomy)
                {
                    // Do they have an entity? If not, create one.
                    bool found = false;
                    for (int i = 0; i < obj.entities.size(); i++)
                    {
                        // if it's disabled, continue
                        if (obj.entities[i].uuid == it->uuid)
                        {
                            found = true;
                            obj.entities[i].xp = it->x;
                            obj.entities[i].yp = it->y;
                            obj.entities[i].dir = it->dir;
                            obj.entities[i].behave = it->gravity; // behave is our gravity attribute
                            obj.entities[i].colour = it->colour;
                            obj.entities[i].invis = it->invis;
                            obj.entities[i].para = it->deathseq; // para is our deathseq attribute
                            obj.entities[i].ax = it->ax;
                            obj.entities[i].ay = it->ay;
                            obj.entities[i].vx = it->vx;
                            obj.entities[i].vy = it->vy;
                            break;
                        }
                    }
                    if (!found)
                    {
                        // Ugh, VVVVVV STILL doesn't return the entity index when you create one.
                        last_uuid_hack = it->uuid;
                        obj.createentity(it->x, it->y, 999, it->invis, it->gravity, it->deathseq);

                        // Now, find the entity we just created
                        for (int i = 0; i < obj.entities.size(); i++)
                        {
                            if (obj.entities[i].uuid == last_uuid_hack)
                            {
                                obj.entities[i].colour = it->colour;
                                obj.entities[i].dir = it->dir;
                                obj.entities[i].ax = it->ax;
                                obj.entities[i].ay = it->ay;
                                obj.entities[i].vx = it->vx;
                                obj.entities[i].vy = it->vy;
                                break;
                            }
                        }

                        last_uuid_hack = "";
                    }
                }
                else
                {
                    // They're not. Do we have an entity for them? If so, remove them.
                    for (int i = 0; i < obj.entities.size(); i++)
                    {
                        if (obj.entities[i].uuid == it->uuid)
                        {
                            obj.disableentity(i);
                        }
                    }
                }
            }
        }
    }

    bool is_server(void)
    {
        return server;
    }

    void set_server(bool server)
    {
        multiplayer::server = server;
    }

    std::string get_server_ip(void)
    {
        return server_ip;
    }

    void set_server_ip(std::string server_ip)
    {
        multiplayer::server_ip = server_ip;
    }

    int get_server_port(void)
    {
        return server_port;
    }

    void set_server_port(int server_port)
    {
        multiplayer::server_port = server_port;
    }

    void update_host(void)
    {
        server_address.port = server_port;
        enet_address_set_host(&server_address, server_ip.c_str());
    }

    bool create_server(void)
    {
        vlog_info("Creating server, listening on %s:%d", server_ip.c_str(), server_port);

        host_instance = enet_host_create(&server_address /* the address to bind the server host to */,
            32      /* allow up to 32 clients and/or outgoing connections */,
            2      /* allow up to 2 channels to be used, 0 and 1 */,
            0      /* assume any amount of incoming bandwidth */,
            0      /* assume any amount of outgoing bandwidth */);

        return host_instance != NULL;
    }

    bool create_client(void)
    {
        host_instance = enet_host_create(NULL /* create a client host */,
            1 /* only allow 1 outgoing connection */,
            2 /* allow up 2 channels to be used, 0 and 1 */,
            0 /* 56K modem with 56 Kbps downstream bandwidth */,
            0 /* 56K modem with 14 Kbps upstream bandwidth */);
        return host_instance != NULL;
    }

    bool connect_to_server(void)
    {
        peer = enet_host_connect(host_instance, &server_address, 2, 0);
        if (peer == NULL)
        {
            vlog_info("No available peers for initiating an ENet connection.");
            return false;
        }

        ENetEvent event;
        if (enet_host_service(host_instance, &event, 5000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT)
        {
            vlog_info("Connection to %s:%d succeeded.", server_ip.c_str(), server_port);
            return true;
        }
        else
        {
            vlog_info("Connection to %s:%d failed.", server_ip.c_str(), server_port);
            enet_peer_reset(peer);
            return false;
        }
    }

    void update(void)
    {
        ENetEvent event;
        while (enet_host_service(host_instance, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                vlog_info("A new client connected from %x:%u.",
                    event.peer->address.host,
                    event.peer->address.port);

                /* Store any relevant client information here. */
                //event.peer->data = "Client information";

                // If we're the server, send the client the current game state
                if (is_server())
                {
                    Player player = Player(event.peer, "?", CONNECTING, "?");
                    players.push_back(player);

                    Packet packet = Packet("connection", ENET_PACKET_FLAG_RELIABLE);
                    packet.write_int(1); // version 1
                    packet.write_string("Test Server");
                    player.send(&packet);
                }

                break;
            case ENET_EVENT_TYPE_RECEIVE:
                // We got a packet, create our fancy packet class (which will free the packet when it's done)
                {
                    Packet packet = Packet(event.packet);
                    Player* player = NULL;
                    for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it)
                    {
                        if (it->peer == event.peer)
                        {
                            player = &(*it);
                            break;
                        }
                    }

                    if (is_server())
                    {
                        if (SDL_strcmp(packet.id, "message") == 0)
                        {
                            if (player == NULL)
                            {
                                vlog_info("Received message from unknown player.");
                                break;
                            }

                            vlog_info("[MESSAGE] <%s> %s", player->name.c_str(), packet.read_string().c_str());
                        }
                        else if (SDL_strcmp(packet.id, "client_info") == 0)
                        {
                            if (player == NULL)
                            {
                                vlog_info("Received client info from unknown player.");
                                break;
                            }

                            std::string name = packet.read_string();
                            std::string uuid = packet.read_string();

                            player->name = name;

                            // Check for conflicting UUIDs
                            bool conflicting = false;
                            for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it)
                            {
                                if (it->uuid == uuid)
                                {
                                    vlog_info("Player %s has a conflicting UUID with player %s.", name.c_str(), it->name.c_str());
                                    enet_peer_disconnect(event.peer, 0);
                                    conflicting = true;
                                    break;
                                }
                            }

                            if (conflicting)
                            {
                                break;
                            }

                            player->connection_type = CONNECTED;
                            player->uuid = uuid;

                            vlog_info("Received client info from %s, UUID for this session is %s", player->name.c_str(), player->uuid.c_str());


                            // Send a player_add packet to all players except the new player
                            for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it)
                            {
                                if (it->peer != event.peer)
                                {
                                    Packet packet = Packet("player_add", ENET_PACKET_FLAG_RELIABLE);
                                    packet.write_string(player->name);
                                    packet.write_string(player->uuid);
                                    packet.write_int(player->room_x);
                                    packet.write_int(player->room_y);
                                    packet.write_int(player->x);
                                    packet.write_int(player->y);
                                    packet.write_int(player->ax);
                                    packet.write_int(player->ay);
                                    packet.write_int(player->vx);
                                    packet.write_int(player->vy);
                                    packet.write_int(player->gravity);
                                    packet.write_int(player->invis);
                                    packet.write_int(player->deathseq);
                                    packet.write_int(player->colour);
                                    packet.send(it->peer);
                                }
                            }

                            // Send a player_add packet to the new player for all players, except the new player
                            for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it)
                            {
                                if (it->peer != event.peer)
                                {
                                    Packet packet = Packet("player_add", ENET_PACKET_FLAG_RELIABLE);
                                    packet.write_string(it->name);
                                    packet.write_string(it->uuid);
                                    packet.write_int(it->room_x);
                                    packet.write_int(it->room_y);
                                    packet.write_int(it->x);
                                    packet.write_int(it->y);
                                    packet.write_int(it->ax);
                                    packet.write_int(it->ay);
                                    packet.write_int(it->vx);
                                    packet.write_int(it->vy);
                                    packet.write_int(it->gravity);
                                    packet.write_int(it->invis);
                                    packet.write_int(it->deathseq);
                                    packet.write_int(it->colour);
                                    packet.send(event.peer);
                                }
                            }
                        }
                        else if (SDL_strcmp(packet.id, "player_movement") == 0)
                        {
                            if (player == NULL)
                            {
                                vlog_info("Received player movement from unknown player.");
                                break;
                            }

                            player->x = packet.read_int();
                            player->y = packet.read_int();
                            player->room_x = packet.read_int();
                            player->room_y = packet.read_int();
                            player->gravity = packet.read_int();
                            player->dir = packet.read_int();
                            player->ax = packet.read_float();
                            player->ay = packet.read_float();
                            player->vx = packet.read_float();
                            player->vy = packet.read_float();

                            vlog_info("Received player movement from %s.", player->name.c_str());

                            // Tell all players about the player's movement

                            for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it)
                            {
                                if (it->peer != event.peer)
                                {
                                    Packet packet = Packet("player_movement", 0);
                                    packet.write_string(player->uuid);
                                    packet.write_int(player->x);
                                    packet.write_int(player->y);
                                    packet.write_int(player->room_x);
                                    packet.write_int(player->room_y);
                                    packet.write_int(player->gravity);
                                    packet.write_int(player->dir);
                                    packet.write_float(player->ax);
                                    packet.write_float(player->ay);
                                    packet.write_float(player->vx);
                                    packet.write_float(player->vy);
                                    packet.send(it->peer);
                                }
                            }
                        }
                        else if (SDL_strcmp(packet.id, "player_update") == 0)
                        {
                            if (player == NULL)
                            {
                                vlog_info("Received player update from unknown player.");
                                break;
                            }

                            player->colour = packet.read_int();
                            player->invis = packet.read_int();
                            player->deathseq = packet.read_int();

                            vlog_info("Received player update from %s.", player->name.c_str());

                            // Tell all players about the player's update

                            for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it)
                            {
                                if (it->peer != event.peer)
                                {
                                    Packet packet = Packet("player_update", 0);
                                    packet.write_string(player->uuid);
                                    packet.write_int(player->colour);
                                    packet.write_int(player->invis);
                                    packet.write_int(player->deathseq);
                                    packet.send(it->peer);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (SDL_strcmp(packet.id, "connection") == 0)
                        {
                            if (packet.read_int() != 1)
                            {
                                vlog_info("Client version mismatch.");
                                enet_peer_disconnect(event.peer, 0);
                                break;
                            }
                            else
                            {
                                vlog_info("Client version match.");
                            }

                            vlog_info("Connected to server %s.", packet.read_string().c_str());

                            // We got server information, so send client information!

                            Packet packet = Packet("client_info", ENET_PACKET_FLAG_RELIABLE);
                            packet.write_string("Ally");
                            packet.write_string(generate_uuid());
                            packet.send(event.peer);

                            //script.startgamemode(Start_SERVER);
                        }
                        else if (SDL_strcmp(packet.id, "message") == 0)
                        {
                            vlog_info("[MESSAGE] <%s> %s", packet.read_string().c_str(), packet.read_string().c_str());
                        }
                        else if (SDL_strcmp(packet.id, "player_add") == 0)
                        {
                            std::string name = packet.read_string();
                            std::string uuid = packet.read_string();

                            Player player = Player(NULL, name, CONNECTED, uuid);
                            player.room_x = packet.read_int();
                            player.room_y = packet.read_int();
                            player.x = packet.read_int();
                            player.y = packet.read_int();
                            player.ax = packet.read_float();
                            player.ay = packet.read_float();
                            player.vx = packet.read_float();
                            player.vy = packet.read_float();
                            player.gravity = packet.read_int();
                            player.invis = packet.read_int();
                            player.deathseq = packet.read_int();
                            player.colour = packet.read_int();
                            players.push_back(player);

                            vlog_info("Player %s added, UUID %s", player.name.c_str(), player.uuid.c_str());
                            update_player_entities();
                        }
                        else if (SDL_strcmp(packet.id, "player_remove") == 0)
                        {
                            std::string uuid = packet.read_string();

                            bool found = false;
                            for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it)
                            {
                                if (it->uuid == uuid)
                                {
                                    found = true;
                                    vlog_info("Removing player %s.", it->name.c_str());
                                    players.erase(it);
                                    break;
                                }
                            }
                            if (!found)
                            {
                                vlog_info("Received player remove from unknown player.");
                                break;
                            }
                            else
                            {
                                update_player_entities();
                            }
                        }
                        else if (SDL_strcmp(packet.id, "player_movement") == 0)
                        {
                            std::string uuid = packet.read_string();

                            bool found = false;
                            for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it)
                            {
                                if (it->uuid == uuid)
                                {
                                    it->x = packet.read_int();
                                    it->y = packet.read_int();
                                    it->room_x = packet.read_int();
                                    it->room_y = packet.read_int();
                                    it->gravity = packet.read_int();
                                    it->dir = packet.read_int();
                                    it->ax = packet.read_float();
                                    it->ay = packet.read_float();
                                    it->vx = packet.read_float();
                                    it->vy = packet.read_float();
                                    found = true;
                                    break;
                                }
                            }
                            if (!found)
                            {
                                vlog_info("Received player movement from unknown player.");
                                break;
                            }
                            else
                            {
                                update_player_entities();
                            }
                        }
                        else if (SDL_strcmp(packet.id, "player_update") == 0)
                        {
                            std::string uuid = packet.read_string();
                            bool found = false;
                            for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it)
                            {
                                if (it->uuid == uuid)
                                {
                                    it->colour = packet.read_int();
                                    it->invis = packet.read_int();
                                    it->deathseq = packet.read_int();
                                    found = true;
                                    break;
                                }
                            }
                            if (!found)
                            {
                                vlog_info("Received player update from unknown player.");
                                break;
                            }
                            else
                            {
                                update_player_entities();
                            }
                        }
                    }
                }

                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                vlog_info("Peer disconnected.");
                /* Reset the peer's client information. */
                event.peer->data = NULL;

                // Remove the player from the list
                for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it)
                {
                    if (it->peer == event.peer)
                    {
                        vlog_info("Removing player %s, UUID %s", it->name.c_str(), it->uuid.c_str());

                        // Send a player_remove packet to all players

                        for (std::vector<Player>::iterator it2 = players.begin(); it2 != players.end(); ++it2)
                        {
                            Packet packet = Packet("player_remove", ENET_PACKET_FLAG_RELIABLE);
                            packet.write_string(it->uuid);
                            packet.send(it2->peer);
                        }

                        players.erase(it);

                        break;
                    }
                }
            }
        }

        if (should_update_player)
            send_player_update_packet();
        if (should_update_player_movement)
            send_player_movement_packet();
    }

    void cleanup(void)
    {
        if (host_instance != NULL)
        {
            enet_host_destroy(host_instance);
        }
    }

    void send_to_server(Packet* packet)
    {
        packet->send(peer);
    }

    void gotoroom()
    {
        // Ok, we changed rooms, tell the server in case they stayed at the exact same position while changing rooms (like a gotoroom)
        update_player_position();
        update_player_entities();
    }

    void update_player_state()
    {
        if (!multiplayer::is_server())
        {
            should_update_player = true;
        }
    }

    void update_player_position()
    {
        if (!multiplayer::is_server())
        {
            should_update_player_movement = true;
        }
    }
}
