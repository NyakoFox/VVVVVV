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
#define SDL_SetTextureScaleMode() 0

struct VVV_Texture
{
    std::string id;
    int w;
    int h;
};

void VVV_DestroyTexture(VVV_Texture* texture);
VVV_Texture* VVV_CreateTexture(int width, int height);

extern "C" DECLSPEC int SDLCALL mainLoop(int argc, char* argv[]);
extern "C" DECLSPEC const char* SDLCALL get_state(void);
extern "C" DECLSPEC void SDLCALL set_base_path(const char* path);

extern "C" DECLSPEC void SDLCALL play_level_init(void);
extern "C" DECLSPEC void SDLCALL set_texture_size(const char* texture, int w, int h);
extern "C" DECLSPEC void SDLCALL play_level(const char* level_data, const char* playassets);

extern "C" DECLSPEC void SDLCALL simulate_keyevent(const char* event_type, const char* new_key);
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
} draw_type;

typedef struct {
    int x;
    int y;
} point;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} color;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} rect;

typedef struct {
    draw_type type;
    point p1;
    point p2;
    point size;
    color color;
    char texture[255];
    rect src;
    rect dest;
    point center;
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
