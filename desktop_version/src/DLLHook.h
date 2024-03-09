#ifndef DLLHOOK_H
#define DLLHOOK_H

#define DLL_EXPORT

#include <stdint.h>
#include <SDL.h>
#include <vector>
#include <string>

#define SDL_Texture VVV_Texture
#define SDL_DestroyTexture(texture) VVV_DestroyTexture(texture)
#define SDL_CreateTexture(renderer, format, access, w, h) VVV_CreateTexture(w, h)
#define SDL_CreateTextureFromSurface(renderer, surface) VVV_CreateTexture(surface->w, surface->h)
#define SDL_SetTextureScaleMode(a, b)

struct VVV_Texture
{
    std::string id;
    int w;
    int h;
};

enum VVV_EventType
{
    VVV_EventType_NONE,
    VVV_EventType_KEYDOWN,
    VVV_EventType_KEYUP,
    VVV_EventType_MOUSEBUTTONDOWN,
    VVV_EventType_MOUSEBUTTONUP,
    VVV_EventType_MOUSEMOTION
};

struct VVV_Event
{
    VVV_EventType type;
    SDL_Keysym keysym;
    int button;
    int x;
    int y;
    bool repeat;
};

bool VVV_PollEvent(VVV_Event* event);
void VVV_PushEvent(VVV_Event* event);

static std::vector<VVV_Event> events;

void VVV_DestroyTexture(VVV_Texture* texture);
VVV_Texture* VVV_CreateTexture(int width, int height);

extern "C" DECLSPEC bool SDLCALL should_vvvvvv_exit(void);

extern "C" DECLSPEC int SDLCALL mainLoop(int argc, char* argv[]);
extern "C" DECLSPEC const char* SDLCALL get_state(void);
extern "C" DECLSPEC void SDLCALL set_base_path(const char* path);

extern "C" DECLSPEC void SDLCALL play_level_init(void);
extern "C" DECLSPEC void SDLCALL set_texture_size(const char* texture, int w, int h);
extern "C" DECLSPEC void SDLCALL play_level(const char* level_data, const char* playassets);

extern "C" DECLSPEC void SDLCALL simulate_keyevent(const char* event_type, const char* new_key, bool repeat);
extern "C" DECLSPEC void SDLCALL simulate_mouseevent(const char* event_type, int button, int x, int y);
extern "C" DECLSPEC void SDLCALL simulate_mousemoveevent(int x, int y);

typedef enum {
    DRAW_NONE = -1,
    DRAW_RECT = 0,
    DRAW_LINE = 1,
    DRAW_CIRCLE = 2,
    DRAW_TEXT = 3,
    DRAW_SET_COLOR = 4,
    DRAW_FILL_RECT = 5,
    DRAW_CLEAR = 6,
    DRAW_SET_TARGET = 7,
    DRAW_TEXTURE = 8,
    DRAW_TEXTURE_EXT = 9,
    DRAW_SET_TINT_COLOR = 10,
    DRAW_SET_TINT_ALPHA = 11
} DLL_draw_type;

typedef struct {
    int x;
    int y;
} DLL_point;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} DLL_color;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} DLL_rect;

typedef struct {
    DLL_draw_type type;
    DLL_point p1;
    DLL_point p2;
    DLL_point size;
    DLL_color color;
    char texture[255];
    DLL_rect src;
    DLL_rect dest;
    DLL_point center;
    int angle;
    bool flip_x;
    bool flip_y;
    bool src_whole;
    bool dest_whole;
} draw_message;

static std::vector<draw_message> draw_messages;

extern "C" DECLSPEC draw_message SDLCALL pop_draw_messages(void);

void push_draw_message(draw_message message);

#endif /* DLLHOOK_H */
