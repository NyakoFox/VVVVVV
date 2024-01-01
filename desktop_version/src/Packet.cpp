#include <enet/enet.h>
#include <string>
#include <SDL.h>

#include "Packet.h"

#include "Vlogging.h"

Packet::Packet(char* id, int flags)
{
    should_free_packet = false;
    packet = enet_packet_create(id, SDL_strlen(id) + 1, flags);

    this->id = SDL_strdup(id);
}

Packet::Packet(ENetPacket* packet)
{
    should_free_packet = false;
    this->packet = packet;

    // Okay, this is an incoming packet. Extract the ID (first string), set our id to that, and resize the packet to remove the ID

    char* id = (char*)packet->data;
    size_t id_length = SDL_strlen(id) + 1;
    this->id = (char*)SDL_malloc(id_length);
    SDL_memcpy(this->id, id, id_length);

    enet_packet_resize(packet, packet->dataLength - id_length);
    packet->data += id_length;
}

Packet::Packet(const Packet& other)
{
    // copy constructor
    packet = enet_packet_create(other.packet->data, other.packet->dataLength, other.packet->flags);
    should_free_packet = true;
    id = SDL_strdup(other.id);
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

int Packet::send(ENetPeer* peer)
{
    return enet_peer_send(peer, 0, packet);
}

int Packet::send(ENetPeer* peer, enet_uint8 channel)
{
    return enet_peer_send(peer, channel, packet);
}

void Packet::write_string(std::string str)
{
    // Write the string with a null terminator
    enet_packet_resize(packet, packet->dataLength + str.length() + 1);
    SDL_memcpy(packet->data + packet->dataLength - str.length() - 1, str.c_str(), str.length() + 1);
}

void Packet::write_int(int i)
{
    enet_packet_resize(packet, packet->dataLength + sizeof(int));
    SDL_memcpy(packet->data + packet->dataLength - sizeof(int), &i, sizeof(int));
}

void Packet::write_float(float f)
{
    enet_packet_resize(packet, packet->dataLength + sizeof(float));
    SDL_memcpy(packet->data + packet->dataLength - sizeof(float), &f, sizeof(float));
}

void Packet::write_double(double d)
{
    enet_packet_resize(packet, packet->dataLength + sizeof(double));
    SDL_memcpy(packet->data + packet->dataLength - sizeof(double), &d, sizeof(double));
}

void Packet::write_bool(bool b)
{
    enet_packet_resize(packet, packet->dataLength + sizeof(bool));
    SDL_memcpy(packet->data + packet->dataLength - sizeof(bool), &b, sizeof(bool));
}

void Packet::write_char(char c)
{
    enet_packet_resize(packet, packet->dataLength + sizeof(char));
    SDL_memcpy(packet->data + packet->dataLength - sizeof(char), &c, sizeof(char));
}

void Packet::write_unsigned_char(unsigned char uc)
{
    enet_packet_resize(packet, packet->dataLength + sizeof(unsigned char));
    SDL_memcpy(packet->data + packet->dataLength - sizeof(unsigned char), &uc, sizeof(unsigned char));
}

void Packet::write_short(short s)
{
    enet_packet_resize(packet, packet->dataLength + sizeof(short));
    SDL_memcpy(packet->data + packet->dataLength - sizeof(short), &s, sizeof(short));
}

void Packet::write_unsigned_short(unsigned short us)
{
    enet_packet_resize(packet, packet->dataLength + sizeof(unsigned short));
    SDL_memcpy(packet->data + packet->dataLength - sizeof(unsigned short), &us, sizeof(unsigned short));
}

void Packet::write_long(long l)
{
    enet_packet_resize(packet, packet->dataLength + sizeof(long));
    SDL_memcpy(packet->data + packet->dataLength - sizeof(long), &l, sizeof(long));
}

void Packet::write_unsigned_long(unsigned long ul)
{
    enet_packet_resize(packet, packet->dataLength + sizeof(unsigned long));
    SDL_memcpy(packet->data + packet->dataLength - sizeof(unsigned long), &ul, sizeof(unsigned long));
}

void Packet::write_long_long(long long ll)
{
    enet_packet_resize(packet, packet->dataLength + sizeof(long long));
    SDL_memcpy(packet->data + packet->dataLength - sizeof(long long), &ll, sizeof(long long));
}

void Packet::write_unsigned_long_long(unsigned long long ull)
{
    enet_packet_resize(packet, packet->dataLength + sizeof(unsigned long long));
    SDL_memcpy(packet->data + packet->dataLength - sizeof(unsigned long long), &ull, sizeof(unsigned long long));
}

void Packet::write_blob(void* blob, size_t blob_size)
{
    enet_packet_resize(packet, packet->dataLength + blob_size);
    SDL_memcpy(packet->data + packet->dataLength - blob_size, blob, blob_size);
}

std::string Packet::read_string(void)
{
    // Read the string, ending at the null terminator

    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    std::string str = (char*)data_copy;
    enet_packet_resize(packet, packet->dataLength - str.length() - 1);
    SDL_memcpy(packet->data, (char*)data_copy + str.length() + 1, packet->dataLength);

    SDL_free(data_copy);
    return str;
}

int Packet::read_int(void)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    int i = *(int*)data_copy;
    enet_packet_resize(packet, packet->dataLength - sizeof(int));
    SDL_memcpy(packet->data, (char*)data_copy + sizeof(int), packet->dataLength);

    SDL_free(data_copy);
    return i;
}

