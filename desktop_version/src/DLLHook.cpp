#include "DLLHook.h"

#include <SDL.h>

#include <vector>

#include "Alloc.h"
#include "CustomLevels.h"
#include "Editor.h"
#include "Entity.h"
#include "Exit.h"
#include "FileSystemUtils.h"
#include "Graphics.h"
#include "Game.h"
#include "Screen.h"
#include "Script.h"
#include "UtilityClass.h"
#include "Map.h"
#include "Music.h"
#include "KeyPoll.h"
#include "UtilityClass.h"
#include "Vlogging.h"

VVV_Texture* VVV_CreateTexture(int width, int height)
{
    VVV_Texture* texture = new VVV_Texture();
    texture->id = "unknown";
    texture->w = width;
    texture->h = height;
    return texture;
}

void VVV_DestroyTexture(VVV_Texture* texture)
{
    delete texture;
}

extern "C" DECLSPEC draw_message SDLCALL pop_draw_messages(void)
{
    draw_message message;

    if (draw_messages.size() == 0)
    {
        message.type = DRAW_NONE;
        return message;
    }

    message = draw_messages.back();
    draw_messages.pop_back();
    return message;
}

void push_draw_message(draw_message message)
{
    draw_messages.push_back(message);
}

extern "C" DECLSPEC void SDLCALL set_base_path(const char* path)
{
    game.base_path = path;
}

extern "C" DECLSPEC const char* SDLCALL get_state(void)
{
    switch (game.gamestate)
    {
    case GAMEMODE:
        return "GAMEMODE";
    case TITLEMODE:
        return "TITLEMODE";
    case MAPMODE:
        return "MAPMODE";
    case TELEPORTERMODE:
        return "TELEPORTERMODE";
    case GAMECOMPLETE:
        return "GAMECOMPLETE";
    case GAMECOMPLETE2:
        return "GAMECOMPLETE2";
    case EDITORMODE:
        return "EDITORMODE";
    case PRELOADER:
        return "PRELOADER";
    case IDLEMODE:
        return "IDLEMODE";
    default:
        return "UNKNOWN";
    }
}

extern "C" DECLSPEC void SDLCALL simulate_keyevent(const char* event_type, const char* new_key)
{
    SDL_Event event;
    SDL_zero(event);

    if (SDL_strcmp(event_type, "keydown") == 0)
    {
        event.type = SDL_KEYDOWN;
    }
    else if (SDL_strcmp(event_type, "keyup") == 0)
    {
        event.type = SDL_KEYUP;
    }
    else
    {
        vlog_error("Invalid event type: %s", event_type);
        return;
    }

    event.key.keysym.sym = SDL_GetKeyFromName(new_key);

    SDL_PushEvent(&event);
}

extern "C" DECLSPEC void SDLCALL simulate_mouseevent(const char* event_type, int button, int x, int y)
{
    SDL_Event event;
    SDL_zero(event);

    if (SDL_strcmp(event_type, "mousedown") == 0)
    {
        event.type = SDL_MOUSEBUTTONDOWN;
    }
    else if (SDL_strcmp(event_type, "mouseup") == 0)
    {
        event.type = SDL_MOUSEBUTTONUP;
    }
    else
    {
        vlog_error("Invalid event type: %s", event_type);
        return;
    }

    event.button.button = button;
    event.button.x = x;
    event.button.y = y;

    SDL_PushEvent(&event);
}

extern "C" DECLSPEC void SDLCALL simulate_mousemoveevent(int x, int y)
{
    SDL_Event event;
    SDL_zero(event);

    event.type = SDL_MOUSEMOTION;
    event.motion.x = x;
    event.motion.y = y;

    SDL_PushEvent(&event);
}

extern "C" DECLSPEC void SDLCALL play_level_init(void)
{
    graphics.texture_sizes.clear();
}

extern "C" DECLSPEC void SDLCALL set_texture_size(const char* texture, int w, int h)
{
    SDL_Point point;
    point.x = w;
    point.y = h;
    graphics.texture_sizes[texture] = point;
}

extern "C" DECLSPEC void SDLCALL play_level(const char* level_data, const char* playassets)
{
    cl.injected_level = (unsigned char*)SDL_malloc(SDL_strlen(level_data) + 1);

    if (cl.injected_level == NULL)
    {
        vlog_error("Failed to allocate memory for injected level data!");
        VVV_exit(1);
    }

    SDL_memcpy(cl.injected_level, level_data, SDL_strlen(level_data) + 1);
    cl.injected_level_len = SDL_strlen(level_data);

    game.levelpage = 0;
    game.playcustomlevel = 0;
    game.playassets = playassets;
    game.menustart = true;

    LevelMetaData meta;
    CliPlaytestArgs pt_args;
    if (cl.getLevelMetaDataAndPlaytestArgs("levels/special/dll.vvvvvv", meta, &pt_args)) {
        cl.ListOfMetaData.clear();
        cl.ListOfMetaData.push_back(meta);
    }
    else {
        cl.loadZips();
        if (cl.getLevelMetaDataAndPlaytestArgs("levels/special/dll.vvvvvv", meta, &pt_args)) {
            cl.ListOfMetaData.clear();
            cl.ListOfMetaData.push_back(meta);
        }
        else {
            vlog_error("Level not found");
            VVV_exit(1);
        }
    }

    bool savefileplaytest = false;
    int savex = 0;
    int savey = 0;
    int saverx = 0;
    int savery = 0;
    int savegc = 0;
    int savemusic = -1;

    if (pt_args.valid)
    {
        savefileplaytest = true;
        savex = pt_args.x;
        savey = pt_args.y;
        saverx = pt_args.rx;
        savery = pt_args.ry;
        savegc = pt_args.gc;
        savemusic = pt_args.music;
    }

    game.loadcustomlevelstats();
    game.customleveltitle = cl.ListOfMetaData[game.playcustomlevel].title;
    game.customlevelfilename = cl.ListOfMetaData[game.playcustomlevel].filename;
    if (savefileplaytest) {
        game.playx = savex;
        game.playy = savey;
        game.playrx = saverx;
        game.playry = savery;
        game.playgc = savegc;
        game.playmusic = savemusic;
        game.cliplaytest = true;
        script.startgamemode(Start_CUSTOM_QUICKSAVE);
    }
    else {
        script.startgamemode(Start_CUSTOM);
    }

    graphics.fademode = FADE_NONE;

    if (cl.injected_level != NULL)
    {
        // we injected a level, so let's free the memory we used for that
        SDL_free(cl.injected_level);
        cl.injected_level = NULL;
        cl.injected_level_len = 0;
    }

    return;
}
