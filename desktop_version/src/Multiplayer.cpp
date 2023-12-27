#include <enet/enet.h>
#include <string>

#include "Multiplayer.h"

#include "Script.h"
#include "Game.h"
#include "Vlogging.h"

namespace multiplayer
{
    Packet::Packet(char* id, int flags)
    {
        should_free_packet = false;
        packet = enet_packet_create(id, SDL_strlen(id) + 1, flags);
        this->id = id;
    }

    Packet::Packet(ENetPacket* packet)
    {
        should_free_packet = false;
        this->packet = packet;

        // Okay, this is an incoming packet. Extract the ID (first string), set our id to that, and resize the packet to remove the ID

        char* id = (char*)packet->data;
        size_t id_length = SDL_strlen(id);
        this->id = (char*)SDL_malloc(id_length);
        SDL_memcpy(this->id, id, id_length);

        enet_packet_resize(packet, packet->dataLength - id_length - 1);
        packet->data += id_length + 1;
    }

    Packet::Packet(const Packet& other)
    {
        // copy constructor
        packet = enet_packet_create(other.packet->data, other.packet->dataLength, other.packet->flags);
        should_free_packet = true;
        id = SDL_strdup(other.id);
    }

    Packet::Packet(Packet&& other) noexcept
    {
        packet = other.packet;
        other.packet = NULL;
        should_free_packet = other.should_free_packet;
        id = other.id;
        other.id = NULL;
    }

    Packet::~Packet(void)
    {
        if (packet != NULL && should_free_packet)
        {
            enet_packet_destroy(packet);
        }

        if (id != NULL)
        {
            SDL_free(id);
        }
    }

    Packet& Packet::operator=(const Packet& other)
    {
        if (packet != NULL)
        {
            enet_packet_destroy(packet);
        }
        packet = enet_packet_create(other.packet->data, other.packet->dataLength, other.packet->flags);
        return *this;
    }

    Packet& Packet::operator=(Packet&& other) noexcept
    {
        if (packet != NULL)
        {
            enet_packet_destroy(packet);
        }
        packet = other.packet;
        other.packet = NULL;
        return *this;
    }

    void* Packet::get_data(void)
    {
        return packet->data;
    }

    size_t Packet::get_data_size(void)
    {
        return packet->dataLength;
    }

    int Packet::get_flags(void)
    {
        return packet->flags;
    }

    void Packet::send(ENetPeer* peer)
    {
        enet_peer_send(peer, 0, packet);
    }

    void Packet::write_string(std::string str)
    {
        // Write the string with a null terminator
        enet_packet_resize(packet, packet->dataLength + str.length() + 1);
        memcpy(packet->data + packet->dataLength - str.length() - 1, str.c_str(), str.length() + 1);
    }

    void Packet::write_int(int i)
    {
        enet_packet_resize(packet, packet->dataLength + sizeof(int));
        memcpy(packet->data + packet->dataLength - sizeof(int), &i, sizeof(int));
    }

    void Packet::write_float(float f)
    {
        enet_packet_resize(packet, packet->dataLength + sizeof(float));
        memcpy(packet->data + packet->dataLength - sizeof(float), &f, sizeof(float));
    }

    void Packet::write_double(double d)
    {
        enet_packet_resize(packet, packet->dataLength + sizeof(double));
        memcpy(packet->data + packet->dataLength - sizeof(double), &d, sizeof(double));
    }

    void Packet::write_bool(bool b)
    {
        enet_packet_resize(packet, packet->dataLength + sizeof(bool));
        memcpy(packet->data + packet->dataLength - sizeof(bool), &b, sizeof(bool));
    }

    void Packet::write_char(char c)
    {
        enet_packet_resize(packet, packet->dataLength + sizeof(char));
        memcpy(packet->data + packet->dataLength - sizeof(char), &c, sizeof(char));
    }