float Packet::read_float(void)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    float f = *(float*)data_copy;
    enet_packet_resize(packet, packet->dataLength - sizeof(float));
    SDL_memcpy(packet->data, (char*)data_copy + sizeof(float), packet->dataLength);

    SDL_free(data_copy);
    return f;
}

double Packet::read_double(void)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    double d = *(double*)data_copy;
    enet_packet_resize(packet, packet->dataLength - sizeof(double));
    SDL_memcpy(packet->data, (char*)data_copy + sizeof(double), packet->dataLength);

    SDL_free(data_copy);
    return d;
}

bool Packet::read_bool(void)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    bool b = *(bool*)data_copy;
    enet_packet_resize(packet, packet->dataLength - sizeof(bool));
    SDL_memcpy(packet->data, (char*)data_copy + sizeof(bool), packet->dataLength);

    SDL_free(data_copy);
    return b;
}

char Packet::read_char(void)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    char c = *(char*)data_copy;
    enet_packet_resize(packet, packet->dataLength - sizeof(char));
    SDL_memcpy(packet->data, (char*)data_copy + sizeof(char), packet->dataLength);

    SDL_free(data_copy);
    return c;
}

unsigned char Packet::read_unsigned_char(void)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    unsigned char uc = *(unsigned char*)data_copy;
    enet_packet_resize(packet, packet->dataLength - sizeof(unsigned char));
    SDL_memcpy(packet->data, (char*)data_copy + sizeof(unsigned char), packet->dataLength);

    SDL_free(data_copy);
    return uc;
}

short Packet::read_short(void)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    short s = *(short*)data_copy;
    enet_packet_resize(packet, packet->dataLength - sizeof(short));
    SDL_memcpy(packet->data, (char*)data_copy + sizeof(short), packet->dataLength);

    SDL_free(data_copy);
    return s;
}

unsigned short Packet::read_unsigned_short(void)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    unsigned short us = *(unsigned short*)data_copy;
    enet_packet_resize(packet, packet->dataLength - sizeof(unsigned short));
    SDL_memcpy(packet->data, (char*)data_copy + sizeof(unsigned short), packet->dataLength);

    SDL_free(data_copy);
    return us;
}

long Packet::read_long(void)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    long l = *(long*)data_copy;
    enet_packet_resize(packet, packet->dataLength - sizeof(long));
    SDL_memcpy(packet->data, (char*)data_copy + sizeof(long), packet->dataLength);

    SDL_free(data_copy);
    return l;
}

unsigned long Packet::read_unsigned_long(void)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    unsigned long ul = *(unsigned long*)data_copy;
    enet_packet_resize(packet, packet->dataLength - sizeof(unsigned long));
    SDL_memcpy(packet->data, (char*)data_copy + sizeof(unsigned long), packet->dataLength);

    SDL_free(data_copy);
    return ul;
}

long long Packet::read_long_long(void)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    long long ll = *(long long*)data_copy;
    enet_packet_resize(packet, packet->dataLength - sizeof(long long));
    SDL_memcpy(packet->data, (char*)data_copy + sizeof(long long), packet->dataLength);

    SDL_free(data_copy);
    return ll;
}

unsigned long long Packet::read_unsigned_long_long(void)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    unsigned long long ull = *(unsigned long long*)data_copy;
    enet_packet_resize(packet, packet->dataLength - sizeof(unsigned long long));
    SDL_memcpy(packet->data, (char*)data_copy + sizeof(unsigned long long), packet->dataLength);

    SDL_free(data_copy);
    return ull;
}

// input is unallocated
void Packet::read_blob(unsigned char** blob, size_t blob_size)
{
    void* data_copy = SDL_malloc(packet->dataLength);
    SDL_memcpy(data_copy, packet->data, packet->dataLength);

    // also alloc the blob

    *blob = (unsigned char*)SDL_malloc(blob_size);

    SDL_memcpy(*blob, data_copy, blob_size);
    enet_packet_resize(packet, packet->dataLength - blob_size);
    SDL_memcpy(packet->data, (char*)data_copy + blob_size, packet->dataLength);

    SDL_free(data_copy);
}
