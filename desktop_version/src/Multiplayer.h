#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include <string>
#include <map>
#include <utility>

#include <enet/enet.h>

#include "BinaryBlob.h"
#include "Packet.h"

namespace multiplayer
{
    extern std::string last_uuid_hack;
    extern std::map<std::string, std::pair<unsigned char*, size_t> > assets_data;
    extern bool connecting;
    extern bool connected;
    extern bool disconnected;
    extern std::string name;
    extern int preferred_color_id;
    extern std::vector<int> player_colors;
    extern int preferred_color;

    extern binaryBlob pppppp_data;
    extern binaryBlob mmmmmm_data;

    extern bool has_custom_vvvvvvmusic;
    extern bool has_custom_mmmmmm;

    bool is_server(void);
    void set_server(bool server);
    std::string get_server_ip(void);
    void set_server_ip(std::string server_ip);
    int get_server_port(void);
    void set_server_port(int server_port);

    void update_host(void);

    bool create_server(void);
    bool create_client(void);

    bool connect_to_server(void);

    void update(void);

    void cleanup(void);

    void send_to_server(Packet* packet);

    void gotoroom(void);

    void update_player_state(void);

    void update_player_position(void);

    void sync_screen_effects(void);

    void playef_others(int sound);

    void server_init(void);

    void unmount_multiplayer_assets(void);
}

void connectingrender(void);
void connectingrenderfixed(void);
void connectinginput(void);
void connectinglogic(void);

void serverrender(void);
void serverrenderfixed(void);
void serverinput(void);
void serverlogic(void);

#endif /* MULTIPLAYER_H */
