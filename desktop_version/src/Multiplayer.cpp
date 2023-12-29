#include <enet/enet.h>
#include <string>
#include <vector>
#include <map>
#include <utility>

#include <SDL.h>

#include "Multiplayer.h"

#include "Alloc.h"
#include "Constants.h"
#include "CustomLevels.h"
#include "Entity.h"
#include "FileSystemUtils.h"
#include "Font.h"
#include "Game.h"
#include "Graphics.h"
#include "Input.h"
#include "Map.h"
#include "Music.h"
#include "Packet.h"
#include "Player.h"
#include "RenderFixed.h"
#include "Script.h"
#include "Vlogging.h"

#include "UtilityClass.h"

// UUIDs...
#ifdef _WIN32
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000
#define WIN32_LEAN_AND_MEAN
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

    bool connected = false;
    bool connecting = false;
    bool did_startmode = false;
    bool should_start = false;

    int total_rooms = 0;
    int waiting_for_rooms = -1;
    int total_assets = 0;
    int waiting_for_assets = -1;

    std::vector<std::string> assets;
    std::map<std::string, std::pair<unsigned char*, size_t> > assets_data;

    ENetAddress server_address;
    ENetHost* host_instance;
    ENetPeer* peer;

    std::vector<Player> players;

    std::string last_uuid_hack;

    int timeout = 5 * 30; // 5 seconds
    int viridian_position = 160;

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
            packet.write_int(obj.entities[i].tile);

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
            packet.write_float(obj.entities[i].ax);
            packet.write_float(obj.entities[i].ay);
            packet.write_float(obj.entities[i].vx);
            packet.write_float(obj.entities[i].vy);
            packet.write_int(game.gravitycontrol);
            packet.write_int(obj.entities[i].dir);

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
        // Okay, now loop through all entities and see if any IDs match any players. If not, remove them.
        for (int i = 0; i < obj.entities.size(); i++)
        {
            if (obj.entities[i].uuid == "") continue;
            if (obj.entities[i].rule != 999) continue;

            bool found = false;
            for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it)
            {
                if (it->uuid == "") continue;
                if (it->uuid == obj.entities[i].uuid)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                obj.disableentity(i);
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

    void server_init()
    {
        assets.clear();

        EnumHandle handle = {};
        const char* item;
        while ((item = FILESYSTEM_enumerateAssets("graphics", &handle)) != NULL)
        {
            std::string path = "graphics/" + std::string(item);
            assets.push_back(path);
        }
        FILESYSTEM_freeEnumerate(&handle);

        handle = {};
        while ((item = FILESYSTEM_enumerateAssets("sounds", &handle)) != NULL)
        {
            std::string path = "sounds/" + std::string(item);
            assets.push_back(path);
        }
        FILESYSTEM_freeEnumerate(&handle);

        if (FILESYSTEM_isAssetMounted("vvvvvvmusic.vvv")) assets.push_back("vvvvvvmusic.vvv");
        if (FILESYSTEM_isAssetMounted("mmmmmm.vvv")) assets.push_back("mmmmmm.vvv");
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
        if (peer == NULL)
        {
            peer = enet_host_connect(host_instance, &server_address, 2, 0);
        }
        else
        {
            vlog_info("Tried to connect while already connected!");
            return false;
        }

        script.hardreset();
        assets.clear();

        FILESYSTEM_unmountAssets();

        timeout = 5 * 30; // 5 seconds
        viridian_position = 160;
        connected = false;
        connecting = true;
        did_startmode = false;
        should_start = false;

        total_rooms = 0;
        waiting_for_rooms = -1;
        total_assets = 0;
        waiting_for_assets = -1;

        if (peer == NULL)
        {
            vlog_info("No available peers for initiating an ENet connection.");
            return false;
        }

        return true;
    }

    void update(void)
    {
        ENetEvent event;
        while (enet_host_service(host_instance, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:

                connecting = false;
                connected = true;

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

                    Packet packet = Packet("server_info", ENET_PACKET_FLAG_RELIABLE);
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
                                    packet.write_int(player->dir);
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
                                    packet.write_int(it->dir);
                                    packet.write_int(it->invis);
                                    packet.write_int(it->deathseq);
                                    packet.write_int(it->colour);
                                    packet.send(event.peer);
                                }
                            }

                            // Send the player general level information
                            Packet packet = Packet("level_info", ENET_PACKET_FLAG_RELIABLE);

                            packet.write_bool(map.custommode);

                            packet.write_string(cl.title);
                            packet.write_string(cl.creator);
                            packet.write_string(cl.website);
                            packet.write_string(cl.Desc1);
                            packet.write_string(cl.Desc2);
                            packet.write_string(cl.Desc3);

                            packet.write_int(map.getwidth());
                            packet.write_int(map.getheight());

                            packet.write_int(cl.levmusic);
                            packet.write_bool(cl.onewaycol_override);
                            packet.write_string(cl.level_font_name);

                            packet.write_int(assets.size());

                            packet.send(event.peer);

                            // Send the player all of the rooms

                            for (int i = 0; i < map.getwidth() * map.getheight(); i++)
                            {
                                int x = i % map.getwidth();
                                int y = floor(i / map.getwidth());

                                Packet packet = Packet("room_info", ENET_PACKET_FLAG_RELIABLE);

                                packet.write_int(x);
                                packet.write_int(y);

                                // get the room properties
                                const RoomProperty* properties = cl.getroomprop(x, y);

                                packet.write_int(properties->tileset);
                                packet.write_int(properties->tilecol);
                                packet.write_int(properties->platx1);
                                packet.write_int(properties->platy1);
                                packet.write_int(properties->platx2);
                                packet.write_int(properties->platy2);
                                packet.write_int(properties->platv);
                                packet.write_int(properties->enemyx1);
                                packet.write_int(properties->enemyy1);
                                packet.write_int(properties->enemyx2);
                                packet.write_int(properties->enemyy2);
                                packet.write_int(properties->enemytype);
                                packet.write_int(properties->directmode);
                                packet.write_int(properties->warpdir);
                                packet.write_string(properties->roomname);

                                // Send the map contents
                                for (int i = 0; i < SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES; i++)
                                {
                                    int x2 = i % SCREEN_WIDTH_TILES;
                                    int y2 = floor(i / SCREEN_WIDTH_TILES);
                                    packet.write_int(cl.gettile(x, y, x2, y2));
                                }

                                packet.send(event.peer);
                            }

                            // Send the player all of the assets
                            for (int i = 0; i < assets.size(); i++)
                            {
                                Packet packet = Packet("level_asset", ENET_PACKET_FLAG_RELIABLE);
                                packet.write_string(assets[i]);

                                unsigned char* fileIn;
                                size_t length;
                                FILESYSTEM_loadAssetToMemory(assets[i].c_str(), &fileIn, &length);
                                if (fileIn == NULL)
                                {
                                    SDL_assert(0 && "File missing!?");
                                    return;
                                }

                                packet.write_int(length);

                                packet.write_blob(fileIn, length);

                                // Send asset packets on channel 1
                                packet.send(event.peer, 1);

                                VVV_free(fileIn);
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
                            player->ax = packet.read_float();
                            player->ay = packet.read_float();
                            player->vx = packet.read_float();
                            player->vy = packet.read_float();
                            player->gravity = packet.read_int();
                            player->dir = packet.read_int();

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
                                    packet.write_float(player->ax);
                                    packet.write_float(player->ay);
                                    packet.write_float(player->vx);
                                    packet.write_float(player->vy);
                                    packet.write_int(player->gravity);
                                    packet.write_int(player->dir);
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
                                    packet.write_int(player->tile);
                                    packet.send(it->peer);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (SDL_strcmp(packet.id, "server_info") == 0)
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
                            player.dir = packet.read_int();
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
                                    it->ax = packet.read_float();
                                    it->ay = packet.read_float();
                                    it->vx = packet.read_float();
                                    it->vy = packet.read_float();
                                    it->gravity = packet.read_int();
                                    it->dir = packet.read_int();
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
                                    it->tile = packet.read_int();
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
                        else if (SDL_strcmp(packet.id, "level_info") == 0)
                        {
                            map.custommode = packet.read_bool();
                            map.custommodeforreal = map.custommode;
                            cl.title = packet.read_string();
                            cl.creator = packet.read_string();
                            cl.website = packet.read_string();
                            cl.Desc1 = packet.read_string();
                            cl.Desc2 = packet.read_string();
                            cl.Desc3 = packet.read_string();

                            int width = packet.read_int();
                            int height = packet.read_int();

                            cl.mapwidth = width;
                            cl.mapheight = height;

                            cl.levmusic = packet.read_int();
                            cl.onewaycol_override = packet.read_bool();
                            cl.level_font_name = packet.read_string();

                            waiting_for_rooms = width * height;
                            waiting_for_assets = packet.read_int();
                        }
                        else if (SDL_strcmp(packet.id, "room_info") == 0)
                        {
                            int x = packet.read_int();
                            int y = packet.read_int();

                            // get the room properties
                            const RoomProperty* properties = cl.getroomprop(x, y);
                            cl.setroomtileset(x, y, packet.read_int());
                            cl.setroomtilecol(x, y, packet.read_int());
                            cl.setroomplatx1(x, y, packet.read_int());
                            cl.setroomplaty1(x, y, packet.read_int());
                            cl.setroomplatx2(x, y, packet.read_int());
                            cl.setroomplaty2(x, y, packet.read_int());
                            cl.setroomplatv(x, y, packet.read_int());
                            cl.setroomenemyx1(x, y, packet.read_int());
                            cl.setroomenemyy1(x, y, packet.read_int());
                            cl.setroomenemyx2(x, y, packet.read_int());
                            cl.setroomenemyy2(x, y, packet.read_int());
                            cl.setroomenemytype(x, y, packet.read_int());
                            cl.setroomdirectmode(x, y, packet.read_int());
                            cl.setroomwarpdir(x, y, packet.read_int());
                            cl.setroomroomname(x, y, packet.read_string());

                            // Get the map contents

                            for (int i = 0; i < SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES; i++)
                            {
                                int x2 = i % SCREEN_WIDTH_TILES;
                                int y2 = floor(i / SCREEN_WIDTH_TILES);
                                cl.settile(x, y, x2, y2, packet.read_int());
                            }

                            total_rooms++;
                        }
                        else if (SDL_strcmp(packet.id, "level_asset") == 0)
                        {
                            std::string path = packet.read_string();
                            size_t length = packet.read_int();

                            unsigned char* fileIn;
                            packet.read_blob(&fileIn, length);

                            vlog_info("Received asset %s", path.c_str());

                            if (fileIn == NULL)
                            {
                                SDL_assert(0 && "Couldn't allocate memory");
                            }
                            else
                            {
                                assets_data[path] = std::make_pair(fileIn, length);
                            }

                            total_assets++;
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

        if (!connected && connecting)
        {
            timeout--;
            if (timeout <= 0)
            {
                vlog_info("Connection to %s:%d timed out.", server_ip.c_str(), server_port);
                enet_peer_reset(peer);
                //enet_host_destroy(host_instance);
                peer = NULL;
                connected = false;
                connecting = false;
                return;
            }
        }
    }

    void cleanup(void)
    {
        // If we're connected, gracefully disconnect.

        if (peer != NULL)
        {
            enet_peer_disconnect(peer, 0);

            // Wait 3 seconds for a response
            ENetEvent event;
            bool force_kill = true;
            while (enet_host_service(host_instance, &event, 3000) > 0)
            {
                switch (event.type)
                {
                case ENET_EVENT_TYPE_RECEIVE:
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    force_kill = false;
                    break;
                }
            }

            if (force_kill)
            {
                enet_peer_reset(peer);
            }
        }

        if (host_instance != NULL)
        {
            enet_host_destroy(host_instance);
        }
    }

    void unmount_multiplayer_assets(void)
    {
        for (std::map<std::string, std::pair<unsigned char*, size_t> >::iterator it = assets_data.begin(); it != assets_data.end(); ++it)
        {
            VVV_free(it->second.first);
        }
        assets_data.clear();
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

static inline float calc_percentage(float amount, float total)
{
    if (amount == total) return 1; // if 0/0, return 1, also just a nice shortcut
    return amount / total;
}

void connectingrender(void)
{
    graphics.clear();

    if (!game.colourblindmode) graphics.drawtowerbackground(graphics.titlebg);

    int tr = graphics.col_tr;
    int tg = graphics.col_tg;
    int tb = graphics.col_tb;

    tr = int(tr * .8f);
    tg = int(tg * .8f);
    tb = int(tb * .8f);
    if (tr < 0) tr = 0;
    if (tr > 255) tr = 255;
    if (tg < 0) tg = 0;
    if (tg > 255) tg = 255;
    if (tb < 0) tb = 0;
    if (tb > 255) tb = 255;

    graphics.draw_rect(64, 128, 192, 16, tr, tg, tb);

    font::print(PR_2X | PR_CEN, -1, 65, "Connecting...", tr, tg, tb);
    if (multiplayer::connected)
    {
        font::print(PR_CEN, -1, 95, "Joining server...", tr, tg, tb);

        int progress = calc_percentage(multiplayer::total_rooms + multiplayer::total_assets, multiplayer::waiting_for_rooms + multiplayer::waiting_for_assets) * 188;

        graphics.fill_rect(66, 130, progress, 12, tr, tg, tb);
    }
    else
    {
        font::print(PR_CEN, -1, 95, "Connecting to the server...", tr, tg, tb);
    }

    graphics.draw_sprite(multiplayer::viridian_position, 240 / 2 + 64, graphics.crewframe, graphics.col_crewcyan);
    graphics.draw_sprite(multiplayer::viridian_position - 320, 240 / 2 + 64, graphics.crewframe, graphics.col_crewcyan);

    graphics.drawfade();

    graphics.renderwithscreeneffects();
}

void connectingrenderfixed(void)
{
    if (!game.colourblindmode)
    {
        graphics.updatetowerbackground(graphics.titlebg);
    }

    titleupdatetextcol();

    graphics.updatetitlecolours();

    multiplayer::viridian_position += 6;
    if (multiplayer::viridian_position > 320)
    {
        multiplayer::viridian_position -= 320;
    }

    graphics.crewframedelay--;
    if (graphics.crewframedelay <= 0)
    {
        graphics.crewframedelay = 4;
        graphics.crewframe = (graphics.crewframe + 1) % 2;
    }
}

void connectinginput(void)
{
    // Press R to cancel maybe?

    if (fadetomode)
    {
        handlefadetomode();
    }
}

void connectinglogic(void)
{
    help.updateglow();

    graphics.titlebg.bypos -= 2;
    graphics.titlebg.bscroll = -2;

    // If we're no longer trying to connect
    if (!multiplayer::connecting)
    {
        if (!multiplayer::connected)
        {
            // We didn't connect, so probably a timeout
            map.nexttowercolour();
            music.playef(Sound_CRY);
            game.createmenu(Menu::connectiontimeout, false);
            game.gamestate = TITLEMODE;
        }
        else if (!multiplayer::did_startmode && multiplayer::should_start)
        {
            multiplayer::did_startmode = true;
            // We did connect, so fade out the screen
            startmode(Start_SERVER);
        }
        else if (!multiplayer::should_start)
        {
            // We're connected, but we haven't gotten everything we need yet

            bool done_rooms = false;
            bool done_assets = false;

            if (multiplayer::total_rooms >= multiplayer::waiting_for_rooms && multiplayer::waiting_for_rooms != -1)
            {
                done_rooms = true;
            }

            if (multiplayer::total_assets >= multiplayer::waiting_for_assets && multiplayer::waiting_for_assets != -1)
            {
                done_assets = true;
            }

            if (done_rooms && done_assets)
            {
                multiplayer::should_start = true;
            }

        }
    }
}