    void Packet::write_unsigned_char(unsigned char uc)
    {
        enet_packet_resize(packet, packet->dataLength + sizeof(unsigned char));
        memcpy(packet->data + packet->dataLength - sizeof(unsigned char), &uc, sizeof(unsigned char));
    }

    void Packet::write_short(short s)
    {
        enet_packet_resize(packet, packet->dataLength + sizeof(short));
        memcpy(packet->data + packet->dataLength - sizeof(short), &s, sizeof(short));
    }

    void Packet::write_unsigned_short(unsigned short us)
    {
        enet_packet_resize(packet, packet->dataLength + sizeof(unsigned short));
        memcpy(packet->data + packet->dataLength - sizeof(unsigned short), &us, sizeof(unsigned short));
    }

    void Packet::write_long(long l)
    {
        enet_packet_resize(packet, packet->dataLength + sizeof(long));
        memcpy(packet->data + packet->dataLength - sizeof(long), &l, sizeof(long));
    }

    void Packet::write_unsigned_long(unsigned long ul)
    {
        enet_packet_resize(packet, packet->dataLength + sizeof(unsigned long));
        memcpy(packet->data + packet->dataLength - sizeof(unsigned long), &ul, sizeof(unsigned long));
    }

    void Packet::write_long_long(long long ll)
    {
        enet_packet_resize(packet, packet->dataLength + sizeof(long long));
        memcpy(packet->data + packet->dataLength - sizeof(long long), &ll, sizeof(long long));
    }

    void Packet::write_unsigned_long_long(unsigned long long ull)
    {
        enet_packet_resize(packet, packet->dataLength + sizeof(unsigned long long));
        memcpy(packet->data + packet->dataLength - sizeof(unsigned long long), &ull, sizeof(unsigned long long));
    }

    std::string Packet::read_string(void)
    {
        // Read the string, ending at the null terminator

        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        std::string str = (char*)data_copy;
        enet_packet_resize(packet, packet->dataLength - str.length() - 1);
        memcpy(packet->data, (char*)data_copy + str.length() + 1, packet->dataLength);

        SDL_free(data_copy);
        return str;
    }

    int Packet::read_int(void)
    {
        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        int i = *(int*)data_copy;
        enet_packet_resize(packet, packet->dataLength - sizeof(int));
        memcpy(packet->data, (char*)data_copy + sizeof(int), packet->dataLength);

        SDL_free(data_copy);
        return i;
    }

    float Packet::read_float(void)
    {
        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        float f = *(float*)data_copy;
        enet_packet_resize(packet, packet->dataLength - sizeof(float));
        memcpy(packet->data, (char*)data_copy + sizeof(float), packet->dataLength);

        SDL_free(data_copy);
        return f;
    }

    double Packet::read_double(void)
    {
        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        double d = *(double*)data_copy;
        enet_packet_resize(packet, packet->dataLength - sizeof(double));
        memcpy(packet->data, (char*)data_copy + sizeof(double), packet->dataLength);

        SDL_free(data_copy);
        return d;
    }

    bool Packet::read_bool(void)
    {
        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        bool b = *(bool*)data_copy;
        enet_packet_resize(packet, packet->dataLength - sizeof(bool));
        memcpy(packet->data, (char*)data_copy + sizeof(bool), packet->dataLength);

        SDL_free(data_copy);
        return b;
    }

    char Packet::read_char(void)
    {
        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        char c = *(char*)data_copy;
        enet_packet_resize(packet, packet->dataLength - sizeof(char));
        memcpy(packet->data, (char*)data_copy + sizeof(char), packet->dataLength);

        SDL_free(data_copy);
        return c;
    }

    unsigned char Packet::read_unsigned_char(void)
    {
        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        unsigned char uc = *(unsigned char*)data_copy;
        enet_packet_resize(packet, packet->dataLength - sizeof(unsigned char));
        memcpy(packet->data, (char*)data_copy + sizeof(unsigned char), packet->dataLength);

        SDL_free(data_copy);
        return uc;
    }

