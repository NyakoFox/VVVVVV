#ifndef PACKET_H
#define PACKET_H

#include <enet/enet.h>

class Packet
{
    bool should_free_packet;
    ENetPacket* packet;

    public:
        char* id;
        Packet(char* id, int flags);
        Packet(ENetPacket* packet);
        Packet(const Packet& other);
        ~Packet(void);

        Packet& operator=(const Packet& other);

        void* get_data(void);
        size_t get_data_size(void);
        int get_flags(void);

        void send(ENetPeer* peer);
        void send(ENetPeer* peer, enet_uint8 channel);

        void write_string(std::string str);
        void write_int(int i);
        void write_float(float f);
        void write_double(double d);
        void write_bool(bool b);
        void write_char(char c);
        void write_unsigned_char(unsigned char uc);
        void write_short(short s);
        void write_unsigned_short(unsigned short us);
        void write_long(long l);
        void write_unsigned_long(unsigned long ul);
        void write_long_long(long long ll);
        void write_unsigned_long_long(unsigned long long ull);
        void write_blob(void* blob, size_t size);

        std::string read_string(void);
        int read_int(void);
        float read_float(void);
        double read_double(void);
        bool read_bool(void);
        char read_char(void);
        unsigned char read_unsigned_char(void);
        short read_short(void);
        unsigned short read_unsigned_short(void);
        long read_long(void);
        unsigned long read_unsigned_long(void);
        long long read_long_long(void);
        unsigned long long read_unsigned_long_long(void);
        void read_blob(unsigned char** blob, size_t blob_size);
};

#endif /* PACKET_H */