    short Packet::read_short(void)
    {
        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        short s = *(short*)data_copy;
        enet_packet_resize(packet, packet->dataLength - sizeof(short));
        memcpy(packet->data, (char*)data_copy + sizeof(short), packet->dataLength);

        SDL_free(data_copy);
        return s;
    }

    unsigned short Packet::read_unsigned_short(void)
    {
        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        unsigned short us = *(unsigned short*)data_copy;
        enet_packet_resize(packet, packet->dataLength - sizeof(unsigned short));
        memcpy(packet->data, (char*)data_copy + sizeof(unsigned short), packet->dataLength);

        SDL_free(data_copy);
        return us;
    }

    long Packet::read_long(void)
    {
        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        long l = *(long*)data_copy;
        enet_packet_resize(packet, packet->dataLength - sizeof(long));
        memcpy(packet->data, (char*)data_copy + sizeof(long), packet->dataLength);

        SDL_free(data_copy);
        return l;
    }

    unsigned long Packet::read_unsigned_long(void)
    {
        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        unsigned long ul = *(unsigned long*)data_copy;
        enet_packet_resize(packet, packet->dataLength - sizeof(unsigned long));
        memcpy(packet->data, (char*)data_copy + sizeof(unsigned long), packet->dataLength);

        SDL_free(data_copy);
        return ul;
    }

    long long Packet::read_long_long(void)
    {
        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        long long ll = *(long long*)data_copy;
        enet_packet_resize(packet, packet->dataLength - sizeof(long long));
        memcpy(packet->data, (char*)data_copy + sizeof(long long), packet->dataLength);

        SDL_free(data_copy);
        return ll;
    }

    unsigned long long Packet::read_unsigned_long_long(void)
    {
        void* data_copy = SDL_malloc(packet->dataLength);
        memcpy(data_copy, packet->data, packet->dataLength);

        unsigned long long ull = *(unsigned long long*)data_copy;
        enet_packet_resize(packet, packet->dataLength - sizeof(unsigned long long));
        memcpy(packet->data, (char*)data_copy + sizeof(unsigned long long), packet->dataLength);

        SDL_free(data_copy);
        return ull;
    }

    bool server = false;
    std::string server_ip = "localhost";
    int server_port = 65432;

    ENetAddress server_address;
    ENetHost* host_instance;
    ENetPeer* peer;


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
            vlog_info("No available peers for initiating an ENet connection.\n");
            return false;
        }

        ENetEvent event;
        if (enet_host_service(host_instance, &event, 5000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT)
        {
            vlog_info("Connection to %s:%d succeeded.\n", server_ip.c_str(), server_port);
            return true;
        }
        else
        {
            vlog_info("Connection to %s:%d failed.\n", server_ip.c_str(), server_port);
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
                vlog_info("A new client connected from %x:%u.\n",
                    event.peer->address.host,
                    event.peer->address.port);

                /* Store any relevant client information here. */
                event.peer->data = "Client information";

                // If we're the server, send the client the current game state
                if (is_server())
                {
                    Packet packet = Packet("connection", ENET_PACKET_FLAG_RELIABLE);
                    packet.write_int(1); // version 1
                    packet.send(event.peer);
                }

                break;
            case ENET_EVENT_TYPE_RECEIVE:
                /* Clean up the packet now that we're done using it. */
                {
                    Packet packet = Packet(event.packet);

                    if (SDL_strcmp(packet.id, "connection") == 0)
                    {
                        if (packet.read_int() != 1)
                        {
                            vlog_info("Client version mismatch.\n");
                            enet_peer_disconnect(event.peer, 0);
                            break;
                        }
                        else
                        {
                            vlog_info("Client version match.\n");
                        }

                        script.startgamemode(Start_SERVER);
                    }
                }

                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                vlog_info("%s disconnected.\n", event.peer->data);
                /* Reset the peer's client information. */
                event.peer->data = NULL;
            }
        }
    }

    void cleanup(void)
    {
        if (host_instance != NULL)
        {
            enet_host_destroy(host_instance);
        }
    }
}
