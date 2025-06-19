#define GRAPHICS_DEFINITION
#include "Graphics.h"

#include <SDL.h>

#include "Alloc.h"
#include "Constants.h"
#include "CustomLevels.h"
#include "Editor.h"
#include "Entity.h"
#include "Exit.h"
#include "FileSystemUtils.h"
#include "FishingRodItem.h"
#include "Font.h"
#include "GraphicsUtil.h"
#include "IMERender.h"
#include "ItemHelpers.h"
#include "Localization.h"
#include "Map.h"
#include "Maths.h"
#include "Music.h"
#include "RoomnameTranslator.h"
#include "Screen.h"
#include "Script.h"
#include "UtilityClass.h"
#include "VFormat.h"
#include "Vlogging.h"

void Graphics::init(void)
{
    flipmode = false;
    setRect(tiles_rect, 0, 0, 8, 8);
    setRect(sprites_rect, 0, 0, 32, 32);
    setRect(footerrect, 0, 230, 320, 10);
    setRect(tele_rect, 0, 0, 96, 96);

    // We initialise a few things

    linestate = 0;

    trinketcolset = false;

    showcutscenebars = false;
    showcutscenebars_fast = false;
    setbars(0);
    oldcutscenebarspos_fast = 0;
    cutscenebarspos = 0;
    notextoutline = false;

    flipmode = false;
    setflipmode = false;

    // Initialize backgrounds
    for (int i = 0; i < numstars; i++)
    {
        const SDL_Rect star = {(int) (fRandom() * 320), (int) (fRandom() * 240), 2, 2};
        stars[i] = star;
        starsspeed[i] = 4 + (fRandom() * 4);
    }

    for (int i = 0; i < numbackboxes; i++)
    {
        SDL_Rect bb;
        int bvx = 0;
        int bvy = 0;
        if (fRandom() * 100 > 50)
        {
            bvx = 9 - (fRandom() * 19);
            if (bvx > -6 && bvx < 6) bvx = 6;
            bvx = bvx * 1.5;
            setRect(bb, fRandom() * 320, fRandom() * 240, 32, 12);
        }
        else
        {
            bvy = 9 - (fRandom() * 19);
            if (bvy > -6 && bvy < 6) bvy = 6;
            bvy = bvy * 1.5;
            setRect(bb, fRandom() * 320, fRandom() * 240, 12, 32);
        }
        backboxes[i] = bb;
        backboxvx[i] = bvx;
        backboxvy[i] = bvy;
    }

    backboxmult = 0.5 + ((fRandom() * 100) / 200);

    backoffset = 0;
    foregrounddrawn = false;
    backgrounddrawn = false;

    warpskip = 0;

    spcol = 0;
    spcoldel = 0;
    rcol = 0;

    crewframe = 0;
    crewframedelay = 4;
    menuoffset = 0;
    oldmenuoffset = 0;
    resumegamemode = false;

    //Fading stuff
    SDL_memset(fadebars, 0, sizeof(fadebars));

    setfade(0);
    fademode = FADE_NONE;
    ingame_fademode = FADE_NONE;

    // initialize everything else to zero
    m = 0;
    linedelay = 0;
    gameTexture = NULL;
    gameplayTexture = NULL;
    menuTexture = NULL;
    ghostTexture = NULL;
    tempShakeTexture = NULL;
    backgroundTexture = NULL;
    backgroundTileTexture = NULL;
    foregroundTexture = NULL;

    shadowTexture = NULL;
    darknessTexture = NULL;

    waterLineTexture = NULL;
    waterLineUnderneathTexture = NULL;
    waterTexture = NULL;

    tempScreenshot = NULL;
    tempScreenshot2x = NULL;
    towerbg = TowerBG();
    titlebg = TowerBG();
    trinketr = 0;
    trinketg = 0;
    trinketb = 0;

    translucentroomname = false;

    alpha = 1.0f;

    screenshake_x = 0;
    screenshake_y = 0;

    SDL_zero(col_crewred);
    SDL_zero(col_crewyellow);
    SDL_zero(col_crewgreen);
    SDL_zero(col_crewcyan);
    SDL_zero(col_crewblue);
    SDL_zero(col_crewpurple);
    SDL_zero(col_crewinactive);
    SDL_zero(col_clock);
    SDL_zero(col_trinket);
    col_tr = 0;
    col_tg = 0;
    col_tb = 0;

    kludgeswnlinewidth = false;

    tiles1_mounted = false;
    tiles2_mounted = false;
    minimap_mounted = false;

    gamecomplete_mounted = false;
    levelcomplete_mounted = false;
    flipgamecomplete_mounted = false;
    fliplevelcomplete_mounted = false;
}

void Graphics::destroy(void)
{
#define CLEAR_ARRAY(name) \
    for (size_t i = 0; i < name.size(); i += 1) \
    { \
        VVV_freefunc(SDL_FreeSurface, name[i]); \
    } \
    name.clear();

    CLEAR_ARRAY(sprites_surf)
    CLEAR_ARRAY(flipsprites_surf)

#undef CLEAR_ARRAY
}

static SDL_Surface* tempFilterSrc = NULL;
static SDL_Surface* tempFilterDest = NULL;

void Graphics::create_buffers(void)
{
#define CREATE_TEXTURE_WITH_DIMENSIONS(w, h) \
    SDL_CreateTexture( \
        gameScreen.m_renderer, \
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, \
        (w), (h) \
    )
#define CREATE_TEXTURE \
    CREATE_TEXTURE_WITH_DIMENSIONS(SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS)
#define CREATE_SCROLL_TEXTURE \
    CREATE_TEXTURE_WITH_DIMENSIONS(SCREEN_WIDTH_PIXELS + 16, SCREEN_WIDTH_PIXELS + 16)

    gameTexture = CREATE_TEXTURE;
    gameplayTexture = CREATE_TEXTURE;
    menuTexture = CREATE_TEXTURE;
    ghostTexture = CREATE_TEXTURE;
    tempShakeTexture = CREATE_TEXTURE;
    backgroundTileTexture = CREATE_TEXTURE;
    foregroundTexture = CREATE_TEXTURE;
    shadowTexture = CREATE_TEXTURE;
    darknessTexture = CREATE_TEXTURE;
    backgroundTexture = CREATE_SCROLL_TEXTURE;
    tempScrollingTexture = CREATE_SCROLL_TEXTURE;
    towerbg.texture = CREATE_SCROLL_TEXTURE;
    titlebg.texture = CREATE_SCROLL_TEXTURE;

    waterLineTexture = CREATE_TEXTURE;
    waterLineUnderneathTexture = CREATE_TEXTURE;
    waterTexture = CREATE_TEXTURE;

#undef CREATE_SCROLL_TEXTURE
#undef CREATE_TEXTURE
#undef CREATE_TEXTURE_WITH_DIMENSIONS

    SDL_SetTextureScaleMode(
        gameTexture,
        gameScreen.isFiltered ? SDL_ScaleModeLinear : SDL_ScaleModeNearest
    );

    SDL_SetTextureScaleMode(
        tempShakeTexture,
        gameScreen.isFiltered ? SDL_ScaleModeLinear : SDL_ScaleModeNearest
    );
}

void Graphics::destroy_buffers(void)
{
    VVV_freefunc(SDL_DestroyTexture, gameTexture);
    VVV_freefunc(SDL_DestroyTexture, gameplayTexture);
    VVV_freefunc(SDL_DestroyTexture, menuTexture);
    VVV_freefunc(SDL_DestroyTexture, backgroundTileTexture);
    VVV_freefunc(SDL_DestroyTexture, ghostTexture);
    VVV_freefunc(SDL_DestroyTexture, tempShakeTexture);
    VVV_freefunc(SDL_DestroyTexture, foregroundTexture);
    VVV_freefunc(SDL_DestroyTexture, backgroundTexture);
    VVV_freefunc(SDL_DestroyTexture, tempScrollingTexture);
    VVV_freefunc(SDL_DestroyTexture, towerbg.texture);
    VVV_freefunc(SDL_DestroyTexture, titlebg.texture);

    VVV_freefunc(SDL_DestroyTexture, waterLineTexture);
    VVV_freefunc(SDL_DestroyTexture, waterLineUnderneathTexture);
    VVV_freefunc(SDL_DestroyTexture, waterTexture);

    VVV_freefunc(SDL_DestroyTexture, shadowTexture);
    VVV_freefunc(SDL_DestroyTexture, darknessTexture);

    VVV_freefunc(SDL_FreeSurface, tempFilterSrc);
    VVV_freefunc(SDL_FreeSurface, tempFilterDest);
    VVV_freefunc(SDL_FreeSurface, tempScreenshot);
    VVV_freefunc(SDL_FreeSurface, tempScreenshot2x);
}

void Graphics::drawspritesetcol(int x, int y, int t, int c)
{
    draw_grid_tile(grphx.im_sprites, t, x, y, sprites_rect.w, sprites_rect.h, getcol(c));
}

void Graphics::updatetitlecolours(void)
{
    col_crewred = getcol(15);
    col_crewyellow = getcol(14);
    col_crewgreen = getcol(13);
    col_crewcyan = getcol(0);
    col_crewblue = getcol(16);
    col_crewpurple = getcol(20);
    col_crewinactive = getcol(19);

    col_clock = getcol(18);
    col_trinket = getcol(18);
}


void Graphics::map_tab(int opt, const char* text, bool selected /*= false*/)
{
    int x = opt*80 + 40;
    if (selected)
    {
        char buffer[SCREEN_WIDTH_CHARS + 1];
        vformat_buf(buffer, sizeof(buffer), loc::get_langmeta()->menu_select_tight.c_str(), "label:str", text);
        font::print(PR_CEN | PR_CJK_LOW | PR_RTL_XFLIP, x, 220, buffer, 196, 196, 255 - help.glow);
    }
    else
    {
        font::print(PR_CEN | PR_CJK_LOW | PR_RTL_XFLIP, x, 220, text, 64, 64, 64);
    }
}

void Graphics::map_option(int opt, int num_opts, const std::string& text, bool selected /*= false*/)
{
    int x = 80 + opt*32;
    int y = 136; // start from middle of menu

    int yoff = -(num_opts * 12) / 2; // could be simplified to -num_opts * 6, this conveys my intent better though
    yoff += opt * 12;

    if (flipmode)
    {
        y -= yoff; // going down, which in Flip Mode means going up
        y -= 40;
    }
    else
    {
        y += yoff; // going up
    }

    if (selected)
    {
        std::string text_upper(loc::toupper(text));

        char buffer[SCREEN_WIDTH_CHARS + 1];
        vformat_buf(buffer, sizeof(buffer), loc::get_langmeta()->menu_select.c_str(), "label:str", text_upper.c_str());

        // Account for brackets
        x -= (font::len(0, buffer) - font::len(0, text_upper.c_str())) / 2;

        font::print(PR_RTL_XFLIP, x, y, buffer, 196, 196, 255 - help.glow);
    }
    else
    {
        font::print(PR_RTL_XFLIP, x, y, loc::remove_toupper_escape_chars(text), 96, 96, 96);
    }
}


void Graphics::printcrewname( int x, int y, int t )
{
    //Print the name of crew member t in the right colour
    const uint32_t flags = flipmode ? PR_CJK_LOW : PR_CJK_HIGH;
    switch(t)
    {
    case 0:
        font::print(flags, x, y, loc::gettext("Viridian"), 16, 240, 240);
        break;
    case 1:
        font::print(flags, x, y, loc::gettext("Violet"), 240, 16, 240);
        break;
    case 2:
        font::print(flags, x, y, loc::gettext("Vitellary"), 240, 240, 16);
        break;
    case 3:
        font::print(flags, x, y, loc::gettext("Vermilion"), 240, 16, 16);
        break;
    case 4:
        font::print(flags, x, y, loc::gettext("Verdigris"), 16, 240, 16);
        break;
    case 5:
        font::print(flags, x, y, loc::gettext("Victoria"), 16, 16, 240);
        break;
    }
}

void Graphics::printcrewnamedark( int x, int y, int t )
{
    //Print the name of crew member t as above, but in black and white
    const uint32_t flags = flipmode ? PR_CJK_LOW : PR_CJK_HIGH;
    switch(t)
    {
    case 0:
        font::print(flags, x, y, loc::gettext("Viridian"), 128,128,128);
        break;
    case 1:
        font::print(flags, x, y, loc::gettext("Violet"), 128,128,128);
        break;
    case 2:
        font::print(flags, x, y, loc::gettext("Vitellary"), 128,128,128);
        break;
    case 3:
        font::print(flags, x, y, loc::gettext("Vermilion"), 128,128,128);
        break;
    case 4:
        font::print(flags, x, y, loc::gettext("Verdigris"), 128,128,128);
        break;
    case 5:
        font::print(flags, x, y, loc::gettext("Victoria"), 128,128,128);
        break;
    }
}

void Graphics::printcrewnamestatus( int x, int y, int t, bool rescued )
{
    //Print the status of crew member t in the right colour
    int r, g, b;
    char gender;

    switch(t)
    {
    case 0:
        r = 12; g = 140, b = 140;
        gender = 3;
        break;
    case 1:
        r = 140; g = 12; b = 140;
        gender = 2;
        break;
    case 2:
        r = 140; g = 140; b = 12;
        gender = 1;
        break;
    case 3:
        r = 140; g = 12; b = 12;
        gender = 1;
        break;
    case 4:
        r = 12; g = 140; b = 12;
        gender = 1;
        break;
    case 5:
        r = 12; g = 12; b = 140;
        gender = 2;
        break;
    default:
        return;
    }

    const char* status_text;
    if (gender == 3 && rescued)
    {
        status_text = loc::gettext("(that's you!)");
    }
    else if (rescued)
    {
        status_text = loc::gettext_case("Exploring!", gender);
    }
    else
    {
        r = 64; g = 64; b = 64;
        status_text = loc::gettext_case("Missing...", gender);
    }

    font::print(flipmode ? PR_CJK_HIGH : PR_CJK_LOW, x, y, status_text, r, g, b);
}

void Graphics::print_level_creator(
    uint32_t print_flags,
    const int y,
    const std::string& creator,
    const uint8_t r,
    const uint8_t g,
    const uint8_t b
) {
    /* We now display a face instead of "by {author}" for several reasons:
     * - "by" may be in a different language than the author and look weird ("por various people")
     * - "by" will be longer in different languages and break the limit that levels assume
     * - "by" and author may need mutually incompatible fonts, e.g. Japanese level in Korean VVVVVV
     * - avoids likely grammar problems: male/female difference, name inflection in user-written text...
     * - it makes sense to make it a face
     * - if anyone is sad about this decision, the happy face will cheer them up anyway :D */
    int width_for_face = 17;
    int total_width = width_for_face + font::len(print_flags, creator.c_str());
    int face_x, text_x, sprite_x;
    int offset_x = -7;
    if (!font::is_rtl(print_flags))
    {
        face_x = (SCREEN_WIDTH_PIXELS - total_width) / 2;
        text_x = face_x + width_for_face;
        sprite_x = 0;
    }
    else
    {
        face_x = (SCREEN_WIDTH_PIXELS + total_width) / 2;
        text_x = face_x - width_for_face;
        face_x -= 10; // sprite origin
        sprite_x = 96;
        print_flags |= PR_RIGHT;
    }
    set_texture_color_mod(grphx.im_sprites, r, g, b);
    draw_texture_part(grphx.im_sprites, face_x + offset_x, y - 3, sprite_x, 0, 24, 12, 1, 1);
    set_texture_color_mod(grphx.im_sprites, 255, 255, 255);
    font::print(print_flags, text_x, y, creator, r, g, b);
}

int Graphics::set_render_target(SDL_Texture* texture)
{
    const int result = SDL_SetRenderTarget(gameScreen.m_renderer, texture);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not set render target: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::set_texture_color_mod(SDL_Texture* texture, const Uint8 r, const Uint8 g, const Uint8 b)
{
    const int result = SDL_SetTextureColorMod(texture, r, g, b);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not set texture color mod: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::set_texture_alpha_mod(SDL_Texture* texture, const Uint8 alpha)
{
    const int result = SDL_SetTextureAlphaMod(texture, alpha);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not set texture alpha mod: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::query_texture(SDL_Texture* texture, Uint32* format, int* access, int* w, int* h)
{
    const int result = SDL_QueryTexture(texture, format, access, w, h);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not query texture: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::set_blendmode(const SDL_BlendMode blendmode)
{
    const int result = SDL_SetRenderDrawBlendMode(gameScreen.m_renderer, blendmode);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not set draw mode: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::set_blendmode(SDL_Texture* texture, const SDL_BlendMode blendmode)
{
    const int result = SDL_SetTextureBlendMode(texture, blendmode);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not set texture blend mode: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::clear(const int r, const int g, const int b, const int a)
{
    set_color(r, g, b, a);

    const int result = SDL_RenderClear(gameScreen.m_renderer);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not clear current render target: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::clear(void)
{
    return clear(0, 0, 0, 255);
}

bool Graphics::substitute(SDL_Texture** texture)
{
    /* Either keep the given texture the same and return false,
     * or substitute it for a translation and return true. */

    if (loc::english_sprites)
    {
        return false;
    }

    SDL_Texture* subst = NULL;

    if (*texture == grphx.im_sprites)
    {
        subst = grphx.im_sprites_translated;
    }
    else if (*texture == grphx.im_flipsprites)
    {
        subst = grphx.im_flipsprites_translated;
    }

    if (subst == NULL)
    {
        return false;
    }

    // Apply the same colors as on the original
    Uint8 r, g, b, a;
    SDL_GetTextureColorMod(*texture, &r, &g, &b);
    SDL_GetTextureAlphaMod(*texture, &a);
    set_texture_color_mod(subst, r, g, b);
    set_texture_alpha_mod(subst, a);

    *texture = subst;
    return true;
}

void Graphics::post_substitute(SDL_Texture* subst)
{
    set_texture_color_mod(subst, 255, 255, 255);
    set_texture_alpha_mod(subst, 255);
}

int Graphics::copy_texture(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dest)
{
    bool is_substituted = substitute(&texture);

    const int result = SDL_RenderCopy(gameScreen.m_renderer, texture, src, dest);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not copy texture: %s", SDL_GetError()));
    }

    if (is_substituted)
    {
        post_substitute(texture);
    }

    return result;
}

int Graphics::copy_texture(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dest, const double angle, const SDL_Point* center, const SDL_RendererFlip flip)
{
    bool is_substituted = substitute(&texture);

    const int result = SDL_RenderCopyEx(gameScreen.m_renderer, texture, src, dest, angle, center, flip);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not copy texture: %s", SDL_GetError()));
    }

    if (is_substituted)
    {
        post_substitute(texture);
    }

    return result;
}

int Graphics::set_color(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
{
    const int result = SDL_SetRenderDrawColor(gameScreen.m_renderer, r, g, b, a);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not set draw color: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::set_color(const Uint8 r, const Uint8 g, const Uint8 b)
{
    return set_color(r, g, b, 255);
}

int Graphics::set_color(const SDL_Color color)
{
    return set_color(color.r, color.g, color.b, color.a);
}

int Graphics::fill_rect(const SDL_Rect* rect)
{
    const int result = SDL_RenderFillRect(gameScreen.m_renderer, rect);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not draw filled rectangle: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::fill_rect(const SDL_Rect* rect, const int r, const int g, const int b, const int a)
{
    set_color(r, g, b, a);
    return fill_rect(rect);
}

int Graphics::fill_rect(const SDL_Rect* rect, const int r, const int g, const int b)
{
    return fill_rect(rect, r, g, b, 255);
}

int Graphics::fill_rect(const int r, const int g, const int b)
{
    return fill_rect(NULL, r, g, b, 255);
}

int Graphics::fill_rect(const SDL_Rect* rect, const SDL_Color color)
{
    return fill_rect(rect, color.r, color.g, color.b, color.a);
}

int Graphics::fill_rect(const int x, const int y, const int w, const int h, const int r, const int g, const int b, const int a)
{
    const SDL_Rect rect = {x, y, w, h};
    return fill_rect(&rect, r, g, b, a);
}

int Graphics::fill_rect(const int x, const int y, const int w, const int h, const int r, const int g, const int b)
{
    return fill_rect(x, y, w, h, r, g, b, 255);
}

int Graphics::fill_rect(const SDL_Color color)
{
    return fill_rect(NULL, color);
}

int Graphics::fill_rect(const int x, const int y, const int w, const int h, const SDL_Color color)
{
    return fill_rect(x, y, w, h, color.r, color.g, color.b, color.a);
}

int Graphics::draw_rect(const SDL_Rect* rect)
{
    const int result = SDL_RenderDrawRect(gameScreen.m_renderer, rect);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not draw rectangle: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::draw_rect(const SDL_Rect* rect, const int r, const int g, const int b, const int a)
{
    set_color(r, g, b, a);
    return draw_rect(rect);
}

int Graphics::draw_rect(const SDL_Rect* rect, const int r, const int g, const int b)
{
    return draw_rect(rect, r, g, b, 255);
}

int Graphics::draw_rect(const SDL_Rect* rect, const SDL_Color color)
{
    return draw_rect(rect, color.r, color.g, color.b, color.a);
}

int Graphics::draw_rect(const int x, const int y, const int w, const int h, const int r, const int g, const int b, const int a)
{
    const SDL_Rect rect = {x, y, w, h};
    return draw_rect(&rect, r, g, b, a);
}

int Graphics::draw_rect(const int x, const int y, const int w, const int h, const int r, const int g, const int b)
{
    return draw_rect(x, y, w, h, r, g, b, 255);
}

int Graphics::draw_rect(const int x, const int y, const int w, const int h, const SDL_Color color)
{
    return draw_rect(x, y, w, h, color.r, color.g, color.b, color.a);
}

int Graphics::draw_lines(const SDL_Point* points, const int count)
{
    const int result = SDL_RenderDrawLines(gameScreen.m_renderer, points, count);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not draw lines: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::draw_line(const int x, const int y, const int x2, const int y2)
{
    const int result = SDL_RenderDrawLine(gameScreen.m_renderer, x, y, x2, y2);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not draw line: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::draw_points(const SDL_Point* points, const int count)
{
    const int result = SDL_RenderDrawPoints(gameScreen.m_renderer, points, count);
    if (result != 0)
    {
        WHINE_ONCE_ARGS(("Could not draw points: %s", SDL_GetError()));
    }
    return result;
}

int Graphics::draw_points(const SDL_Point* points, const int count, const int r, const int g, const int b)
{
    set_color(r, g, b);
    return draw_points(points, count);
}

void Graphics::draw_sprite(const int x, const int y, const int t, const int r, const int g, const int b)
{
    draw_grid_tile(grphx.im_sprites, t, x, y, sprites_rect.w, sprites_rect.h, r, g, b);
}

void Graphics::draw_sprite(const int x, const int y, const int t, const SDL_Color color)
{
    draw_grid_tile(grphx.im_sprites, t, x, y, sprites_rect.w, sprites_rect.h, color);
}

void Graphics::draw_flipsprite(const int x, const int y, const int t, const SDL_Color color)
{
    draw_grid_tile(grphx.im_flipsprites, t, x, y, sprites_rect.w, sprites_rect.h, color);
}

void Graphics::scroll_texture(SDL_Texture* texture, SDL_Texture* temp, const int x, const int y)
{
    SDL_Texture* target = SDL_GetRenderTarget(gameScreen.m_renderer);
    SDL_Rect texture_rect = {0, 0, 0, 0};
    SDL_QueryTexture(texture, NULL, NULL, &texture_rect.w, &texture_rect.h);

    const SDL_Rect src = {0, 0, texture_rect.w, texture_rect.h};
    const SDL_Rect dest = {x, y, texture_rect.w, texture_rect.h};

    set_render_target(temp);
    clear();
    copy_texture(texture, &src, &dest);
    set_render_target(target);
    copy_texture(temp, &src, &src);
}

bool Graphics::shouldrecoloroneway(const int tilenum, const bool mounted)
{
    return (tilenum >= 14 && tilenum <= 17
    && (!mounted
    || cl.onewaycol_override));
}

bool Graphics::isbg(int t)
{
    // ok, let's just copy solid tiles...

    if (map.towermode)
    {
        if (t >= 12 && t <= 27) return false;
    }
    else if (map.tileset == 2)
    {
        if (t >= 12 && t <= 27) return false;
    }
    else
    {
        if (t == 1) return false;
        if (map.tileset == 0 && t == 59) return false;
        if (t >= 80 && t < 680) return false;
        if (t == 740 && map.tileset == 1) return false;
    }

    // ok, so the tile is nonsolid. some should still show up in front of entities though, like the grass! so let's add a special case for those:
    if (map.tileset == 1)
    {
        if (t >= 6 && t <= 9) return false; // spikes
        if (t >= 49 && t <= 80) return false; // all spikes
        if (t >= 27 && t <= 39) return false; // extra beach spikes
        if (t >= 706 && t <= 718) return false; // extra grass spikes
        if (t >= 1080 && t <= 1087) return false; // extra grass spikes
        if (t >= 1124 && t <= 1127) return false; // extra grass spikes
        // GRASS:

        if (t == 941 || t == 942 || t == 981 || t == 982) return false; // grass-to-wall tiles
        if (t >= 944 && t <= 949) return false; // grass (top)
        if (t >= 984 && t <= 989) return false; // grass (bottom)
        if (t >= 1024 && t <= 1033) return false; // literally every other decal (row 1)
        if (t >= 1064 && t <= 1073) return false; // literally every other decal (row 2)

        // SAND:
        if (t == 888 || t == 889) return false; // corner tiles for rocks (row 1)
        if (t == 928 || t == 929) return false; // corner tiles for rocks (row 2)
        if (t >= 891 && t <= 896) return false; // rock (top)
        if (t >= 931 && t <= 936) return false; // rock (bottom)
        if (t >= 971 && t <= 976) return false; // rock (left)
        if (t >= 1011 && t <= 1016) return false; // rock (right)

        if (t == 1048 || t == 1049) return false; // corner tiles for sand (row 1)
        if (t == 1088 || t == 1089) return false; // corner tiles for sand (row 2)

        if (t >= 1051 && t <= 1054) return false; // sand (top)
        if (t >= 1091 && t <= 1094) return false; // sand (bottom)
        if (t >= 1131 && t <= 1134) return false; // sand (left)
        if (t >= 1171 && t <= 1174) return false; // sand (right)

        if (t >= 1058 && t <= 1061) return false; // corner connectors (row 1)
        if (t >= 1098 && t <= 1101) return false; // corner connectors (row 2)
    }

    return true;
}

void Graphics::drawtile(int x, int y, int t)
{
    if (shouldrecoloroneway(t, tiles1_mounted))
    {
        draw_grid_tile(grphx.im_tiles_tint, t, x, y, tiles_rect.w, tiles_rect.h, cl.getonewaycol());
    }
    else
    {
        draw_grid_tile(grphx.im_tiles, t, x, y, tiles_rect.w, tiles_rect.h);
    }
}


void Graphics::drawtile2(int x, int y, int t)
{
    if (shouldrecoloroneway(t, tiles2_mounted))
    {
        draw_grid_tile(grphx.im_tiles2_tint, t, x, y, tiles_rect.w, tiles_rect.h, cl.getonewaycol());
    }
    else
    {
        draw_grid_tile(grphx.im_tiles2, t, x, y, tiles_rect.w, tiles_rect.h);
    }
}

void Graphics::drawtile3(int x, int y, int t, int off, int height_subtract /*= 0*/)
{
    t += off * 30;

    // Can't use drawgridtile because we want to draw a slice of the tile,
    // so do the logic ourselves (except include height_subtract in the final call)

    int width;
    if (query_texture(grphx.im_tiles3, NULL, NULL, &width, NULL) != 0)
    {
        return;
    }
    const int x2 = (t % (width / 8)) * 8;
    const int y2 = (t / (width / 8)) * 8;
    draw_texture_part(grphx.im_tiles3, x, y, x2, y2, 8, 8 - height_subtract, 1, 1);
}

const char* Graphics::textbox_line(
    char* buffer,
    const size_t buffer_len,
    const size_t textbox_i,
    const size_t line_i
) {
    /* Gets a line in a textbox, accounting for filling button placeholders like {b_map}.
     * Takes a buffer as an argument, but DOESN'T ALWAYS write to that buffer.
     * Always use the return value! ^^
     * Does not check boundaries. */

    const char* line = textboxes[textbox_i].lines[line_i].c_str();
    if (!textboxes[textbox_i].fill_buttons)
    {
        return line;
    }

    vformat_buf(buffer, buffer_len,
        line,
        "b_act:but,"
        "b_int:but,"
        "b_map:but,"
        "b_res:but,"
        "b_esc:but",
        vformat_button(ActionSet_InGame, Action_InGame_ACTION),
        vformat_button(ActionSet_InGame, Action_InGame_Interact),
        vformat_button(ActionSet_InGame, Action_InGame_Map),
        vformat_button(ActionSet_InGame, Action_InGame_Restart),
        vformat_button(ActionSet_InGame, Action_InGame_Esc)
    );
    return buffer;
}

void Graphics::drawgui(void)
{
    int text_sign;

    if (flipmode)
    {
        text_sign = -1;
    }
    else
    {
        text_sign = 1;
    }

    //Draw all the textboxes to the screen
    for (size_t i = 0; i < textboxes.size(); i++)
    {
        int text_yoff;
        int yp;
        int font_height = font::height(textboxes[i].print_flags);
        if (flipmode)
        {
            text_yoff = 8 + (textboxes[i].lines.size() - 1) * (font_height + textboxes[i].linegap);
        }
        else
        {
            text_yoff = 8;
        }

        yp = textboxes[i].yp;
        if (flipmode && textboxes[i].flipme)
        {
            yp = SCREEN_HEIGHT_PIXELS - yp - 16 - textboxes[i].lines.size() * (font_height + textboxes[i].linegap);
        }

        char buffer[SCREEN_WIDTH_CHARS + 1];

        int w = textboxes[i].w;
        if (textboxes[i].fill_buttons)
        {
            /* If we can fill in buttons, the width of the box may change...
             * This is Violet's fault. She decided to say a button name out loud. */
            int max = 0;
            for (size_t j = 0; j < textboxes[i].lines.size(); j++)
            {
                int len = font::len(textboxes[i].print_flags, textbox_line(buffer, sizeof(buffer), i, j));
                if (len > max)
                {
                    max = len;
                }
            }
            w = max + 16;
        }

        uint32_t print_flags = textboxes[i].print_flags | PR_CJK_LOW;
        int text_xp;

        if (font::is_rtl(print_flags))
        {
            print_flags |= PR_RIGHT;
            text_xp = textboxes[i].xp + w - 8;
        }
        else
        {
            text_xp = textboxes[i].xp + 8;
        }

        const bool transparent = (textboxes[i].r | textboxes[i].g | textboxes[i].b) == 0;

        if (transparent)
        {
            /* To avoid the outlines for different lines overlapping the text itself,
             * first draw all the outlines and then draw the text. */
            size_t j;
            for (j = 0; j < textboxes[i].lines.size(); j++)
            {
                const int x = text_xp;
                const int y = yp + text_yoff + text_sign * (j * (font_height + textboxes[i].linegap));
                if (!textboxes[i].force_outline)
                {
                    font::print(print_flags | PR_BOR, x, y, textbox_line(buffer, sizeof(buffer), i, j), 0, 0, 0);
                }
                else if (textboxes[i].outline)
                {
                    // We're forcing an outline, so we'll have to draw it ourselves instead of relying on PR_BOR.
                    font::print(print_flags, x - 1, y, textbox_line(buffer, sizeof(buffer), i, j), 0, 0, 0);
                    font::print(print_flags, x + 1, y, textbox_line(buffer, sizeof(buffer), i, j), 0, 0, 0);
                    font::print(print_flags, x, y - 1, textbox_line(buffer, sizeof(buffer), i, j), 0, 0, 0);
                    font::print(print_flags, x, y + 1, textbox_line(buffer, sizeof(buffer), i, j), 0, 0, 0);
                }
            }
            for (j = 0; j < textboxes[i].lines.size(); j++)
            {
                font::print(
                    print_flags,
                    text_xp,
                    yp + text_yoff + text_sign * (j * (font_height + textboxes[i].linegap)),
                    textbox_line(buffer, sizeof(buffer), i, j),
                    196, 196, 255 - help.glow
                );
            }
        }
        else
        {
            const float tl_lerp = lerp(textboxes[i].prev_tl, textboxes[i].tl);
            const int r = textboxes[i].r * tl_lerp;
            const int g = textboxes[i].g * tl_lerp;
            const int b = textboxes[i].b * tl_lerp;

            drawpixeltextbox(textboxes[i].xp, yp, w, textboxes[i].h, r, g, b);

            for (size_t j = 0; j < textboxes[i].lines.size(); j++)
            {
                font::print(
                    print_flags | PR_BRIGHTNESS(tl_lerp*255),
                    text_xp,
                    yp + text_yoff + text_sign * (j * (font_height + textboxes[i].linegap)),
                    textbox_line(buffer, sizeof(buffer), i, j),
                    textboxes[i].r, textboxes[i].g, textboxes[i].b
                );
            }
        }

        const bool opaque = textboxes[i].tl >= 1.0;
        const bool draw_overlays = opaque || transparent;

        if (!draw_overlays)
        {
            continue;
        }

        if (textboxes[i].image == TEXTIMAGE_LEVELCOMPLETE)
        {
            // Level complete
            const char* english = "Level Complete!";
            const char* translation = loc::gettext(english);
            if (SDL_strcmp(english, translation) != 0
                && !(flipmode && fliplevelcomplete_mounted)
                && !(!flipmode && levelcomplete_mounted)
            )
            {
                int sc = 2;
                int y = 28;
                if (font::len(0, translation) > 144)
                {
                    // We told translators how long it could be... Ah well, mitigate the damage.
                    sc = 1;
                    y += 4;
                }
                if (flipmode)
                {
                    y = 240 - y - 8 * sc;
                }
                SDL_Color color = TEXT_COLOUR("cyan");
                font::print((sc == 2 ? PR_2X : PR_1X) | PR_CEN, -1, y, translation, color.r, color.g, color.b);
            }
            else
            {
                if (flipmode)
                {
                    drawimage(IMAGE_FLIPLEVELCOMPLETE, 0, 180, true);
                }
                else
                {
                    drawimage(IMAGE_LEVELCOMPLETE, 0, 12, true);
                }
            }
        }
        else if (textboxes[i].image == TEXTIMAGE_GAMECOMPLETE)
        {
            // Game complete
            const char* english = "Game Complete!";
            const char* translation = loc::gettext(english);
            if (SDL_strcmp(english, translation) != 0
                && !(flipmode && flipgamecomplete_mounted)
                && !(!flipmode && gamecomplete_mounted)
            )
            {
                int sc = 2;
                int y = 28;
                if (font::len(0, translation) > 144)
                {
                    // We told translators how long it could be... Ah well, mitigate the damage.
                    sc = 1;
                    y += 4;
                }
                if (flipmode)
                {
                    y = 240 - y - 8 * sc;
                }
                font::print((sc == 2 ? PR_2X : PR_1X) | PR_CEN, -1, y, translation, 196, 196, 243);
            }
            else
            {
                if (flipmode)
                {
                    drawimage(IMAGE_FLIPGAMECOMPLETE, 0, 180, true);
                }
                else
                {
                    drawimage(IMAGE_GAMECOMPLETE, 0, 12, true);
                }
            }
        }

        for (size_t index = 0; index < textboxes[i].sprites.size(); index++)
        {
            TextboxSprite* sprite = &textboxes[i].sprites[index];
            int y = sprite->y + yp;

            if (flipmode)
            {
                y = yp + textboxes[i].h - sprite->y - sprites_rect.h;
            }

            draw_grid_tile(
                grphx.im_sprites,
                sprite->tile,
                sprite->x + textboxes[i].xp,
                y,
                sprites_rect.w,
                sprites_rect.h,
                getcol(sprite->col),
                1,
                (flipmode ? -1 : 1)
            );
        }

        for (size_t index = 0; index < textboxes[i].items.size(); index++)
        {
            TextboxItem* sprite = &textboxes[i].items[index];
            int y = sprite->y + yp;
            if (flipmode)
            {
                y = yp + textboxes[i].h - sprite->y - 16;
            }
            Item* item = getItem(sprite->item);
            if (item != NULL)
            {
                item->draw(sprite->x + textboxes[i].xp, y);
            }
        }
    }
}

void Graphics::updatetextboxes(void)
{
    for (size_t i = 0; i < textboxes.size(); i++)
    {
        textboxes[i].update();

        if (textboxes[i].tm == 2 && textboxes[i].tl <= 0.5)
        {
            textboxes.erase(textboxes.begin() + i);
            i--;
            continue;
        }

        if (textboxes[i].tl >= 1.0f
        && ((textboxes[i].r == 175 && textboxes[i].g == 175)
        || textboxes[i].r == 175
        || textboxes[i].g == 175
        || textboxes[i].b == 175)
        && (textboxes[i].r != 175 || textboxes[i].b != 175))
        {
            textboxes[i].rand = fRandom() * 20;
        }
    }
}

void Graphics::drawimagecol( int t, int xp, int yp, const SDL_Color ct, bool cent/*= false*/ )
{
    if (!INBOUNDS_ARR(t, images) || images[t] == NULL)
    {
        return;
    }
    SDL_Rect trect;

    trect.x = xp;
    trect.y = yp;

    if (query_texture(images[t], NULL, NULL, &trect.w, &trect.h) != 0)
    {
        return;
    }

    if (cent)
    {
        trect.x = (int) ((SCREEN_WIDTH_PIXELS - trect.w) / 2);
    }

    set_texture_color_mod(images[t], ct.r, ct.g, ct.b);
    draw_texture(images[t], trect.x, trect.y);
    set_texture_color_mod(images[t], 255, 255, 255);
}

void Graphics::drawimage( int t, int xp, int yp, bool cent/*=false*/ )
{
    if (!INBOUNDS_ARR(t, images) || images[t] == NULL)
    {
        return;
    }

    SDL_Rect trect;

    trect.x = xp;
    trect.y = yp;

    if (query_texture(images[t], NULL, NULL, &trect.w, &trect.h) != 0)
    {
        return;
    }

    if (cent)
    {
        trect.x = (int) ((SCREEN_WIDTH_PIXELS - trect.w) / 2);
    }

    draw_texture(images[t], trect.x, trect.y);
}

void Graphics::drawpartimage(const int t, const int xp, const int yp, const int wp, const int hp)
{
    if (!INBOUNDS_ARR(t, images) || images[t] == NULL)
    {
        return;
    }
    draw_texture_part(images[t], xp, yp, 0, 0, wp, hp, 1, 1);
}

void Graphics::draw_texture(SDL_Texture* image, const int x, const int y)
{
    draw_texture(image, x, y, 1, 1);
}

void Graphics::draw_texture(SDL_Texture * image, const int x, const int y, const float scalex, const float scaley)
{
    int w, h;

    if (query_texture(image, NULL, NULL, &w, &h) != 0)
    {
        return;
    }

    const SDL_Rect dstrect = { x, y, w * scalex, h * scaley };

    copy_texture(image, NULL, &dstrect);
}

void Graphics::draw_light(SDL_Texture* image, const int x, const int y, const float scalex, const float scaley, const int alpha)
{
    SDL_Texture* target = SDL_GetRenderTarget(gameScreen.m_renderer);
    set_render_target(shadowTexture);
    SDL_SetTextureBlendMode(image, SDL_BLENDMODE_BLEND);

    int w, h;

    if (query_texture(image, NULL, NULL, &w, &h) != 0)
    {
        return;
    }

    set_texture_alpha_mod(image, alpha);
    draw_texture(image, x - (w * scalex) / 2, y - (h * scaley) / 2, scalex, scaley);
    set_texture_alpha_mod(image, 255);

    SDL_SetTextureBlendMode(image, SDL_BLENDMODE_NONE);
    set_render_target(target);
}

void Graphics::draw_light(SDL_Texture* image, const int x, const int y)
{
    draw_light(image, x, y, 1, 1, 255);
}

void Graphics::draw_point_light(const int x, const int y, const int radius = 64, const int alpha = 255)
{
    int w, h;

    if (query_texture(grphx.im_light_point, NULL, NULL, &w, &h) != 0)
    {
        return;
    }

    draw_light(grphx.im_light_point, x, y, (float)radius / w, (float)radius / h, alpha);
}

void Graphics::draw_texture_part(SDL_Texture* image, const int x, const int y, const int x2, const int y2, const int w, const int h, const int scalex, const int scaley)
{
    const SDL_Rect srcrect = {x2, y2, w, h};

    int flip = SDL_FLIP_NONE;

    if (scalex < 0)
    {
        flip |= SDL_FLIP_HORIZONTAL;
    }
    if (scaley < 0)
    {
        flip |= SDL_FLIP_VERTICAL;
    }

    const SDL_Rect dstrect = {x, y, w * SDL_abs(scalex), h * SDL_abs(scaley)};

    copy_texture(image, &srcrect, &dstrect, 0, NULL, (SDL_RendererFlip) flip);
}

void Graphics::draw_grid_tile(SDL_Texture* texture, const int t, const int x, const int y, const int width, const int height, const int scalex, const int scaley)
{
    int tex_width;

    if (query_texture(texture, NULL, NULL, &tex_width, NULL) != 0)
    {
        return;
    }

    const int x2 = (t % (tex_width / width)) * width;
    const int y2 = (t / (tex_width / width)) * height;
    draw_texture_part(texture, x, y, x2, y2, width, height, scalex, scaley);
}

void Graphics::draw_grid_tile(
    SDL_Texture* texture, const int t,
    const int x, const int y, const int width, const int height
) {
    draw_grid_tile(texture, t, x, y, width, height, 1, 1);
}

void Graphics::draw_grid_tile(
    SDL_Texture* texture, const int t,
    const int x, const int y, const int width, const int height,
    const int r, const int g, const int b, const int a,
    const int scalex, const int scaley
) {
    set_texture_color_mod(texture, r, g, b);
    set_texture_alpha_mod(texture, a);
    draw_grid_tile(texture, t, x, y, width, height, scalex, scaley);
    set_texture_color_mod(texture, 255, 255, 255);
    set_texture_alpha_mod(texture, 255);
}

void Graphics::draw_grid_tile(
    SDL_Texture* texture, const int t,
    const int x, const int y, const int width, const int height,
    const int r, const int g, const int b, const int a
) {
    draw_grid_tile(texture, t, x, y, width, height, r, g, b, a, 1, 1);
}

void Graphics::draw_grid_tile(
    SDL_Texture* texture, const int t,
    const int x, const int y, const int width, const int height,
    const int r, const int g, const int b,
    const int scalex, const int scaley
) {
    draw_grid_tile(texture, t, x, y, width, height, r, g, b, 255, scalex, scaley);
}

void Graphics::draw_grid_tile(
    SDL_Texture* texture, const int t,
    const int x, const int y, const int width, const int height,
    const int r, const int g, const int b
) {
    draw_grid_tile(texture, t, x, y, width, height, r, g, b, 255);
}

void Graphics::draw_grid_tile(
    SDL_Texture* texture, const int t,
    const int x, const int y, const int width, const int height,
    const SDL_Color color,
    const int scalex, const int scaley
) {
    draw_grid_tile(texture, t, x, y, width, height, color.r, color.g, color.b, color.a, scalex, scaley);
}

void Graphics::draw_grid_tile(
    SDL_Texture* texture, const int t,
    const int x, const int y, const int width, const int height,
    const SDL_Color color
) {
    draw_grid_tile(texture, t, x, y, width, height, color, 1, 1);
}

void Graphics::draw_region_image(int t, int xp, int yp, int wp, int hp)
{
    if (!INBOUNDS_ARR(t, customminimaps) || customminimaps[t] == NULL)
    {
        return;
    }
    draw_texture_part(customminimaps[t], xp, yp, 0, 0, wp, hp, 1, 1);
}

void Graphics::cutscenebars(void)
{
    const int usethispos = lerp(oldcutscenebarspos, cutscenebarspos);
    if (showcutscenebars)
    {
        fill_rect(0, 0, usethispos, 16, 0, 0, 0);
        fill_rect(360 - usethispos, 224, usethispos, 16, 0, 0, 0);
    }
    else if (cutscenebarspos > 0) //disappearing
    {
        //draw
        fill_rect(0, 0, usethispos, 16, 0, 0, 0);
        fill_rect(360 - usethispos, 224, usethispos, 16, 0, 0, 0);
    }

    const float usethispos2 = lerp(oldcutscenebarspos_fast, cutscenebarspos_fast);
    if (showcutscenebars_fast)
    {
        const int off = easeOutCubic(usethispos2) * 16;

        fill_rect(0, -16 + off, 320, 16, 0, 0, 0);
        fill_rect(0, 240 - off, 320, 16, 0, 0, 0);
    }
    else if (cutscenebarspos_fast > 0) //disappearing
    {
        const int off = easeInCubic(usethispos2) * 16;
        fill_rect(0, -16 + off, 320, 16, 0, 0, 0);
        fill_rect(0, 240 - off, 320, 16, 0, 0, 0);
    }
}

void Graphics::cutscenebarstimer(void)
{
    oldcutscenebarspos = cutscenebarspos;
    if (showcutscenebars)
    {
        cutscenebarspos += 25;
        cutscenebarspos = SDL_min(cutscenebarspos, 361);
    }
    else if (cutscenebarspos > 0)
    {
        //disappearing
        cutscenebarspos -= 25;
        cutscenebarspos = SDL_max(cutscenebarspos, 0);
    }

    oldcutscenebarspos_fast = cutscenebarspos_fast;
    if (showcutscenebars_fast)
    {
        cutscenebarspos_fast += 0.15;
        cutscenebarspos_fast = SDL_min(cutscenebarspos_fast, 1);
    }
    else if (cutscenebarspos_fast > 0)
    {
        //disappearing
        cutscenebarspos_fast -= 0.15;
        cutscenebarspos_fast = SDL_max(cutscenebarspos_fast, 0);
    }
}

void Graphics::setbars(const int position)
{
    cutscenebarspos = position;
    oldcutscenebarspos = position;
}

void Graphics::drawcrewman(int x, int y, int t, bool act, bool noshift /*=false*/)
{
    if (!act)
    {
        if (noshift)
        {
            if (flipmode)
            {
                draw_sprite(x, y, 14, col_crewinactive);
            }
            else
            {
                draw_sprite(x, y, 12, col_crewinactive);
            }
        }
        else
        {
            if (flipmode)
            {
                draw_sprite(x - 8, y, 14, col_crewinactive);
            }
            else
            {
                draw_sprite(x - 8, y, 12, col_crewinactive);
            }
        }
    }
    else
    {
        if (flipmode) crewframe += 6;

        switch(t)
        {
        case 0:
            draw_sprite(x, y, crewframe, col_crewcyan);
            break;
        case 1:
            draw_sprite(x, y, crewframe, col_crewpurple);
            break;
        case 2:
            draw_sprite(x, y, crewframe, col_crewyellow);
            break;
        case 3:
            draw_sprite(x, y, crewframe, col_crewred);
            break;
        case 4:
            draw_sprite(x, y, crewframe, col_crewgreen);
            break;
        case 5:
            draw_sprite(x, y, crewframe, col_crewblue);
            break;
        }

        if (flipmode) crewframe -= 6;
    }
}

void Graphics::drawpixeltextbox(
    const int x,
    const int y,
    const int w,
    const int h,
    const int r,
    const int g,
    const int b
) {
    int k;

    fill_rect(x, y, w, h, r / 6, g / 6, b / 6);

    /* Horizontal tiles */
    for (k = 0; k < w / 8 - 2; ++k)
    {
        drawcoloredtile(x + 8 + k * 8, y, 41, r, g, b);
        drawcoloredtile(x + 8 + k * 8, y + h - 8, 46, r, g, b);
    }

    if (w % 8 != 0)
    {
        /* Fill in horizontal gap */
        drawcoloredtile(x + w - 16, y, 41, r, g, b);
        drawcoloredtile(x + w - 16, y + h - 8, 46, r, g, b);
    }

    /* Vertical tiles */
    for (k = 0; k < h / 8 - 2; ++k)
    {
        drawcoloredtile(x, y + 8 + k * 8, 43, r, g, b);
        drawcoloredtile(x + w - 8, y + 8 + k * 8, 44, r, g, b);
    }

    if (h % 8 != 0)
    {
        /* Fill in vertical gap */
        drawcoloredtile(x, y + h - 16, 43, r, g, b);
        drawcoloredtile(x + w - 8, y + h - 16, 44, r, g, b);
    }

    /* Corners */
    drawcoloredtile(x, y, 40, r, g, b);
    drawcoloredtile(x + w - 8, y, 42, r, g, b);
    drawcoloredtile(x, y + h - 8, 45, r, g, b);
    drawcoloredtile(x + w - 8, y + h - 8, 47, r, g, b);
}

void Graphics::textboxactive(void)
{
    //Remove all but the most recent textbox
    for (int i = 0; i < (int) textboxes.size(); i++)
    {
        if (m != i) textboxes[i].remove();
    }
}

void Graphics::textboxremovefast(void)
{
    //Remove all textboxes
    for (size_t i = 0; i < textboxes.size(); i++)
    {
        textboxes[i].removefast();
    }
}

void Graphics::textboxremove(void)
{
    //Remove all textboxes
    for (size_t i = 0; i < textboxes.size(); i++)
    {
        textboxes[i].remove();
    }
}

void Graphics::textboxtimer(int t)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxtimer() out-of-bounds!");
        return;
    }

    textboxes[m].timer = t;
}

void Graphics::addsprite(int x, int y, int tile, int col)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("addsprite() out-of-bounds!");
        return;
    }

    textboxes[m].addsprite(x, y, tile, col);
}

void Graphics::additem(int x, int y, std::string item)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("additem() out-of-bounds!");
        return;
    }
    textboxes[m].additem(x, y, item);
}

void Graphics::setimage(TextboxImage image)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("setimage() out-of-bounds!");
        return;
    }

    textboxes[m].setimage(image);
}

void Graphics::textboxoutline(bool enabled)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxoutline() out-of-bounds!");
        return;
    }

    textboxes[m].force_outline = true;
    textboxes[m].outline = enabled;
}

void Graphics::addline( const std::string& t )
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("addline() out-of-bounds!");
        return;
    }

    textboxes[m].addline(t);
}

void Graphics::setlarge(bool large)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("setlarge() out-of-bounds!");
        return;
    }

    textboxes[m].large = large;
}

void Graphics::textboxapplyposition(void)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxapplyposition() out-of-bounds!");
        return;
    }

    textboxes[m].applyposition();
}

void Graphics::setlinegap(int customvalue)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("setlinegap() out-of-bounds!");
        return;
    }

    textboxes[m].linegap = customvalue;
    textboxes[m].resize();
}

int Graphics::getlinegap(void)
{
    // Don't use linegaps in custom levels, for now anyway
    if (map.custommode) return 0;
    return 1;
}

void Graphics::textboxindex(const int index)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxindex() out-of-bounds!");
        return;
    }

    textboxes[m].other_textbox_index = index;
}

void Graphics::createtextboxreal(
    const std::string& t,
    int xp, int yp,
    int r, int g, int b,
    bool flipme
) {
    m = textboxes.size();

    if (m < 20)
    {
        textboxclass text(getlinegap());
        text.lines.push_back(t);
        text.xp = xp;
        if (xp == -1) text.xp = 160 - ((font::len(PR_FONT_LEVEL, t.c_str()) / 2) + 8);
        text.yp = yp;
        text.initcol(r, g, b);
        text.flipme = flipme;
        text.resize();
        textboxes.push_back(text);
    }
}

void Graphics::createtextbox(
    const std::string& t,
    int xp, int yp,
    SDL_Color color
) {
    createtextboxreal(t, xp, yp, color.r, color.g, color.b, false);
}

void Graphics::createtextbox(
    const std::string& t,
    int xp, int yp,
    int r, int g, int b
) {
    createtextboxreal(t, xp, yp, r, g, b, false);
}

void Graphics::createtextboxflipme(
    const std::string& t,
    int xp, int yp,
    SDL_Color color
) {
    createtextboxreal(t, xp, yp, color.r, color.g, color.b, true);
}

void Graphics::createtextboxflipme(
    const std::string& t,
    int xp, int yp,
    int r, int g, int b
) {
    createtextboxreal(t, xp, yp, r, g, b, true);
}

void Graphics::drawfade(void)
{
    const int usethisamount = lerp(oldfadeamount, fadeamount);
    switch (fademode)
    {
    case FADE_FULLY_BLACK:
    case FADE_START_FADEIN:
        fill_rect(0, 0, 0);
        break;
    case FADE_FADING_OUT:
        for (size_t i = 0; i < SDL_arraysize(fadebars); i++)
        {
            fill_rect(fadebars[i], i * 16, usethisamount, 16, 0, 0, 0);
        }
        break;
    case FADE_FADING_IN:
        for (size_t i = 0; i < SDL_arraysize(fadebars); i++)
        {
            fill_rect(fadebars[i] - usethisamount, i * 16, 500, 16, 0, 0, 0);
        }
        break;
    case FADE_NONE:
    case FADE_START_FADEOUT:
        break;
    }
}

void Graphics::processfade(void)
{
    oldfadeamount = fadeamount;
    switch (fademode)
    {
    case FADE_START_FADEOUT:
        for (size_t i = 0; i < SDL_arraysize(fadebars); i++)
        {
            fadebars[i] = -(int)(fRandom() * 12) * 8;
        }
        setfade(0);
        fademode = FADE_FADING_OUT;
        break;
    case FADE_FADING_OUT:
        fadeamount += 24;
        if (fadeamount > 416)
        {
            fademode = FADE_FULLY_BLACK;
        }
        break;
    case FADE_START_FADEIN:
        for (size_t i = 0; i < SDL_arraysize(fadebars); i++)
        {
            fadebars[i] = 320 + (int)(fRandom() * 12) * 8;
        }
        setfade(416);
        fademode = FADE_FADING_IN;
        break;
    case FADE_FADING_IN:
        fadeamount -= 24;
        if (fadeamount <= 0)
        {
            fademode = FADE_NONE;
        }
        break;
    case FADE_NONE:
    case FADE_FULLY_BLACK:
        break;
    }
}

void Graphics::setfade(const int amount)
{
    fadeamount = amount;
    oldfadeamount = amount;
}

void Graphics::drawmenu(int cr, int cg, int cb, enum Menu::MenuName menu)
{
    /* The MenuName is only used for some special cases,
     * like the levels list and the language screen. */

    bool language_screen = menu == Menu::language && !loc::languagelist.empty();
    unsigned int twocol_voptions;
    if (language_screen)
    {
        size_t n_options = game.menuoptions.size();
        twocol_voptions = n_options - (n_options / 2);
    }

    for (size_t i = 0; i < game.menuoptions.size(); i++)
    {
        MenuOption& opt = game.menuoptions[i];

        int fr, fg, fb;
        if (opt.active)
        {
            // Color it normally
            fr = cr;
            fg = cg;
            fb = cb;
        }
        else
        {
            // Color it gray
            fr = 128;
            fg = 128;
            fb = 128;
        }

        int x, y;
        if (language_screen)
        {
            int name_len = font::len(opt.print_flags, opt.text);
            x = (i < twocol_voptions ? 80 : 240) - name_len / 2;
            y = 36 + (i % twocol_voptions) * 12;
        }
        else
        {
            x = i * game.menuspacing + game.menuxoff;
            y = 140 + i * 12 + game.menuyoff;
        }

        if (menu == Menu::levellist)
        {
            size_t separator;
            if (cl.ListOfMetaData.size() > 8)
            {
                separator = 3;
            }
            else
            {
                separator = 1;
            }
            if (game.menuoptions.size() - i <= separator)
            {
                // We're on "next page", "previous page", or "return to menu". Draw them separated by a bit
                y += 8;
            }
            else
            {
                // Get out of the way of the level descriptions
                y += 4;
            }
        }

        if (menu == Menu::translator_options_cutscenetest)
        {
            size_t separator = 4;
            if (game.menuoptions.size() - i <= separator)
            {
                y += 4;
            }
        }

        char buffer[MENU_TEXT_BYTES];
        if ((int) i == game.currentmenuoption && game.slidermode == SLIDER_NONE)
        {
            std::string opt_text;
            if (opt.active)
            {
                // Uppercase the text
                opt_text = loc::toupper(opt.text);
            }
            else
            {
                opt_text = loc::remove_toupper_escape_chars(opt.text);
            }

            vformat_buf(buffer, sizeof(buffer), loc::get_langmeta()->menu_select.c_str(), "label:str", opt_text.c_str());

            // Account for brackets
            x -= (font::len(opt.print_flags, buffer) - font::len(opt.print_flags, opt_text.c_str())) / 2;
        }
        else
        {
            SDL_strlcpy(buffer, loc::remove_toupper_escape_chars(opt.text).c_str(), sizeof(buffer));
        }

        font::print(opt.print_flags, x, y, buffer, fr, fg, fb);
    }
}


void Graphics::drawcoloredtile(
    const int x, const int y,
    const int t,
    const int r, const int g, const int b
) {
    draw_grid_tile(grphx.im_tiles_white, t, x, y, tiles_rect.w, tiles_rect.h, r, g, b);
}


bool Graphics::Hitest(SDL_Surface* surface1, SDL_Point p1, SDL_Surface* surface2, SDL_Point p2)
{

    //find rectangle where they intersect:

    int r1_left = p1.x;
    int r1_right = r1_left + surface1->w;
    int r2_left = p2.x;
    int r2_right = r2_left + surface2->w;

    int r1_bottom = p1.y;
    int r1_top = p1.y + surface1->h;
    int r2_bottom  = p2.y;
    int r2_top = p2.y + surface2->h;

    SDL_Rect rect1 = {p1.x, p1.y, surface1->w, surface1->h};
    SDL_Rect rect2 = {p2.x, p2.y, surface2->w, surface2->h};
    bool intersection = help.intersects(rect1, rect2);

    if(intersection)
    {
        int r3_left = SDL_max(r1_left, r2_left);
        int r3_top = SDL_min(r1_top, r2_top);
        int r3_right = SDL_min(r1_right, r2_right);
        int r3_bottom= SDL_max(r1_bottom, r2_bottom);

        //for every pixel inside rectangle
        for(int x = r3_left; x < r3_right; x++)
        {
            for(int y = r3_bottom; y < r3_top; y++)
            {
                const SDL_Color pixel1 = ReadPixel(surface1, x - p1.x, y - p1.y);
                const SDL_Color pixel2 = ReadPixel(surface2, x - p2.x, y - p2.y);
                /* INTENTIONAL BUG! In previous versions, the game mistakenly
                 * checked the red channel, not the alpha channel.
                 * We preserve it here because some people abuse this. */
                if (pixel1.r != 0 && pixel2.r != 0)
                {
                    return true;
                }
            }
        }
    }
    return false;

}

void Graphics::drawgravityline(const int t, const int x, const int y, const int w, const int h)
{
    if (!INBOUNDS_VEC(t, obj.entities))
    {
        WHINE_ONCE("drawgravityline() out-of-bounds!");
        return;
    }

    if (w <= 0 && h <= 0)
    {
        return;
    }

    set_color(obj.entities[t].realcol);

    draw_line(x, y, x + w, y + h);
}

void Graphics::drawtrophytext(void)
{
    int brightness;

    if (obj.trophytext < 15)
    {
        const int usethismult = lerp(obj.oldtrophytext, obj.trophytext);
        brightness = (usethismult / 15.0) * 255;
    }
    else
    {
        brightness = 255;
    }

    /* These were originally all at the top of the screen, but might be too tight for localization.
     * It probably makes sense to make them all have a top text now, but for now this is probably fine.
     * Look at the Steam achievements, they have pretty logical titles that should probably be used. */
    const char* top_text = NULL;
    const char* bottom_text = NULL;

    switch(obj.trophytype)
    {
    case 1:
        top_text = loc::gettext("SPACE STATION 1 MASTERED");
        bottom_text = loc::gettext("Obtain a V Rank in this Time Trial");
        break;
    case 2:
        top_text = loc::gettext("LABORATORY MASTERED");
        bottom_text = loc::gettext("Obtain a V Rank in this Time Trial");
        break;
    case 3:
        top_text = loc::gettext("THE TOWER MASTERED");
        bottom_text = loc::gettext("Obtain a V Rank in this Time Trial");
        break;
    case 4:
        top_text = loc::gettext("SPACE STATION 2 MASTERED");
        bottom_text = loc::gettext("Obtain a V Rank in this Time Trial");
        break;
    case 5:
        top_text = loc::gettext("WARP ZONE MASTERED");
        bottom_text = loc::gettext("Obtain a V Rank in this Time Trial");
        break;
    case 6:
        top_text = loc::gettext("FINAL LEVEL MASTERED");
        bottom_text = loc::gettext("Obtain a V Rank in this Time Trial");
        break;
    case 7:
        top_text = loc::gettext("GAME COMPLETE");
        bottom_text = loc::gettext_case("Complete the game", 1);
        break;
    case 8:
        top_text = loc::gettext("FLIP MODE COMPLETE");
        bottom_text = loc::gettext("Complete the game in flip mode");
        break;
    case 9:
        bottom_text = loc::gettext("Win with less than 50 deaths");
        break;
    case 10:
        bottom_text = loc::gettext("Win with less than 100 deaths");
        break;
    case 11:
        bottom_text = loc::gettext("Win with less than 250 deaths");
        break;
    case 12:
        bottom_text = loc::gettext("Win with less than 500 deaths");
        break;
    case 13:
        bottom_text = loc::gettext("Last 5 seconds on the Super Gravitron");
        break;
    case 14:
        bottom_text = loc::gettext("Last 10 seconds on the Super Gravitron");
        break;
    case 15:
        bottom_text = loc::gettext("Last 15 seconds on the Super Gravitron");
        break;
    case 16:
        bottom_text = loc::gettext("Last 20 seconds on the Super Gravitron");
        break;
    case 17:
        bottom_text = loc::gettext("Last 30 seconds on the Super Gravitron");
        break;
    case 18:
        bottom_text = loc::gettext("Last 1 minute on the Super Gravitron");
        break;
    case 20:
        top_text = loc::gettext("MASTER OF THE UNIVERSE");
        bottom_text = loc::gettext("Complete the game in no death mode");
        break;
    }

    short lines;
    if (top_text != NULL)
    {
        font::string_wordwrap(0, top_text, 304, &lines);
        font::print_wrap(PR_CEN | PR_BRIGHTNESS(brightness) | PR_BOR, -1, 11 - (lines - 1) * 5, top_text, 196, 196, 255 - help.glow);
    }
    if (bottom_text != NULL)
    {
        font::string_wordwrap(0, bottom_text, 304, &lines);
        font::print_wrap(PR_CEN | PR_BRIGHTNESS(brightness) | PR_BOR, -1, 221 - (lines - 1) * 5, bottom_text, 196, 196, 255 - help.glow);
    }
}

void Graphics::drawentities(void)
{
    const int yoff = map.towermode ? lerp(map.oldypos, map.ypos) : 0;

    if (!map.custommode)
    {
        for (int i = obj.entities.size() - 1; i >= 0; i--)
        {
            if (!obj.entities[i].ishumanoid())
            {
                drawentity(i, yoff);
            }
        }

        for (int i = obj.entities.size() - 1; i >= 0; i--)
        {
            if (obj.entities[i].ishumanoid())
            {
                drawentity(i, yoff);
            }
        }
    }
    else
    {
        for (int i = obj.entities.size() - 1; i >= 0; i--)
        {
            drawentity(i, yoff);
        }
    }
}

void Graphics::drawentity(const int i, const int yoff)
{
    if (!INBOUNDS_VEC(i, obj.entities))
    {
        WHINE_ONCE("drawentity() out-of-bounds!");
        return;
    }

    if (obj.entities[i].invis)
    {
        return;
    }

    SDL_Point tpoint;

    SDL_Rect drawRect;

    bool custom_gray;

    // Special case for gray Warp Zone tileset!
    if (map.custommode)
    {
        const RoomProperty* const room = cl.getroomprop(game.roomx - 100, game.roomy - 100);
        custom_gray = room->tileset == 3 && room->tilecol == 6;
    }
    else
    {
        custom_gray = false;
    }

    SDL_Texture* sprites = flipmode ? grphx.im_flipsprites : grphx.im_sprites;
    SDL_Texture* tiles = (map.custommode && !map.finalmode) ? grphx.im_entcolours : grphx.im_tiles;
    SDL_Texture* tiles_tint = (map.custommode && !map.finalmode) ? grphx.im_entcolours_tint : grphx.im_tiles_tint;

    const int xp = lerp(obj.entities[i].lerpoldxp, obj.entities[i].xp);
    const int yp = lerp(obj.entities[i].lerpoldyp, obj.entities[i].yp);

    switch (obj.entities[i].size)
    {
    case 0:
    {
        // Sprites
        tpoint.x = xp;
        tpoint.y = yp - yoff;
        const SDL_Color ct = obj.entities[i].realcol;

        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;

        draw_grid_tile(sprites, obj.entities[i].drawframe, drawRect.x, drawRect.y, 32, 32, ct);

        if (obj.entities[i].rule == 0)
        {
            // the player! let's draw their fishing rod! if they have one
            ItemStack* stack = getEquippedRod();
            if (stack != NULL)
            {
            }

            if (obj.entities[i].drawframe >= 204 && obj.entities[i].drawframe <= 251)
            {
                int scale_x = 1;
                int scale_y = 1;

                int rod_x = drawRect.x - 5;
                int rod_y = drawRect.y - 9;

                int drawframe = 0;

                switch (obj.entities[i].drawframe)
                {
                case 204: case 216: case 228: case 240:
                    drawframe = 0;
                    break;
                case 205: case 217: case 229: case 241:
                    drawframe = 1;
                    break;
                case 206: case 218: case 230: case 242:
                    drawframe = 2;
                    break;
                case 207: case 219: case 231: case 243:
                    drawframe = 3;
                    break;
                case 208: case 220: case 232: case 244:
                    drawframe = 4;
                    break;
                case 209: case 221: case 233: case 245:
                    drawframe = 5;
                    break;
                case 210: case 222: case 234: case 246:
                    drawframe = 6;
                    break;
                case 211: case 223: case 235: case 247:
                    drawframe = 7;
                    break;
                case 212: case 224: case 236: case 248:
                    drawframe = 8;
                    break;
                }

                if (obj.entities[i].dir == 0)
                {
                    scale_x = -1;
                    rod_x -= 2;
                }
                if (game.gravitycontrol == 1)
                {
                    rod_y += (3 + 8);
                    scale_y = -1;
                }

                draw_grid_tile(grphx.im_fishingrod_anim, drawframe, rod_x, rod_y, 36, 32, getcol(200), scale_x, scale_y);
                draw_grid_tile(grphx.im_fishingrod_line_anim, drawframe, rod_x, rod_y, 36, 32, getcol(201), scale_x, scale_y);
            }

            // draw lines to bobbers (should only exist if you casted your rod so no need for rod checks
            for (int j = 0; j < obj.entities.size(); j++)
            {
                if (obj.entities[j].type == EntityType_BOBBER)
                {
                    // draw a line from the player to the bobber
                    SDL_Point bobberPoint;
                    bobberPoint.x = obj.entities[j].xp + obj.entities[j].off_x + obj.entities[j].cx + (obj.entities[j].w / 2);
                    bobberPoint.y = obj.entities[j].yp + obj.entities[j].off_y + obj.entities[j].cy - 1;
                    if (obj.entities[j].bobbergrav == 1)
                    {
                        bobberPoint.y += 4;
                    }

                    SDL_Point playerPoint = obj.getRodPointPosition();

                    set_color(255, 255, 255);

                    if (obj.entities[j].behave == 1)
                    {
                        draw_line(playerPoint.x, playerPoint.y, bobberPoint.x, bobberPoint.y);
                    }
                    else
                    {
                        // TODO: maybe some actual physics for the midpoint...
                        SDL_Point midPoint;
                        midPoint.x = lerpReal(playerPoint.x, bobberPoint.x, 0.1);
                        midPoint.y = lerpReal(playerPoint.y, bobberPoint.y, 0.9);

                        SDL_Point points[FISHING_LINE_SMOOTHNESS + 1];
                        for (int k = 0; k <= FISHING_LINE_SMOOTHNESS; k++)
                        {
                            float progress = (float)k / FISHING_LINE_SMOOTHNESS;
                            float left_line_x = lerpReal(playerPoint.x, midPoint.x, progress);
                            float left_line_y = lerpReal(playerPoint.y, midPoint.y, progress);
                            float right_line_x = lerpReal(midPoint.x, bobberPoint.x, progress);
                            float right_line_y = lerpReal(midPoint.y, bobberPoint.y, progress);

                            SDL_Point coords;
                            coords.x = lerpReal(left_line_x, right_line_x, progress);
                            coords.y = lerpReal(left_line_y, right_line_y, progress);

                            points[k] = coords;
                        }

                        draw_lines(points, SDL_arraysize(points));
                    }
                }
            }
        }

        int x = drawRect.x + obj.entities[i].cx + (obj.entities[i].w / 2);
        int y = drawRect.y + obj.entities[i].cy + (obj.entities[i].h / 2);
        draw_point_light(x, y, obj.entities[i].light_radius, obj.entities[i].light_strength);

        // screenwrapping!
        SDL_Point wrappedPoint;
        bool wrapX = false;
        bool wrapY = false;

        wrappedPoint.x = tpoint.x;
        if (tpoint.x < 0)
        {
            wrapX = true;
            wrappedPoint.x += 320;
        }
        else if (tpoint.x > 288)
        {
            wrapX = true;
            wrappedPoint.x -= 320;
        }

        wrappedPoint.y = tpoint.y;
        if (tpoint.y < 8)
        {
            wrapY = true;
            wrappedPoint.y += 232;
        }
        else if (tpoint.y > 200)
        {
            wrapY = true;
            wrappedPoint.y -= 232;
        }

        const bool isInWrappingAreaOfTower = map.towermode && !map.minitowermode && map.ypos >= 500 && map.ypos <= 5000;
        if (wrapX && (map.warpx || isInWrappingAreaOfTower))
        {
            drawRect = sprites_rect;
            drawRect.x += wrappedPoint.x;
            drawRect.y += tpoint.y;
            draw_grid_tile(sprites, obj.entities[i].drawframe, drawRect.x, drawRect.y, 32, 32, ct);
            int x = drawRect.x + obj.entities[i].cx + (obj.entities[i].w / 2);
            int y = drawRect.y + obj.entities[i].cy + (obj.entities[i].h / 2);
            draw_point_light(x, y, obj.entities[i].light_radius, obj.entities[i].light_strength);
        }
        if (wrapY && map.warpy)
        {
            drawRect = sprites_rect;
            drawRect.x += tpoint.x;
            drawRect.y += wrappedPoint.y;
            draw_grid_tile(sprites, obj.entities[i].drawframe, drawRect.x, drawRect.y, 32, 32, ct);
            int x = drawRect.x + obj.entities[i].cx + (obj.entities[i].w / 2);
            int y = drawRect.y + obj.entities[i].cy + (obj.entities[i].h / 2);
            draw_point_light(x, y, obj.entities[i].light_radius, obj.entities[i].light_strength);
        }
        if (wrapX && wrapY && map.warpx && map.warpy)
        {
            drawRect = sprites_rect;
            drawRect.x += wrappedPoint.x;
            drawRect.y += wrappedPoint.y;
            draw_grid_tile(sprites, obj.entities[i].drawframe, drawRect.x, drawRect.y, 32, 32, ct);
            int x = drawRect.x + obj.entities[i].cx + (obj.entities[i].w / 2);
            int y = drawRect.y + obj.entities[i].cy + (obj.entities[i].h / 2);
            draw_point_light(x, y, obj.entities[i].light_radius, obj.entities[i].light_strength);
        }
        break;
    }
    case 1:
        // Tiles
        tpoint.x = xp;
        tpoint.y = yp - yoff;
        drawRect = tiles_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;

        draw_grid_tile(grphx.im_tiles, obj.entities[i].drawframe, drawRect.x, drawRect.y, 8, 8);
        break;
    case 2:
    case 8:
    {
        // Special: Moving platform, 4 tiles or 8 tiles

        tpoint.x = xp;
        tpoint.y = yp - yoff;
        int thiswidth = 4;
        if (obj.entities[i].size == 8)
        {
            thiswidth = 8;
        }
        for (int ii = 0; ii < thiswidth; ii++)
        {
            drawRect = tiles_rect;
            drawRect.x += tpoint.x;
            drawRect.y += tpoint.y;
            drawRect.x += 8 * ii;
            if (custom_gray)
            {
                draw_grid_tile(tiles_tint, obj.entities[i].drawframe, drawRect.x, drawRect.y, 8, 8);
            }
            else
            {
                draw_grid_tile(tiles, obj.entities[i].drawframe, drawRect.x, drawRect.y, 8, 8);
            }
        }
        break;
    }
    case 3: // Big chunky pixels!
        fill_rect(xp, yp - yoff, 4, 4, obj.entities[i].realcol);
        break;
    case 4: // Small pickups
    {
        const SDL_Color color = obj.entities[i].realcol;
        drawcoloredtile(xp, yp - yoff, obj.entities[i].tile, color.r, color.g, color.b);
        break;
    }
    case 5: // Horizontal Line
    {
        int oldw = obj.entities[i].w;
        if ((game.swngame == SWN_START_GRAVITRON_STEP_2 || kludgeswnlinewidth)
            && obj.getlineat(84 - 32) == i)
        {
            oldw -= 24;
        }
        drawgravityline(i, xp, yp - yoff, lerp(oldw, obj.entities[i].w) - 1, 0);
        break;
    }
    case 6: // Vertical Line
        drawgravityline(i, xp, yp - yoff, 0, obj.entities[i].h - 1);
        break;
    case 7: // Teleporter
        drawtele(xp, yp - yoff, obj.entities[i].drawframe, obj.entities[i].realcol);
        draw_light(grphx.im_light_teleporter, xp + 48, yp + 48, 1, 1, (obj.entities[i].colour != 100) ? 255 : 127);
        break;
    // case 8:    // Special: Moving platform, 8 tiles
        // Note: This code is in the 4-tile code
        break;
    case 9: // Really Big Sprite! (2x2)
    {
        const SDL_Color ct = obj.entities[i].realcol;

        tpoint.x = xp;
        tpoint.y = yp - yoff;

        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;

        draw_grid_tile(sprites, obj.entities[i].drawframe, drawRect.x, drawRect.y, 32, 32, ct);

        tpoint.x = xp + 32;
        tpoint.y = yp - yoff;

        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;

        draw_grid_tile(sprites, obj.entities[i].drawframe + 1, drawRect.x, drawRect.y, 32, 32, ct);

        tpoint.x = xp;
        tpoint.y = yp + 32 - yoff;

        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;

        draw_grid_tile(sprites, obj.entities[i].drawframe + 12, drawRect.x, drawRect.y, 32, 32, ct);

        tpoint.x = xp + 32;
        tpoint.y = yp + 32 - yoff;

        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;

        draw_grid_tile(sprites, obj.entities[i].drawframe + 13, drawRect.x, drawRect.y, 32, 32, ct);
        break;
    }
    case 10: // 2x1 Sprite
    {
        const SDL_Color ct = obj.entities[i].realcol;

        tpoint.x = xp;
        tpoint.y = yp - yoff;

        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;

        draw_grid_tile(sprites, obj.entities[i].drawframe, drawRect.x, drawRect.y, 32, 32, ct);

        tpoint.x = xp + 32;
        tpoint.y = yp - yoff;

        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;

        draw_grid_tile(sprites, obj.entities[i].drawframe + 1, drawRect.x, drawRect.y, 32, 32, ct);
        break;
    }
    case 11: // The fucking elephant
        drawimagecol(IMAGE_ELEPHANT, xp, yp - yoff, obj.entities[i].realcol);
        break;
    case 12: // Regular sprites that don't wrap
    {
        tpoint.x = xp;
        tpoint.y = yp - yoff;
        const SDL_Color ct = obj.entities[i].realcol;

        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;

        draw_grid_tile(sprites, obj.entities[i].drawframe, drawRect.x, drawRect.y, 32, 32, ct);

        // if we're outside the screen, we need to draw indicators

        if (obj.entities[i].xp < -20 && obj.entities[i].vx > 0)
        {
            if (obj.entities[i].xp < -100)
            {
                tpoint.x = -5 + (int) (-xp / 10);
            }
            else
            {
                tpoint.x = 5;
            }

            tpoint.y = tpoint.y + 4;


            drawRect = tiles_rect;
            drawRect.x += tpoint.x;
            drawRect.y += tpoint.y;

            draw_grid_tile(grphx.im_tiles_white, 1167, drawRect.x, drawRect.y, 8, 8, ct);

        }
        else if (obj.entities[i].xp > 340 && obj.entities[i].vx < 0)
        {
            if (obj.entities[i].xp > 420)
            {
                tpoint.x = 320 - (int) ((xp - 320) / 10);
            }
            else
            {
                tpoint.x = 310;
            }

            tpoint.y = tpoint.y+4;

            drawRect = tiles_rect;
            drawRect.x += tpoint.x;
            drawRect.y += tpoint.y;

            draw_grid_tile(grphx.im_tiles_white, 1166, drawRect.x, drawRect.y, 8, 8, ct);
        }
        break;
    }
    case 13:
    {
        // Special for epilogue: huge hero!
        draw_grid_tile(grphx.im_sprites, obj.entities[i].drawframe, xp, yp - yoff, sprites_rect.w, sprites_rect.h, obj.entities[i].realcol, 6, 6);
        break;
    }
    case 14:
    {
        int frame = 0;
        if (obj.entities[i].state >= 1)
        {
            frame = ((game.framecounter / 5) % 5) + 1;
            draw_point_light(xp + 6, yp + 7, 96, 255);
            draw_point_light(xp + 6, yp + 7, 96, 255);
        }
        else
        {
            draw_point_light(xp + 6, yp + 7, 64, 127);
        }

        draw_grid_tile(grphx.im_dripplelamps, frame, xp, yp - yoff, 12, 20);
        break;
    }
    case 200: // Bobber
    {
        int w, h;

        if (query_texture(getBobberTexture(), NULL, NULL, &w, &h) != 0)
        {
            return;
        }

        bool flipped = (obj.entities[i].bobbergrav == 1);

        const SDL_Rect dstrect = { xp + obj.entities[i].off_x, yp + obj.entities[i].off_y + (flipped ? -12 : 0), w, h};

        copy_texture(getBobberTexture(), NULL, &dstrect, 0, NULL, flipped ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
        break;
    }

    case 201: // Gate
    {
        SDL_Texture* texture = NULL;
        switch (obj.entities[i].behave)
        {
        case 0: texture = grphx.im_gate_yellow; break;
        case 1: texture = grphx.im_gate_blue; break;
        case 2: texture = grphx.im_gate_purple; break;
        case 3: texture = grphx.im_gate_red; break;
        }

        int x = xp;
        int y = yp - yoff;
        int w = obj.entities[i].w;
        int h = obj.entities[i].h;

        if (texture == NULL)
        {
            draw_rect(x, y, w, h, getRGB(255, 255, 255));
            return;
        }

        for (int cx = x + 4; cx < x + w - 4; cx += 4)
        {
            // top edge
            draw_grid_tile(texture, 3, cx, y, 4, 4);
            // bottom edge
            draw_grid_tile(texture, 13, cx, y + (h - 4), 4, 4);
        }
        for (int cy = y + 4; cy < y + h - 4; cy += 4)
        {
            // left edge
            draw_grid_tile(texture, 7, x, cy, 4, 4);
            // right edge
            draw_grid_tile(texture, 9, x + (w - 4), cy, 4, 4);
        }

        for (int cy = y + 4; cy < y + h - 4; cy += 4)
        {
            for (int cx = x + 4; cx < x + w - 4; cx += 4)
            {
                draw_grid_tile(texture, 8, cx, cy, 4, 4);
            }
        }

        // top left corner
        draw_grid_tile(texture, 2, x, y, 4, 4);
        // top right corner
        draw_grid_tile(texture, 4, x + (w - 4), y, 4, 4);
        // bottom left corner
        draw_grid_tile(texture, 12, x, y + (h - 4), 4, 4);
        // bottom right corner
        draw_grid_tile(texture, 14, x + (w - 4), y + (h - 4), 4, 4);

        // keyhole
        draw_grid_tile(texture, 0, x + (w / 2) - 4, y + (h / 2) - 4, 4, 4);
        draw_grid_tile(texture, 1, x + (w / 2), y + (h / 2) - 4, 4, 4);
        draw_grid_tile(texture, 5, x + (w / 2) - 4, y + (h / 2), 4, 4);
        draw_grid_tile(texture, 6, x + (w / 2), y + (h / 2), 4, 4);

        break;
    }
    case 205: // Special terminal
        draw_grid_tile(grphx.im_special_terminals, obj.entities[i].drawframe, xp, yp - yoff, 32, 40, obj.entities[i].realcol);
        break;
    case 206: // Item display
        if (obj.entities[i].item != NULL)
        {
            obj.entities[i].item->draw(xp, yp - yoff);
            draw_point_light(xp + 8, yp + 8, 64, 255);
        }
        break;
    }
}

void Graphics::drawbackground( int t )
{
    switch(t)
    {
    case 1:
        // Starfield
        fill_rect(0, 0, 0);
        for (int i = 0; i < numstars; i++)
        {
            SDL_Rect star_rect = stars[i];
            star_rect.x = lerp(star_rect.x + starsspeed[i], star_rect.x);
            if (starsspeed[i] <= 6)
            {
                fill_rect(&star_rect, getRGB(0x22,0x22,0x22));
            }
            else
            {
                fill_rect(&star_rect, getRGB(0x55,0x55,0x55));
            }
        }
        break;
    case 2:
    {
        SDL_Color bcol;
        SDL_Color bcol2;
        SDL_zero(bcol);
        SDL_zero(bcol2);

        // Lab
        switch (rcol)
        {
            // Akward ordering to match tileset
        case 0:
            bcol2 = getRGB(0, 16 * backboxmult, 16 * backboxmult);
            break; // Cyan
        case 1:
            bcol2 = getRGB(16 * backboxmult, 0, 0);
            break; // Red
        case 2:
            bcol2 = getRGB(16 * backboxmult, 0, 16 * backboxmult);
            break; // Purple
        case 3:
            bcol2 = getRGB(0, 0, 16 * backboxmult);
            break; // Blue
        case 4:
            bcol2 = getRGB(16 * backboxmult, 16 * backboxmult, 0);
            break; // Yellow
        case 5:
            bcol2 = getRGB(0, 16 * backboxmult, 0);
            break; // Green
        case 6:
            // crazy case
            switch (spcol)
            {
            case 0:
                bcol2 = getRGB(0, 16 * backboxmult, 16 * backboxmult);
                break; // Cyan
            case 1:
                bcol2 = getRGB(0, (spcoldel + 1) * backboxmult, 16 * backboxmult);
                break; // Cyan
            case 2:
                bcol2 = getRGB(0, 0, 16 * backboxmult);
                break; // Blue
            case 3:
                bcol2 = getRGB((16 - spcoldel) * backboxmult, 0, 16 * backboxmult);
                break; // Blue
            case 4:
                bcol2 = getRGB(16 * backboxmult, 0, 16 * backboxmult);
                break; // Purple
            case 5:
                bcol2 = getRGB(16 * backboxmult, 0, (spcoldel + 1) * backboxmult);
                break; // Purple
            case 6:
                bcol2 = getRGB(16 * backboxmult, 0, 0);
                break; // Red
            case 7:
                bcol2 = getRGB(16 * backboxmult, (16 - spcoldel) * backboxmult, 0);
                break; // Red
            case 8:
                bcol2 = getRGB(16 * backboxmult, 16 * backboxmult, 0);
                break; // Yellow
            case 9:
                bcol2 = getRGB((spcoldel + 1) * backboxmult, 16 * backboxmult, 0);
                break; // Yellow
            case 10:
                bcol2 = getRGB(0, 16 * backboxmult, 0);
                break; // Green
            case 11:
                bcol2 = getRGB(0, 16 * backboxmult, (16 - spcoldel) * backboxmult);
                break; // Green
            }
            break;
        }
        fill_rect(bcol2);

        for (int i = 0; i < numbackboxes; i++)
        {
            switch (rcol)
            {
                // Akward ordering to match tileset
            case 0:
                bcol = getRGB(16, 128 * backboxmult, 128 * backboxmult);
                break; // Cyan
            case 1:
                bcol = getRGB(128 * backboxmult, 16, 16);
                break; // Red
            case 2:
                bcol = getRGB(128 * backboxmult, 16, 128 * backboxmult);
                break; // Purple
            case 3:
                bcol = getRGB(16, 16, 128 * backboxmult);
                break; // Blue
            case 4:
                bcol = getRGB(128 * backboxmult, 128 * backboxmult, 16);
                break; // Yellow
            case 5:
                bcol = getRGB(16, 128 * backboxmult, 16);
                break; // Green
            case 6:
                // crazy case
                switch (spcol)
                {
                case 0:
                    bcol = getRGB(16, 128 * backboxmult, 128 * backboxmult);
                    break; // Cyan
                case 1:
                    bcol = getRGB(16, ((spcoldel + 1) * 8) * backboxmult, 128 * backboxmult);
                    break; // Cyan
                case 2:
                    bcol = getRGB(16, 16, 128 * backboxmult);
                    break; // Blue
                case 3:
                    bcol = getRGB((128 - (spcoldel * 8)) * backboxmult, 16, 128 * backboxmult);
                    break; // Blue
                case 4:
                    bcol = getRGB(128 * backboxmult, 16, 128 * backboxmult);
                    break; // Purple
                case 5:
                    bcol = getRGB(128 * backboxmult, 16, ((spcoldel + 1) * 8) * backboxmult);
                    break; // Purple
                case 6:
                    bcol = getRGB(128 * backboxmult, 16, 16);
                    break; // Red
                case 7:
                    bcol = getRGB(128 * backboxmult, (128 - (spcoldel * 8)) * backboxmult, 16);
                    break; // Red
                case 8:
                    bcol = getRGB(128 * backboxmult, 128 * backboxmult, 16);
                    break; // Yellow
                case 9:
                    bcol = getRGB(((spcoldel + 1) * 8) * backboxmult, 128 * backboxmult, 16);
                    break; // Yellow
                case 10:
                    bcol = getRGB(16, 128 * backboxmult, 16);
                    break; // Green
                case 11:
                    bcol = getRGB(16, 128 * backboxmult, (128 - (spcoldel * 8)) * backboxmult);
                    break; // Green
                }
                break;
            }

            SDL_Rect backboxrect = backboxes[i];
            backboxrect.x = lerp(backboxes[i].x - backboxvx[i], backboxes[i].x);
            backboxrect.y = lerp(backboxes[i].y - backboxvy[i], backboxes[i].y);

            fill_rect(&backboxrect, bcol);
            backboxrect.x++;
            backboxrect.y++;
            backboxrect.w -= 2;
            backboxrect.h -= 2;
            fill_rect(&backboxrect, bcol2);
        }
        break;
    }
    case 3: //Warp zone (horizontal)
    {
        clear();

        const int offset = (int) lerp(-3, 0);
        const SDL_Rect srcRect = {8 + offset, 0, SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS};

        copy_texture(backgroundTexture, &srcRect, NULL);
        break;
    }
    case 4: //Warp zone (vertical)
    {
        clear();

        const int offset = (int) lerp(-3, 0);
        const SDL_Rect srcRect = {0, 8 + offset, SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS};

        copy_texture(backgroundTexture, &srcRect, NULL);
        break;
    }
    case 5:
    {
        // Warp zone, central
        SDL_Color warpbcol;
        SDL_Color warpfcol;

        switch(rcol)
        {
            // Akward ordering to match tileset
        case 0:
            warpbcol = getRGB(0x0A, 0x10, 0x0E);
            warpfcol = getRGB(0x10, 0x22, 0x21);
            break; // Cyan
        case 1:
            warpbcol = getRGB(0x11, 0x09, 0x0B);
            warpfcol = getRGB(0x22, 0x10, 0x11);
            break; // Red
        case 2:
            warpbcol = getRGB(0x0F, 0x0A, 0x10);
            warpfcol = getRGB(0x22,0x10,0x22);
            break; // Purple
        case 3:
            warpbcol = getRGB(0x0A, 0x0B, 0x10);
            warpfcol = getRGB(0x10, 0x10, 0x22);
            break; // Blue
        case 4:
            warpbcol = getRGB(0x10, 0x0D, 0x0A);
            warpfcol = getRGB(0x22, 0x1E, 0x10);
            break; // Yellow
        case 5:
            warpbcol = getRGB(0x0D, 0x10, 0x0A);
            warpfcol = getRGB(0x14, 0x22, 0x10);
            break; // Green
        case 6:
            warpbcol = getRGB(0x0A, 0x0A, 0x0A);
            warpfcol = getRGB(0x12, 0x12, 0x12);
            break; // Gray
        default:
            warpbcol = getRGB(0xFF, 0xFF, 0xFF);
            warpfcol = getRGB(0xFF, 0xFF, 0xFF);
        }

        for (int i = 10; i >= 0; i--)
        {
            const int temp = (i * 16) + backoffset;
            const SDL_Rect warprect = {160 - temp, 120 - temp, temp * 2, temp * 2};
            if (i % 2 == warpskip)
            {
                fill_rect(&warprect, warpbcol);
            }
            else
            {
                fill_rect(&warprect, warpfcol);
            }
        }
        break;
    }
    case 6:
        // Final Starfield
        fill_rect(0, 0, 0);
        for (int i = 0; i < numstars; i++)
        {
            SDL_Rect star_rect = stars[i];
            star_rect.y = lerp(star_rect.y + starsspeed[i], star_rect.y);
            if (starsspeed[i] <= 8)
            {
                fill_rect(&star_rect, getRGB(0x22, 0x22, 0x22));
            }
            else
            {
                fill_rect(&star_rect, getRGB(0x55, 0x55, 0x55));
            }
        }
        break;
    case 7:
        // Static, unscrolling section of the tower
        for (int j = 0; j < 30; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                drawtile3(i * 8, j * 8, map.tower.backat(i, j, 200), 15);
            }
        }
        break;
    case 8:
        // Static, unscrolling section of the tower
        for (int j = 0; j < 30; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                drawtile3(i * 8, j * 8, map.tower.backat(i, j, 200), 10);
            }
        }
        break;
    case 9:
        // Static, unscrolling section of the tower
        for (int j = 0; j < 30; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                drawtile3(i * 8, j * 8, map.tower.backat(i, j, 600), 0);
            }
        }
        break;
    default:
        fill_rect(0, 0, 0);
        break;
    }
}

void Graphics::updatebackground(int t)
{
    switch (t)
    {
    case 1:
        // Starfield
        for (int i = 0; i < numstars; i++)
        {
            stars[i].x -= starsspeed[i];
            if (stars[i].x < -10)
            {
                stars[i].x += 340;
                stars[i].y = (int) (fRandom() * 240);
                starsspeed[i] = 4 + (int) (fRandom() * 4);
            }
        }
        break;
    case 2:
        // Lab
        if (rcol == 6)
        {
            // crazy caze
            spcoldel--;
            if (spcoldel <= 0)
            {
                spcoldel = 15;
                spcol++;
                if (spcol >= 12) spcol = 0;
            }
        }
        for (int i = 0; i < numbackboxes; i++)
        {
            backboxes[i].x += backboxvx[i];
            backboxes[i].y += backboxvy[i];
            if (backboxes[i].x < -40)
            {
                backboxes[i].x = 320;
                backboxes[i].y = fRandom() * 240;
            }
            if (backboxes[i].x > 320)
            {
                backboxes[i].x = -32;
                backboxes[i].y = fRandom() * 240;
            }
            if (backboxes[i].y < -40)
            {
                backboxes[i].y = 240;
                backboxes[i].x = fRandom() * 320;
            }
            if (backboxes[i].y > 260)
            {
                backboxes[i].y = -32;
                backboxes[i].x = fRandom() * 320;
            }
        }
        break;
    case 3: // Warp zone (horizontal)
    {
        const int temp = 680 + (rcol * 3);
        backoffset += 3;
        if (backoffset >= 16) backoffset -= 16;

        SDL_Texture* target = SDL_GetRenderTarget(gameScreen.m_renderer);
        set_render_target(backgroundTexture);

        if (backgrounddrawn)
        {
            scroll_texture(backgroundTexture, tempScrollingTexture, -3, 0);
            for (int j = 0; j < 15; j++)
            {
                for (int i = 0; i < 2; i++)
                {
                    drawtile2(317 - backoffset + (i * 16), (j * 16), temp + 40);  // 20*16 = 320
                    drawtile2(317 - backoffset + (i * 16) + 8, (j * 16), temp + 41);
                    drawtile2(317 - backoffset + (i * 16), (j * 16) + 8, temp + 80);
                    drawtile2(317 - backoffset + (i * 16) + 8, (j * 16) + 8, temp + 81);
                }
            }
        }
        else
        {
            // draw the whole thing for the first time!
            backoffset = 0;
            clear();
            for (int j = 0; j < 15; j++)
            {
                for (int i = 0; i < 21; i++)
                {
                    drawtile2((i * 16) - backoffset - 3, (j * 16), temp + 40);
                    drawtile2((i * 16) - backoffset + 8 - 3, (j * 16), temp + 41);
                    drawtile2((i * 16) - backoffset - 3, (j * 16) + 8, temp + 80);
                    drawtile2((i * 16) - backoffset + 8 - 3, (j * 16) + 8, temp + 81);
                }
            }
            backgrounddrawn = true;
        }
        set_render_target(target);
        break;
    }
    case 4: // Warp zone (vertical)
    {
        const int temp = 760 + (rcol * 3);
        backoffset += 3;
        if (backoffset >= 16) backoffset -= 16;

        SDL_Texture* target = SDL_GetRenderTarget(gameScreen.m_renderer);
        set_render_target(backgroundTexture);

        if (backgrounddrawn)
        {
            scroll_texture(backgroundTexture, tempScrollingTexture, 0, -3);
            for (int j = 0; j < 2; j++)
            {
                for (int i = 0; i < 21; i++)
                {
                    drawtile2((i * 16), 237 - backoffset + (j * 16), temp + 40); // 14*17=240 - 3
                    drawtile2((i * 16) + 8, 237 - backoffset + (j * 16), temp + 41);
                    drawtile2((i * 16), 237 - backoffset + (j * 16) + 8, temp + 80);
                    drawtile2((i * 16) + 8, 237 - backoffset + (j * 16) + 8, temp + 81);
                }
            }
        }
        else
        {
            // draw the whole thing for the first time!
            backoffset = 0;
            clear();
            for (int j = 0; j < 16; j++)
            {
                for (int i = 0; i < 21; i++)
                {
                    drawtile2((i * 16), (j * 16) - backoffset - 3, temp + 40);
                    drawtile2((i * 16) + 8, (j * 16) - backoffset - 3, temp + 41);
                    drawtile2((i * 16), (j * 16) - backoffset + 8 - 3, temp + 80);
                    drawtile2((i * 16) + 8, (j * 16) - backoffset + 8 - 3, temp + 81);
                }
            }
            backgrounddrawn = true;
        }
        set_render_target(target);
        break;
    }
    case 5:
        // Warp zone, central

        backoffset++;
        if (backoffset >= 16)
        {
            backoffset -= 16;
            warpskip = (warpskip + 1) % 2;
        }
        break;
    case 6:
        // Final Starfield
        for (int i = 0; i < numstars; i++)
        {
            stars[i].y -= starsspeed[i];
            if (stars[i].y < -10)
            {
                stars[i].y += 260;
                stars[i].x = fRandom() * 320;
                starsspeed[i] = 5 + (fRandom() * 5);
            }
        }
        break;
    }
}

static float point_lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

// Catmull-Rom spline interpolation
static SDL_FPoint catmull_rom(const SDL_FPoint& p0, const SDL_FPoint& p1, const SDL_FPoint& p2, const SDL_FPoint& p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;

    float x = 0.5f * (
        (2 * p1.x) +
        (-p0.x + p2.x) * t +
        (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t2 +
        (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t3
        );

    float y = 0.5f * (
        (2 * p1.y) +
        (-p0.y + p2.y) * t +
        (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t2 +
        (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t3
        );

    return { x, y };
}

// Generate a smooth spline from a list of SDL_FPoints
static std::vector<SDL_FPoint> generate_spline(const std::vector<SDL_FPoint>& points, int subdivisions) {
    std::vector<SDL_FPoint> smoothPoints;

    for (size_t i = 0; i < points.size() - 1; ++i) {
        // Control points
        SDL_FPoint p0 = (i > 0) ? points[i - 1] : points[i];
        SDL_FPoint p1 = points[i];
        SDL_FPoint p2 = points[i + 1];
        SDL_FPoint p3 = (i < points.size() - 2) ? points[i + 2] : points[i + 1];

        // Add the start point of the segment
        smoothPoints.push_back(p1);

        // Generate intermediate points using Catmull-Rom interpolation
        for (int j = 1; j <= subdivisions; ++j) {
            float t = static_cast<float>(j) / (subdivisions + 1);
            SDL_FPoint interpolated = catmull_rom(p0, p1, p2, p3, t);
            smoothPoints.push_back(interpolated);
        }
    }

    // Add the final point
    smoothPoints.push_back(points.back());

    return smoothPoints;
}

void Graphics::drawwater(void)
{
    SDL_Texture* target = SDL_GetRenderTarget(gameScreen.m_renderer);

    set_render_target(waterLineTexture);
    clear(0, 0, 0, 0);
    set_render_target(waterLineUnderneathTexture);
    clear(0, 0, 0, 0);
    set_render_target(waterTexture);
    clear(0, 0, 0, 0);

    for (size_t j = 0; j < obj.blocks.size(); j++)
    {
        blockclass block = obj.blocks[j];

        if (block.type == WATER)
        {
            SDL_Color water_col = getWaterColorsForPool(block.script);
            std::vector<SDL_FPoint> current_points;
            std::vector<SDL_FPoint> current_bottom_points;

            for (int i = 0; i < block.springs.size(); i++)
            {
                SDL_FPoint point;
                point.x = block.xp + block.springs[i].x;
                point.y = block.yp + block.springs[i].y;

                current_points.push_back(point);
            }
            for (int i = 0; i < block.bottom_springs.size(); i++)
            {
                SDL_FPoint point;
                point.x = block.xp + block.bottom_springs[i].x;
                point.y = block.yp + block.bottom_springs[i].y;

                current_bottom_points.push_back(point);
            }

            std::vector<SDL_FPoint> genned_points = generate_spline(current_points, 4);
            std::vector<SDL_FPoint> genned_bottom_points = generate_spline(current_bottom_points, 4);

            std::vector<SDL_Point> points;
            std::vector<SDL_Point> bottom_points;
            // Loop through the points, shift them all down a pixel, and copy them to our render vector
            for (int i = 0; i < genned_points.size(); i++)
            {
                SDL_Point point;
                point.x = genned_points[i].x;
                point.y = genned_points[i].y + 1;
                points.push_back(point);
            }
            for (int i = 0; i < genned_bottom_points.size(); i++)
            {
                SDL_Point point;
                point.x = genned_bottom_points[i].x;
                point.y = genned_bottom_points[i].y - 1;
                bottom_points.push_back(point);
            }

            // From left to right, draw vertical lines to fill in the water area

            for (int x = 0; x < block.wp; x++) {
                int top_y = 0;
                int bottom_y = 0;

                // Figure out the top_y by finding the line (two points) above this x position -- and then we should interpolate between the two to find the position on the line.

                // Left point:
                int left_point_top = 0;
                for (int i = 0; i < block.springs.size(); i++) {
                    if (block.springs[i].x <= x) {
                        left_point_top = i;
                    }
                }
                int left_point_bottom = 0;
                for (int i = 0; i < block.bottom_springs.size(); i++) {
                    if (block.bottom_springs[i].x <= x) {
                        left_point_bottom = i;
                    }
                }

                // Right point:
                int right_point_top = 0;
                for (int i = 0; i < block.springs.size(); i++) {
                    if (block.springs[i].x > x) {
                        right_point_top = i;
                        break;
                    }
                }
                int right_point_bottom = 0;
                for (int i = 0; i < block.bottom_springs.size(); i++) {
                    if (block.bottom_springs[i].x > x) {
                        right_point_bottom = i;
                        break;
                    }
                }

                // Interpolate between the two points
                float val_top = (x - block.springs[left_point_top].x) / (block.springs[right_point_top].x - block.springs[left_point_top].x);
                top_y = point_lerp(block.springs[left_point_top].y, block.springs[right_point_top].y, val_top);
                float val_bottom = (x - block.bottom_springs[left_point_bottom].x) / (block.bottom_springs[right_point_bottom].x - block.bottom_springs[left_point_bottom].x);
                bottom_y = point_lerp(block.bottom_springs[left_point_bottom].y, block.bottom_springs[right_point_bottom].y, val_bottom);

                set_render_target(waterTexture);

                // Draw the line
                set_color(water_col);
                SDL_RenderDrawLineF(gameScreen.m_renderer, block.xp + x, block.yp + top_y, block.xp + x, block.yp + bottom_y);
            }

            set_render_target(waterLineUnderneathTexture);

            // Draw the springs as connected lines.
            set_color(SDL_clamp(water_col.r + 64, 0, 255), SDL_clamp(water_col.g + 64, 0, 255), SDL_clamp(water_col.b + 64, 0, 255));
            SDL_RenderDrawLines(gameScreen.m_renderer, points.data(), points.size());
            SDL_RenderDrawLines(gameScreen.m_renderer, bottom_points.data(), bottom_points.size());
            set_color(255, 255, 255);

            // Loop through the points and shift them all back up a pixel!

            for (int i = 0; i < points.size(); i++)
            {
                points[i].y--;
            }
            for (int i = 0; i < bottom_points.size(); i++)
            {
                bottom_points[i].y++;
            }

            set_render_target(waterLineTexture);

            // Draw the springs as connected lines
            set_color(255, 255, 255);
            SDL_RenderDrawLines(gameScreen.m_renderer, points.data(), points.size());
            SDL_RenderDrawLines(gameScreen.m_renderer, bottom_points.data(), bottom_points.size());
        }
    }

    set_render_target(target);

    //static SDL_BlendMode mode = SDL_ComposeCustomBlendMode(
    //);

    // WATER

    // FIRST PASS: Just the normal blend is fine thanks
    set_blendmode(waterTexture, SDL_BLENDMODE_BLEND);
    set_texture_alpha_mod(waterTexture, 96);
    copy_texture(waterTexture, NULL, NULL);
    set_texture_alpha_mod(waterTexture, 255);

    // SECOND PASS: Make that shit multiply !!
    set_blendmode(waterTexture, SDL_BLENDMODE_MUL);
    set_texture_alpha_mod(waterTexture, 192);
    copy_texture(waterTexture, NULL, NULL);
    set_texture_alpha_mod(waterTexture, 255);

    // UNDER WATER LINE
    // ADDITIVE (128 alpha)
    set_blendmode(waterLineUnderneathTexture, SDL_BLENDMODE_ADD);
    set_texture_alpha_mod(waterLineUnderneathTexture, 128);
    copy_texture(waterLineUnderneathTexture, NULL, NULL);
    set_texture_alpha_mod(waterLineUnderneathTexture, 255);

    // WATER LINE
    // ADDITIVE
    set_blendmode(waterLineTexture, SDL_BLENDMODE_ADD);
    set_texture_alpha_mod(waterLineTexture, 255);
    copy_texture(waterLineTexture, NULL, NULL);
    set_texture_alpha_mod(waterLineTexture, 255);
}

void Graphics::drawmap(bool bg)
{
    if (!foregrounddrawn)
    {
        SDL_Texture* target = SDL_GetRenderTarget(gameScreen.m_renderer);

        set_render_target(foregroundTexture);
        set_blendmode(foregroundTexture, SDL_BLENDMODE_BLEND);
        clear(0, 0, 0, 0);
        set_render_target(backgroundTileTexture);
        set_blendmode(backgroundTileTexture, SDL_BLENDMODE_BLEND);
        clear(0, 0, 0, 0);

        for (int y = 0; y < 30; y++)
        {
            for (int x = 0; x < 40; x++)
            {
                int tile;
                int tileset;
                if (game.gamestate == EDITORMODE)
                {
                    tile = cl.gettile(ed.levx, ed.levy, x, y);
                    tileset = (cl.getroomprop(ed.levx, ed.levy)->tileset == 0) ? 0 : 1;
                }
                else
                {
                    tile = map.contents[TILE_IDX(x, y)];
                    tileset = map.tileset;
                }

                if (tile > 0)
                {
                    if (isbg(tile)) {
                        set_render_target(backgroundTileTexture);
                    }
                    else
                    {
                        set_render_target(foregroundTexture);
                    }
                    if (tileset == 0)
                    {
                        drawtile(x * 8, y * 8, tile);
                    }
                    else if (tileset == 1)
                    {
                        drawtile2(x * 8, y * 8, tile);
                    }
                    else if (tileset == 2)
                    {
                        drawtile3(x * 8, y * 8, tile, map.rcol);
                    }
                }
            }
        }

        set_render_target(target);
        foregrounddrawn = true;
    }

    copy_texture(bg ? backgroundTileTexture : foregroundTexture, NULL, NULL);
}

void Graphics::drawfinalmap(void)
{
    if (!foregrounddrawn)
    {
        SDL_Texture* target = SDL_GetRenderTarget(gameScreen.m_renderer);

        set_render_target(foregroundTexture);
        set_blendmode(foregroundTexture, SDL_BLENDMODE_BLEND);
        clear(0, 0, 0, 0);
        if (map.tileset == 0) {
            for (int j = 0; j < 30; j++) {
                for (int i = 0; i < 40; i++) {
                    if ((map.contents[TILE_IDX(i, j)]) > 0)
                        drawtile(i * 8, j * 8, map.finalat(i, j));
                }
            }
        }
        else if (map.tileset == 1) {
            for (int j = 0; j < 30; j++) {
                for (int i = 0; i < 40; i++) {
                    if ((map.contents[TILE_IDX(i, j)]) > 0)
                        drawtile2(i * 8, j * 8, map.finalat(i, j));
                }
            }
        }
        set_render_target(target);
        foregrounddrawn = true;
    }

    copy_texture(foregroundTexture, NULL, NULL);
}

void Graphics::drawtowermap(void)
{
    const int yoff = lerp(map.oldypos, map.ypos);
    for (int j = 0; j < 31; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            const int temp = map.tower.at(i, j, yoff);
            if (temp > 0)
            {
                drawtile3(i * 8, (j * 8) - (yoff % 8), temp, towerbg.colstate);
            }
        }
    }
}

void Graphics::drawtowerspikes(void)
{
    int spikeleveltop = lerp(map.oldspikeleveltop, map.spikeleveltop);
    int spikelevelbottom = lerp(map.oldspikelevelbottom, map.spikelevelbottom);
    for (int i = 0; i < 40; i++)
    {
        drawtile3(i * 8, -8+spikeleveltop, 9, towerbg.colstate);
        drawtile3(i * 8, 230-spikelevelbottom, 8, towerbg.colstate, 8 - spikelevelbottom);
    }
}

void Graphics::drawtowerbackground(const TowerBG& bg_obj)
{
    clear();

    const int offset = (int) lerp(-bg_obj.bscroll, 0);
    const SDL_Rect srcRect = {0, 8 + offset, SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS};

    copy_texture(bg_obj.texture, &srcRect, NULL);
}

void Graphics::updatetowerbackground(TowerBG& bg_obj)
{
    if (bg_obj.bypos < 0) bg_obj.bypos += 120 * 8;

    SDL_Texture* target = SDL_GetRenderTarget(gameScreen.m_renderer);
    set_render_target(bg_obj.texture);

    if (bg_obj.tdrawback)
    {
        int off = bg_obj.scrolldir == 0 ? 0 : bg_obj.bscroll;
        //Draw the whole thing; needed for every colour cycle!
        clear();
        for (int j = -1; j < 32; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                const int temp = map.tower.backat(i, j, bg_obj.bypos);
                drawtile3(i * 8, (j * 8) - (bg_obj.bypos % 8) - off, temp, bg_obj.colstate);
            }
        }

        bg_obj.tdrawback = false;
    }
    else
    {
        // just update the bottom
        scroll_texture(bg_obj.texture, tempScrollingTexture, 0, -bg_obj.bscroll);
        if (bg_obj.scrolldir == 0)
        {
            for (int i = 0; i < 40; i++)
            {
                int temp = map.tower.backat(i, -1, bg_obj.bypos);
                drawtile3(i * 8, -1 * 8 - (bg_obj.bypos % 8), temp, bg_obj.colstate);
                temp = map.tower.backat(i, 0, bg_obj.bypos);
                drawtile3(i * 8, -(bg_obj.bypos % 8), temp, bg_obj.colstate);
            }
        }
        else
        {
            for (int i = 0; i < 40; i++)
            {
                int temp = map.tower.backat(i, 29, bg_obj.bypos);
                drawtile3(i * 8, 29 * 8 - (bg_obj.bypos % 8) - bg_obj.bscroll, temp, bg_obj.colstate);
                temp = map.tower.backat(i, 30, bg_obj.bypos);
                drawtile3(i * 8, 30 * 8 - (bg_obj.bypos % 8) - bg_obj.bscroll, temp, bg_obj.colstate);
                temp = map.tower.backat(i, 31, bg_obj.bypos);
                drawtile3(i * 8, 31 * 8 - (bg_obj.bypos % 8) - bg_obj.bscroll, temp, bg_obj.colstate);
                temp = map.tower.backat(i, 32, bg_obj.bypos);
                drawtile3(i * 8, 32 * 8 - (bg_obj.bypos % 8) - bg_obj.bscroll, temp, bg_obj.colstate);
            }
        }
    }
    set_render_target(target);
}

static SDL_Color makeDarker(SDL_Color color)
{
    return { (Uint8)(color.r * 0.5), (Uint8)(color.g * 0.5), (Uint8)(color.b * 0.5), color.a };
}

#define GETCOL_RANDOM (game.noflashingmode ? 0.5 : fRandom())
SDL_Color Graphics::getcol( int t )
{

    SDL_Color gold_colors[] = {
        getRGB(255, 255, 150),
        getRGB(224, 198, 85),
        getRGB(198, 173, 61),
        getRGB(175, 143, 29),
        getRGB(163, 129, 52),
        getRGB(158, 112, 37)
    };

    // Setup predefinied colours as per our zany palette
    switch(t)
    {
        // Player Normal
    case 0:
        return getRGB(160 - help.glow/2 - (int) (GETCOL_RANDOM * 20), 200 - help.glow/2, 220 - help.glow);
        // Player Hurt
    case 1:
        return getRGB(196 - (GETCOL_RANDOM * 64), 10, 10);
        // Enemies and stuff
    case 2:
        return getRGB(225 - (help.glow / 2), 75, 30);
    case 3: // Trinket
        if (!trinketcolset)
        {
            trinketr = 200 - (GETCOL_RANDOM * 64);
            trinketg = 200 - (GETCOL_RANDOM * 128);
            trinketb = 164 + (GETCOL_RANDOM * 60);
            trinketcolset = true;
        }
        return getRGB(trinketr, trinketg, trinketb);
    case 4: // Inactive savepoint
    {
        const int temp = (help.glow / 2) + (int) (GETCOL_RANDOM * 8);
        return getRGB(80 + temp, 80 + temp, 80 + temp);
    }
    case 5: // Active savepoint
        return getRGB(164 + (GETCOL_RANDOM * 64), 164 + (GETCOL_RANDOM * 64), 255 - (GETCOL_RANDOM * 64));
    case 6: // Enemy : Red
        return getRGB(250 - help.glow / 2, 60 - help.glow / 2, 60 - help.glow / 2);
    case 7: // Enemy : Green
        return getRGB(100 - help.glow / 2 - (int) (GETCOL_RANDOM * 30), 250 - help.glow / 2, 100 - help.glow / 2 - (int) (GETCOL_RANDOM * 30));
    case 8: // Enemy : Purple
        return getRGB(250 - help.glow / 2, 20, 128 - help.glow / 2 + (int) (GETCOL_RANDOM * 30));
    case 9: // Enemy : Yellow
        return getRGB(250 - help.glow / 2, 250 - help.glow / 2, 20);
    case 10: // Warp point (white)
        return getRGB(255 - (GETCOL_RANDOM * 64), 255 - (GETCOL_RANDOM * 64), 255 - (GETCOL_RANDOM * 64));
    case 11: // Enemy : Cyan
        return getRGB(20, 250 - help.glow / 2, 250 - help.glow / 2);
    case 12: // Enemy : Blue
        return getRGB(90 - help.glow / 2, 90 - help.glow / 2, 250 - help.glow / 2);
        // Crew Members
        // green
    case 13:
        return getRGB(120 - help.glow / 4 - (int) (GETCOL_RANDOM * 20), 220 - help.glow / 4, 120 - help.glow / 4);
        // Yellow
    case 14:
        return getRGB(220 - help.glow / 4 - (int) (GETCOL_RANDOM * 20), 210 - help.glow / 4, 120 - help.glow / 4);
        // pink
    case 15:
        return getRGB(255 - help.glow / 8, 70 - help.glow / 4, 70 - help.glow / 4);
        // Blue
    case 16:
        return getRGB(75, 75, 255 - help.glow / 4 - (int) (GETCOL_RANDOM * 20));

    case 17: // Enemy : Orange
        return getRGB(250 - help.glow / 2, 130 - help.glow / 2, 20);
    case 18: // Enemy : Gray
        return getRGB(130 - help.glow / 2, 130 - help.glow / 2, 130 - help.glow / 2);
    case 19: // Enemy : Dark gray
        return getRGB(60 - help.glow / 8, 60 - help.glow / 8, 60 - help.glow / 8);
        // Purple
    case 20:
        return getRGB(220 - help.glow / 4 - (int) (GETCOL_RANDOM * 20), 120 - help.glow/4, 210 - help.glow/4);

    case 21: // Enemy : Light Gray
        return getRGB(180 - help.glow/2, 180 - help.glow/2, 180 - help.glow/2);
    case 22: // Enemy : Indicator Gray
        return getRGB(230 - help.glow/2, 230 - help.glow/2, 230 - help.glow/2);
    case 23: // Enemy : Indicator Gray
        return getRGB(255 - help.glow / 2 - (int) (GETCOL_RANDOM * 40), 255 - help.glow/2 - (int) (GETCOL_RANDOM * 40), 255 - help.glow/2 - (int) (GETCOL_RANDOM * 40));

    case 24: // Gravity line (Inactive)
        return getRGB(96, 96, 96);
    case 25: // Gravity line (Active)
        if (game.noflashingmode)
        {
            return getRGB(200 - 20, 200 - 20, 200 - 20);
        }

        switch (linestate)
        {
        default:
        case 0:
            return getRGB(200 - 20, 200 - 20, 200 - 20);
        case 1:
            return getRGB(245 - 30, 245 - 30, 225 - 30);
        case 2:
            return getRGB(225 - 30, 245 - 30, 245 - 30);
        case 3:
            return getRGB(200 - 20, 200 - 20, 164 - 10);
        case 4:
            return getRGB(196 - 20, 255 - 30, 224 - 20);
        case 5:
            return getRGB(196 - 20, 235 - 30, 205 - 20);
        case 6:
            return getRGB(164 - 10, 164 - 10, 164 - 10);
        case 7:
            return getRGB(205 - 20, 245 - 30, 225 - 30);
        case 8:
            return getRGB(225 - 30, 255 - 30, 205 - 20);
        case 9:
            return getRGB(245 - 30, 245 - 30, 245 - 30);
        }
    case 26: // Coin
        if (game.noflashingmode)
        {
            return getRGB(234, 234, 10);
        }
        return getRGB(250 - (int) (GETCOL_RANDOM * 32), 250 - (int) (GETCOL_RANDOM * 32), 10);
    case 27: // Particle flashy red
        return getRGB((GETCOL_RANDOM * 64), 10, 10);

    // Trophies
    // cyan
    case 30:
        return RGBf(160, 200, 220);
        // Purple
    case 31:
        return RGBf(220, 120, 210);
        // Yellow
    case 32:
        return RGBf(220, 210, 120);
        // red
    case 33:
        return RGBf(255, 70, 70);
        // green
    case 34:
        return RGBf(120, 220, 120);
        // Blue
    case 35:
        return RGBf(75, 75, 255);
        // Gold
    case 36:
        return getRGB(180, 120, 20);
    case 37: // Trinket
        if (!trinketcolset)
        {
            trinketr = 200 - (GETCOL_RANDOM * 64);
            trinketg = 200 - (GETCOL_RANDOM * 128);
            trinketb = 164 + (GETCOL_RANDOM * 60);
            trinketcolset = true;
        }
        return RGBf(trinketr, trinketg, trinketb);
        // Silver
    case 38:
        return RGBf(196, 196, 196);
        // Bronze
    case 39:
        return RGBf(128, 64, 10);
        // Awesome
    case 40: // Teleporter in action!
    {
        if (game.noflashingmode)
        {
            return getRGB(196, 196, 223);
        }

        const int temp = GETCOL_RANDOM * 150;
        if (temp < 33)
        {
            return RGBf(255 - (GETCOL_RANDOM * 64), 64 + (GETCOL_RANDOM * 64), 64 + (GETCOL_RANDOM * 64));
        }
        else if (temp < 66)
        {
            return RGBf(64 + (GETCOL_RANDOM * 64), 255 - (GETCOL_RANDOM * 64), 64 + (GETCOL_RANDOM * 64));
        }
        else if (temp < 100)
        {
            return RGBf(64 + (GETCOL_RANDOM * 64), 64 + (GETCOL_RANDOM * 64), 255 - (GETCOL_RANDOM * 64));
        }
        else
        {
            return RGBf(164 + (GETCOL_RANDOM * 64), 164 + (GETCOL_RANDOM * 64), 255 - (GETCOL_RANDOM * 64));
        }
    }

    case 100: // Inactive Teleporter
    {
        const int temp = (help.glow / 2) + (GETCOL_RANDOM * 8);
        return getRGB(42 + temp, 42 + temp, 42 + temp);
    }
    case 101: // Active Teleporter
        return getRGB(164 + (GETCOL_RANDOM * 64), 164 + (GETCOL_RANDOM * 64), 255 - (GETCOL_RANDOM * 64));
    case 102: // Teleporter in action!
    {
        if (game.noflashingmode)
        {
            return getRGB(196, 196, 223);
        }

        const int temp = GETCOL_RANDOM * 150;
        if (temp < 33)
        {
            return getRGB(255 - (GETCOL_RANDOM * 64), 64 + (GETCOL_RANDOM * 64), 64 + (GETCOL_RANDOM * 64));
        }
        else if (temp < 66)
        {
            return getRGB(64 + (GETCOL_RANDOM * 64), 255 - (GETCOL_RANDOM * 64), 64 + (GETCOL_RANDOM * 64));
        }
        else if (temp < 100)
        {
            return getRGB(64 + (GETCOL_RANDOM * 64), 64 + (GETCOL_RANDOM * 64), 255 - (GETCOL_RANDOM * 64));
        }
        else
        {
            return getRGB(164 + (GETCOL_RANDOM * 64), 164 + (GETCOL_RANDOM * 64), 255 - (GETCOL_RANDOM * 64));
        }
    }

    case 200: // Normal fishing rod
    {
        return getRGB(92 - help.glow / 4, 128 - help.glow / 4, 128 - help.glow / 4);
    }
    case 201: // Fishing line
    {
        const int temp = GETCOL_RANDOM * 24;
        return getRGB(255 - temp, 255 - temp, 255 - temp);
    }

    case 202: // Largemouth bass layer 1
    {
        return getRGB(128 - help.glow / 2, 140 - help.glow / 2, 64 - help.glow / 2);
    }
    case 203: // Largemouth bass layer 2
    {
        return getRGB(210 - help.glow / 2, 210 - help.glow / 2, 152 - help.glow / 2);
    }
    case 204: // Worm layer 1
    {
        return getRGB(218 - help.glow / 2, 164 - help.glow / 2, 180 - help.glow / 2);
    }
    case 205: // Worm layer 2
    {
        return getRGB(255 - help.glow / 2, 128 - help.glow / 2, 164 - help.glow / 2);
    }
    case 206: // Smallmouth bass layer 1
        return getRGB(152 - help.glow / 2, 128 - help.glow / 2, 96 - help.glow / 2);
    case 207: // Smallmouth bass layer 2
        return getRGB(180 - help.glow / 2, 180 - help.glow / 2, 140 - help.glow / 2);
    case 208: // Bullhead layer 1
        return getRGB(128 - help.glow / 2, 110 - help.glow / 2, 48 - help.glow / 2);
    case 209: // Bullhead layer 2
        return getRGB(175 - help.glow / 2, 148 - help.glow / 2, 80 - help.glow / 2);
    case 210: // Chub layer 1
        return getRGB(142 - help.glow / 2, 160 - help.glow / 2, 172 - help.glow / 2);
    case 211: // Chub layer 2
        return getRGB(200 - help.glow / 2, 92 - help.glow / 2, 38 - help.glow / 2);
    case 212: // Pike layer 1
        return getRGB(100 - help.glow / 2, 110 - help.glow / 2, 72 - help.glow / 2);
    case 213: // Pike layer 2
        return getRGB(160 - help.glow / 2, 150 - help.glow / 2, 116 - help.glow / 2);
    case 214: // Walleye layer 1
        return getRGB(128 - help.glow / 2, 145 - help.glow / 2, 90 - help.glow / 2);
    case 215: // Walleye layer 2
        return getRGB(190 - help.glow / 2, 200 - help.glow / 2, 140 - help.glow / 2);
    case 216: // Carp layer 1
        return getRGB(210 - help.glow / 2, 170 - help.glow / 2, 90 - help.glow / 2);
    case 217: // Carp layer 2
        return getRGB(245 - help.glow / 2, 205 - help.glow / 2, 140 - help.glow / 2);
    case 218: // Perch layer 1
        return getRGB(216 - help.glow / 2, 190 - help.glow / 2, 74 - help.glow / 2);
    case 219: // Perch layer 2
        return getRGB(236 - help.glow / 2, 230 - help.glow / 2, 190 - help.glow / 2);
    case 220: // Catfish layer 1
        return getRGB(104 - help.glow / 2, 94 - help.glow / 2, 76 - help.glow / 2);
    case 221: // Catfish layer 2
        return getRGB(152 - help.glow / 2, 142 - help.glow / 2, 117 - help.glow / 2);
    case 222: // Minnow layer 1
        return getRGB(160 - help.glow / 2, 157 - help.glow / 2, 115 - help.glow / 2);
    case 223: // Minnow layer 2
        return getRGB(104 - help.glow / 2, 93 - help.glow / 2, 82 - help.glow / 2);
    case 224: // Koi layer 1
        return getRGB(230 - help.glow / 2, 238 - help.glow / 2, 244 - help.glow / 2);
    case 225: // Koi layer 2
        return getRGB(255 - help.glow / 2, 82 - help.glow / 2, 58 - help.glow / 2);
    case 226: // Rainbow trout layer 1
        return getRGB(99 - help.glow / 2, 124 - help.glow / 2, 88 - help.glow / 2);
    case 227: // Rainbow trout layer 2
        return getRGB(196 - help.glow / 2, 198 - help.glow / 2, 170 - help.glow / 2);
    case 228: // Rainbow trout layer 3
        return getRGB(243 - help.glow / 2, 106 - help.glow / 2, 134 - help.glow / 2);
    case 229: // Rainbow trout layer 4
        return getRGB(203 - help.glow / 2, 171 - help.glow / 2, 88 - help.glow / 2);
    case 230: // Golden trout layer 1
        return getRGB(160 - help.glow / 2, 127 - help.glow / 2, 67 - help.glow / 2);
    case 231: // Golden trout layer 2
        return getRGB(228 - help.glow / 2, 184 - help.glow / 2, 48 - help.glow / 2);
    case 232: // Golden trout layer 3
        return getRGB(229 - help.glow / 2, 75 - help.glow / 2, 51 - help.glow / 2);
    case 233: // Goldfish layer 1
        return getRGB(236 - help.glow / 2, 126 - help.glow / 2, SDL_max((19 - help.glow / 2), 0));
    case 234: // Goldfish layer 2
        return getRGB(255 - help.glow / 2, 177 - help.glow / 2, SDL_max((19 - help.glow / 2), 0));
    case 235: // Gold goldfish layer 1
    case 236: // Gold goldfish layer 2
    case 237: // Gold goldfish layer 3
    case 238: // Gold goldfish layer 4
    case 239: // Gold goldfish layer 5
    case 240: // Gold goldfish layer 6
    {
        SDL_Color cur = gold_colors[(game.framecounter / 6 + (t - 235)) % 6];
        SDL_Color next = gold_colors[(game.framecounter / 6 + (t - 235) + 1) % 6];
        float time = (game.framecounter % 6) / 6.0f;
        return getRGB(lerpReal(cur.r, next.r, time), lerpReal(cur.g, next.g, time), lerpReal(cur.b, next.b, time));
    }
    case 241: // Prismatic trout, rainbow cycle
        return getRGB(
            sinf(0.05 * game.framecounter + 0 * M_PI / 3) * 127 + 128,
            sinf(0.05 * game.framecounter + 2 * M_PI / 3) * 127 + 128,
            sinf(0.05 * game.framecounter + 4 * M_PI / 3) * 127 + 128
        );
    case 242: // Red snapper, layer 1
        return getRGB(221 - help.glow / 2, 81 - help.glow / 2, 70 - help.glow / 2);
    case 243: // Red snapper, layer 2
        return getRGB(234 - help.glow / 2, 146 - help.glow / 2, 143 - help.glow / 2);
    case 244: // Tuna, layer 1
        return getRGB(167 - help.glow / 2, 210 - help.glow / 2, 211 - help.glow / 2);
    case 245: // Tuna, layer 2
        return getRGB(SDL_max(23 - help.glow / 2, 0), 110 - help.glow / 2, 168 - help.glow / 2);
    case 246: // Tuna, layer 3
        return getRGB(237 - help.glow / 2, 224 - help.glow / 2, 151 - help.glow / 2);

    case 247: // blue marlin layer 1
        return getRGB(195 - help.glow / 2, 209 - help.glow / 2, 224 - help.glow / 2);
    case 248: // blue marlin layer 2
        return getRGB(75 - help.glow / 2, 96 - help.glow / 2, 216 - help.glow / 2);
    case 249: // amberjack layer 1
        return getRGB(192 - help.glow / 2, 196 - help.glow / 2, 173 - help.glow / 2);
    case 250: // amberjack layer 2
        return getRGB(211 - help.glow / 2, 151 - help.glow / 2, 81 - help.glow / 2);
    case 251: // amberjack layer 3
        return getRGB(124 - help.glow / 2, 138 - help.glow / 2, 139 - help.glow / 2);
    case 252: // coalfish layer 1
        return getRGB(114 - help.glow / 2, 137 - help.glow / 2, 145 - help.glow / 2);
    case 253: // coalfish layer 2
        return getRGB(61 - help.glow / 2, 81 - help.glow / 2, 81 - help.glow / 2);
    case 254: // wahoo layer 1
        return getRGB(70 - help.glow / 2, 109 - help.glow / 2, 127 - help.glow / 2);
    case 255: // wahoo layer 2
        return getRGB(159 - help.glow / 2, 186 - help.glow / 2, 198 - help.glow / 2);
    case 256: // anchovy layer 1
        return getRGB(188 - help.glow / 2, 219 - help.glow / 2, 221 - help.glow / 2);
    case 257: // anchovy layer 2
        return getRGB(105 - help.glow / 2, 159 - help.glow / 2, 188 - help.glow / 2);
    case 258: // sardine layer 1
        return getRGB(226 - help.glow / 2, 220 - help.glow / 2, 206 - help.glow / 2);
    case 259: // sardine layer 2
        return getRGB(117 - help.glow / 2, 117 - help.glow / 2, 130 - help.glow / 2);
    case 260: // pompano layer 1
        return getRGB(207 - help.glow / 2, 218 - help.glow / 2, 214 - help.glow / 2);
    case 261: // pompano layer 2
        return getRGB(120 - help.glow / 2, 165 - help.glow / 2, 138 - help.glow / 2);
    case 262: // pompano layer 3
        return getRGB(252 - help.glow / 2, 215 - help.glow / 2, 95 - help.glow / 2);
    case 263: // pufferfish layer 1
        return getRGB(201 - help.glow / 2, 159 - help.glow / 2, 100 - help.glow / 2);
    case 264: // pufferfish layer 2
        return getRGB(243 - help.glow / 2, 231 - help.glow / 2, 205 - help.glow / 2);
    case 265: // mahi-mahi layer 1
        return getRGB(209 - help.glow / 2, 216 - help.glow / 2, 82 - help.glow / 2);
    case 266: // mahi-mahi layer 2
        return getRGB(72 - help.glow / 2, 175 - help.glow / 2, 80 - help.glow / 2);
    case 267: // mahi-mahi layer 3
        return getRGB(76 - help.glow / 2, 109 - help.glow / 2, 165 - help.glow / 2);
    case 268: // moray eel layer 1
        return getRGB(225 - help.glow / 2, 226 - help.glow / 2, 127 - help.glow / 2);
    case 269: // moray eel layer 2
        return getRGB(160 - help.glow / 2, 178 - help.glow / 2, 105 - help.glow / 2);
    case 270: // ribbon eel layer 1
        return getRGB(234 - help.glow / 2, 211 - help.glow / 2, 60 - help.glow / 2);
    case 271: // ribbon eel layer 2
        return getRGB(77 - help.glow / 2, 139 - help.glow / 2, 226 - help.glow / 2);
    case 272: // squid layer 1
        return getRGB(229 - help.glow / 2, 214 - help.glow / 2, 208 - help.glow / 2);
    case 273: // squid layer 2
        return getRGB(193 - help.glow / 2, 150 - help.glow / 2, 143 - help.glow / 2);
    case 274: // octopus layer 1
        return getRGB(158 - help.glow / 2, 63 - help.glow / 2, 93 - help.glow / 2);
    case 275: // octopus layer 2
        return getRGB(135 - help.glow / 2, 35 - help.glow / 2, 66 - help.glow / 2);
    case 276: // star fish
        return getRGB(247 - help.glow / 2, 228 - help.glow / 2, 121 - help.glow / 2);
    case 277: // fishtronaut layer 1
        return getRGB(137 - help.glow / 2, 111 - help.glow / 2, 88 - help.glow / 2);
    case 278: // fishtronaut layer 2
        return getRGB(173 - help.glow / 2, 172 - help.glow / 2, 138 - help.glow / 2);
    case 279: // fishtronaut layer 3
        return getRGB(255 - help.glow / 2, 255 - help.glow / 2, 255 - help.glow / 2);
    case 280: // fishtronaut layer 4
        return getRGB(201 - help.glow / 2, 206 - help.glow / 2, 206 - help.glow / 2);
    case 281: // fishtronaut layer 5
        return getRGB(154 - help.glow / 2, 165 - help.glow / 2, 165 - help.glow / 2);
    case 282: // fishtronaut layer 6
        return getRGB(74 - help.glow / 2, 96 - help.glow / 2, 135 - help.glow / 2);
    case 283: // fishtronaut layer 7
        return getRGB(165 - help.glow / 2, 131 - help.glow / 2, 46 - help.glow / 2);
    case 284: // naval mine layer 1
        return getRGB(61 - help.glow / 2, 58 - help.glow / 2, 60 - help.glow / 2);
    case 285: // naval mine layer 2
        return getRGB(86 - help.glow / 2, 83 - help.glow / 2, 85 - help.glow / 2);
    case 286: // squishfish layer 1
        return getRGB(117 - help.glow / 2, 209 - help.glow / 2, 248 - help.glow / 2);
    case 287: // squishfish layer 2
        return getRGB(158 - help.glow / 2, 255 - help.glow / 2, 227 - help.glow / 2);
    case 288: // squishfish layer 3
        return getRGB(117 - help.glow / 2, 139 - help.glow / 2, 248 - help.glow / 2);

    case 300: // Blue key
        if (game.noflashingmode)
        {
            return getRGB(32, 32, 234);
        }
        return getRGB(32, 32, 250 - (int)(GETCOL_RANDOM * 32));
    case 301: // Purple key
        if (game.noflashingmode)
        {
            return getRGB(234, 10, 234);
        }
        return getRGB(250 - (int)(GETCOL_RANDOM * 32), 10, 250 - (int)(GETCOL_RANDOM * 32));
    case 302: // Red key
        if (game.noflashingmode)
        {
            return getRGB(234, 10, 10);
        }
        return getRGB(250 - (int)(GETCOL_RANDOM * 32), 10, 10);

    case 303: // Bucket
        return getRGB(128 - help.glow / 2, 128 - help.glow / 2, 128 - help.glow / 2);
    case 304: return makeDarker(getWaterColorsForPool("freshwater_small")); // Freshwater
    case 305: return makeDarker(getWaterColorsForPool("saltwater_small")); // Saltwater
    case 306: return makeDarker(getWaterColorsForPool("junk")); // Junk pool
    case 307: return makeDarker(getWaterColorsForPool("special_green")); // Green water
    case 308: return makeDarker(getWaterColorsForPool("special_blue")); // Blue water
    case 309: return makeDarker(getWaterColorsForPool("special_purple")); // Purple water

    case 310: // Enhanced Bait layer 1
        return getRGB(0, 255 - help.glow / 2, 180 - help.glow / 2);
    case 311: // Enhanced Bait layer 2
        return getRGB(0, 200 - help.glow / 2, 140 - help.glow / 2);
    case 312: // Deluxe Bait layer 1
        return getRGB(64, 64, 255 - help.glow / 2);
    case 313: // Deluxe Bait layer 2
        return getRGB(0, 0, 255 - help.glow / 2);
    case 314: // Ultra Bait layer 1
        return getRGB(240 - help.glow / 2, 0, 255 - help.glow / 2);
    case 315: // Ultra Bait layer 2
        return getRGB(220-20 - help.glow / 2, 0, 230 - 20 - help.glow / 2);
    }

    return getRGB(255, 255, 255);
}
#undef GETCOL_RANDOM

void Graphics::menuoffrender(void)
{
    if (copy_texture(gameplayTexture, NULL, NULL) != 0)
    {
        return;
    }

    if (game.gamestate == SHOPMODE)
    {
        cutscenebars();
    }

    const int offset = (int) lerp(oldmenuoffset, menuoffset);
    const SDL_Rect offsetRect = {0, SDL_max(offset, 0), SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS};

    if (copy_texture(menuTexture, NULL, &offsetRect) != 0)
    {
        return;
    }
}

void Graphics::textboxabsolutepos(int x, int y)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxabsolutepos() out-of-bounds!");
        return;
    }

    textboxes[m].position_absolute = true;
    textboxes[m].xp = x;
    textboxes[m].yp = y;
}

void Graphics::textboxcenterx(void)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxcenterx() out-of-bounds!");
        return;
    }

    textboxes[m].should_centerx = true;
}

int Graphics::textboxwidth(void)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxwidth() out-of-bounds!");
        return 0;
    }

    return textboxes[m].w;
}

void Graphics::textboxmoveto(int xo)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxmoveto() out-of-bounds!");
        return;
    }

    textboxes[m].xp = xo;
}

void Graphics::textboxcentery(void)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxcentery() out-of-bounds!");
        return;
    }

    textboxes[m].should_centery = true;
}

void Graphics::textboxpad(size_t left_pad, size_t right_pad)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxpad() out-of-bounds!");
        return;
    }

    textboxes[m].spacing.pad_left = left_pad;
    textboxes[m].spacing.pad_right = right_pad;
}

void Graphics::textboxpadtowidth(size_t new_w)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxpadtowidth() out-of-bounds!");
        return;
    }

    textboxes[m].spacing.padtowidth = new_w;
}

void Graphics::textboxcentertext(void)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxcentertext() out-of-bounds!");
        return;
    }

    textboxes[m].spacing.centertext = true;
}

void Graphics::textboxprintflags(const uint32_t flags)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxprintflags() out-of-bounds!");
        return;
    }

    textboxes[m].print_flags = flags;
    textboxes[m].resize();
}

void Graphics::textboxbuttons(void)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxbuttons() out-of-bounds!");
        return;
    }

    textboxes[m].fill_buttons = true;
}

void Graphics::textboxcrewmateposition(const TextboxCrewmatePosition* crewmate_position)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxcrewmateposition() out-of-bounds!");
        return;
    }

    textboxes[m].crewmate_position = *crewmate_position;
}

void Graphics::textboxoriginalcontext(const TextboxOriginalContext* original_context)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxoriginalcontext() out-of-bounds!");
        return;
    }

    textboxes[m].original = *original_context;
}

void Graphics::textboxoriginalcontextauto(void)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxoriginalcontextauto() out-of-bounds!");
        return;
    }

    TextboxOriginalContext context = TextboxOriginalContext();
    context.text_case = 1;
    context.lines = textboxes[m].lines;
    if (script.running)
    {
        context.script_name = script.scriptname;
    }
    context.x = textboxes[m].xp;
    context.y = textboxes[m].yp;

    textboxes[m].original = context;
}

void Graphics::textboxcase(char text_case)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxcase() out-of-bounds!");
        return;
    }

    textboxes[m].original.text_case = text_case;
}

void Graphics::textboxtranslate(const TextboxTranslate translate, const TextboxFunction function)
{
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxtranslate() out-of-bounds!");
        return;
    }

    if (translate == TEXTTRANSLATE_FUNCTION && function == NULL)
    {
        SDL_assert(0 && "function is NULL!");
        return;
    }
    if (translate != TEXTTRANSLATE_FUNCTION && function != NULL)
    {
        SDL_assert(0 && "function provided when it won't be used!");
        return;
    }

    textboxes[m].translate = translate;
    textboxes[m].function = function;
    textboxes[m].updatetext();
}

static void commsrelay_textbox(textboxclass* THIS)
{
    THIS->lines.clear();

    if (THIS->original.lines.empty())
    {
        return;
    }
    THIS->lines.push_back(loc::gettext(THIS->original.lines[0].c_str()));
    THIS->wrap(11);
    THIS->resize();
    THIS->xp = 224 - THIS->w;
}

void Graphics::textboxcommsrelay(const char* text)
{
    // Special treatment for the gamestate textboxes in Comms Relay
    if (!INBOUNDS_VEC(m, textboxes))
    {
        vlog_error("textboxcommsrelay() out-of-bounds!");
        return;
    }
    textboxprintflags(PR_FONT_INTERFACE);
    textboxes[m].original.lines.push_back(text);
    textboxtranslate(TEXTTRANSLATE_FUNCTION, commsrelay_textbox);
}

int Graphics::crewcolour(const int t)
{
    // Given crewmate t, return colour
    switch (t)
    {
    case 0:
        return EntityColour_CREW_CYAN;
    case 1:
        return EntityColour_CREW_PURPLE;
    case 2:
        return EntityColour_CREW_YELLOW;
    case 3:
        return EntityColour_CREW_RED;
    case 4:
        return EntityColour_CREW_GREEN;
    case 5:
        return EntityColour_CREW_BLUE;
    default:
        return EntityColour_CREW_CYAN;
    }
}

void Graphics::flashlight(void)
{
    set_blendmode(SDL_BLENDMODE_NONE);

    fill_rect(NULL, 0xBB, 0xBB, 0xBB, 0xBB);
}

void Graphics::screenshake(void)
{
    if (gameScreen.badSignalEffect)
    {
        ApplyFilter(&tempFilterSrc, &tempFilterDest);
    }

    set_render_target(tempShakeTexture);
    set_blendmode(SDL_BLENDMODE_NONE);
    clear();

    const SDL_Rect shake = {screenshake_x, screenshake_y, SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS};

    copy_texture(gameTexture, NULL, &shake);

    draw_screenshot_border();

    set_render_target(gameTexture);
    clear();

    // Clear the gameplay texture so blackout() is actually black after a screenshake
    if (game.gamestate == GAMEMODE && game.blackout)
    {
        set_render_target(gameplayTexture);
        clear();
    }

    set_render_target(NULL);
    set_blendmode(SDL_BLENDMODE_NONE);
    draw_window_background();

    SDL_Rect rect;
    get_stretch_info(&rect);

    copy_texture(tempShakeTexture, NULL, &rect, 0, NULL, flipmode ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
}

void Graphics::updatescreenshake(void)
{
    screenshake_x =  static_cast<Sint32>((fRandom() * 7) - 4);
    screenshake_y =  static_cast<Sint32>((fRandom() * 7) - 4);
}

void Graphics::draw_window_background(void)
{
    clear();
}

void Graphics::get_stretch_info(SDL_Rect* rect)
{
    int width;
    int height;
    gameScreen.GetScreenSize(&width, &height);

    switch (gameScreen.scalingMode)
    {
    case SCALING_INTEGER:
    {
        int scale = SDL_min(width / SCREEN_WIDTH_PIXELS, height / SCREEN_HEIGHT_PIXELS);
        rect->x = (width - SCREEN_WIDTH_PIXELS * scale) / 2;
        rect->y = (height - SCREEN_HEIGHT_PIXELS * scale) / 2;
        rect->w = SCREEN_WIDTH_PIXELS * scale;
        rect->h = SCREEN_HEIGHT_PIXELS * scale;
        break;
    }
    case SCALING_LETTERBOX:
        if (width * SCREEN_HEIGHT_PIXELS > height * SCREEN_WIDTH_PIXELS)
        {
            rect->x = (width - height * SCREEN_WIDTH_PIXELS / SCREEN_HEIGHT_PIXELS) / 2;
            rect->y = 0;
            rect->w = height * SCREEN_WIDTH_PIXELS / SCREEN_HEIGHT_PIXELS;
            rect->h = height;
        }
        else
        {
            rect->x = 0;
            rect->y = (height - width * SCREEN_HEIGHT_PIXELS / SCREEN_WIDTH_PIXELS) / 2;
            rect->w = width;
            rect->h = width * SCREEN_HEIGHT_PIXELS / SCREEN_WIDTH_PIXELS;
        }
        break;
    case SCALING_STRETCH:
        /* Could pass NULL to copy_texture instead, but this feels better */
        rect->x = 0;
        rect->y = 0;
        rect->w = width;
        rect->h = height;
        break;
    default:
        SDL_assert(0 && "Invalid scaling mode!");
        rect->x = 0;
        rect->y = 0;
        rect->w = width;
        rect->h = height;
    }

    // In case anything accidentally set the width/height to 0, we'll clamp it to avoid crashing from a division by 0
    rect->w = SDL_max(1, rect->w);
    rect->h = SDL_max(1, rect->h);
}

void Graphics::render(void)
{
    ime_render();
    draw_screenshot_border();

    if (gameScreen.badSignalEffect)
    {
        ApplyFilter(&tempFilterSrc, &tempFilterDest);
    }

    set_render_target(NULL);
    set_blendmode(SDL_BLENDMODE_NONE);

    draw_window_background();

    SDL_Rect stretch_info;
    get_stretch_info(&stretch_info);

    ime_set_rect(&stretch_info);

    copy_texture(gameTexture, NULL, &stretch_info, 0, NULL, flipmode ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
}

void Graphics::renderwithscreeneffects(void)
{
    if (game.flashlight > 0 && !game.noflashingmode)
    {
        flashlight();
    }

    if (game.screenshake > 0 && !game.noflashingmode)
    {
        screenshake();
    }
    else
    {
        render();
    }
}

void Graphics::renderfixedpre(void)
{
    if (game.screenshake > 0)
    {
        updatescreenshake();
    }

    if (gameScreen.badSignalEffect)
    {
        UpdateFilter();
    }
}

void Graphics::renderfixedpost(void)
{
    /* Screen effects timers */
    if (game.flashlight > 0)
    {
        --game.flashlight;
    }
    if (game.screenshake > 0)
    {
        --game.screenshake;
    }

    game.old_screenshot_border_timer = game.screenshot_border_timer;
    if (game.screenshot_border_timer > 0)
    {
        game.screenshot_border_timer -= 15;
    }
}

void Graphics::draw_screenshot_border(void)
{
    const int border_alpha = lerp(game.old_screenshot_border_timer, game.screenshot_border_timer);

    if (border_alpha <= 100)
    {
        return;
    }

    int width = 0;
    int height = 0;
    int result = query_texture(gameTexture, NULL, NULL, &width, &height);
    if (result != 0)
    {
        return;
    }

    const SDL_Rect rect_inner = {1, 1, width - 2, height - 2};

    if (game.screenshot_saved_success)
    {
        set_color(196, 196, 20, border_alpha);
    }
    else
    {
        set_color(196, 20, 20, border_alpha);
    }

    set_blendmode(SDL_BLENDMODE_BLEND);
    draw_rect(NULL);
    draw_rect(&rect_inner);
    set_blendmode(SDL_BLENDMODE_NONE);
}

void Graphics::drawtele(int x, int y, int t, const SDL_Color color)
{
    SDL_Rect telerect;
    setRect(telerect, x, y, tele_rect.w, tele_rect.h);

    draw_grid_tile(grphx.im_teleporter, 0, x, y, tele_rect.w, tele_rect.h, 16, 16, 16);

    if (t > 9) t = 8;
    if (t < 1) t = 1;

    draw_grid_tile(grphx.im_teleporter, t, x, y, tele_rect.w, tele_rect.h, color);
}

SDL_Color Graphics::getRGBA(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
{
    const SDL_Color color = {r, g, b, a};
    return color;
}

SDL_Color Graphics::getRGB(const Uint8 r, const Uint8 g, const Uint8 b)
{
    const SDL_Color color = {r, g, b, 255};
    return color;
}

SDL_Color Graphics::RGBf(int r, int g, int b)
{
    r = (r + 128) / 3;
    g = (g + 128) / 3;
    b = (b + 128) / 3;
    const SDL_Color color = {(Uint8) r, (Uint8) g, (Uint8) b, 255};
    return color;
}

bool Graphics::onscreen(int t)
{
    return (t >= -40 && t <= 280);
}

bool Graphics::checktexturesize(
    const char* filename, SDL_Texture* texture,
    const int tilewidth, const int tileheight
) {
    int texturewidth;
    int textureheight;
    if (query_texture(texture, NULL, NULL, &texturewidth, &textureheight) != 0)
    {
        /* Just give it the benefit of the doubt. */
        vlog_warn(
            "Assuming the dimensions of %s are exact multiples of %i by %i!",
            filename, tilewidth, tileheight
        );
        return true;
    }

    const bool valid = texturewidth % tilewidth == 0 && textureheight % tileheight == 0;
    if (!valid)
    {
        FILESYSTEM_setLevelDirError(
            loc::gettext("{filename} dimensions not exact multiples of {width} by {height}!"),
            "filename:str, width:int, height:int",
            filename, tilewidth, tileheight
        );
        return false;
    }

    return true;
}

static void make_array(
    SDL_Surface** tilesheet,
    std::vector<SDL_Surface*>& vector,
    const int tile_square
) {
    int j;
    for (j = 0; j < (*tilesheet)->h / tile_square; j++)
    {
        int i;
        for (i = 0; i < (*tilesheet)->w / tile_square; i++)
        {
            SDL_Surface* temp = GetSubSurface(
                *tilesheet,
                i * tile_square, j * tile_square,
                tile_square, tile_square
            );
            vector.push_back(temp);
        }
    }
}

bool Graphics::reloadresources(void)
{
    grphx.destroy();
    grphx.init();

    MAYBE_FAIL(checktexturesize("tiles.png", grphx.im_tiles, 8, 8));
    MAYBE_FAIL(checktexturesize("tiles2.png", grphx.im_tiles2, 8, 8));
    MAYBE_FAIL(checktexturesize("tiles3.png", grphx.im_tiles3, 8, 8));
    MAYBE_FAIL(checktexturesize("entcolours.png", grphx.im_entcolours, 8, 8));
    MAYBE_FAIL(checktexturesize("sprites.png", grphx.im_sprites, 32, 32));
    MAYBE_FAIL(checktexturesize("flipsprites.png", grphx.im_flipsprites, 32, 32));
    MAYBE_FAIL(checktexturesize("teleporter.png", grphx.im_teleporter, 96, 96));

    destroy();

    make_array(&grphx.im_sprites_surf, sprites_surf, 32);
    make_array(&grphx.im_flipsprites_surf, flipsprites_surf, 32);

    images[IMAGE_LEVELCOMPLETE] = grphx.im_image0;
    images[IMAGE_MINIMAP] = grphx.im_image1;
    images[IMAGE_COVERED] = grphx.im_image2;
    images[IMAGE_ELEPHANT] = grphx.im_image3;
    images[IMAGE_GAMECOMPLETE] = grphx.im_image4;
    images[IMAGE_FLIPLEVELCOMPLETE] = grphx.im_image5;
    images[IMAGE_FLIPGAMECOMPLETE] = grphx.im_image6;

    images[IMAGE_SITE] = grphx.im_image7;
    images[IMAGE_SITE2] = grphx.im_image8;
    images[IMAGE_SITE3] = grphx.im_image9;
    images[IMAGE_ENDING] = grphx.im_image10;
    images[IMAGE_SITE4] = grphx.im_image11;
    images[IMAGE_CUSTOMMINIMAP] = grphx.im_image12;

    gameScreen.LoadIcon();

    music.destroy();
    music.init();

    tiles1_mounted = FILESYSTEM_isAssetMounted("graphics/tiles.png");
    tiles2_mounted = FILESYSTEM_isAssetMounted("graphics/tiles2.png");
    minimap_mounted = FILESYSTEM_isAssetMounted("graphics/minimap.png");

    gamecomplete_mounted = FILESYSTEM_isAssetMounted("graphics/gamecomplete.png");
    levelcomplete_mounted = FILESYSTEM_isAssetMounted("graphics/levelcomplete.png");
    flipgamecomplete_mounted = FILESYSTEM_isAssetMounted("graphics/flipgamecomplete.png");
    fliplevelcomplete_mounted = FILESYSTEM_isAssetMounted("graphics/fliplevelcomplete.png");

    return true;

fail:
    return false;
}

SDL_Color Graphics::crewcolourreal(int t)
{
    switch (t)
    {
    case 0:
        return col_crewcyan;
    case 1:
        return col_crewpurple;
    case 2:
        return col_crewyellow;
    case 3:
        return col_crewred;
    case 4:
        return col_crewgreen;
    case 5:
        return col_crewblue;
    }
    return col_crewcyan;
}

void Graphics::render_roomname(uint32_t font_flag, const char* roomname, int r, int g, int b)
{
    int font_height = font::height(font_flag);
    if (font_height <= 8)
    {
        footerrect.h = font_height + 2;
    }
    else
    {
        footerrect.h = font_height + 1;
    }
    footerrect.y = 240 - footerrect.h;

    set_blendmode(SDL_BLENDMODE_BLEND);
    fill_rect(&footerrect, getRGBA(0, 0, 0, translucentroomname ? 127 : 255));
    font::print(font_flag | PR_CEN | PR_BOR | PR_CJK_LOW, -1, footerrect.y+1, roomname, r, g, b);
    set_blendmode(SDL_BLENDMODE_NONE);
}

void Graphics::print_roomtext(int x, const int y, const char* text, const bool rtl)
{
    uint32_t flags = PR_FONT_LEVEL | PR_CJK_LOW;

    if (rtl)
    {
        flags |= PR_RIGHT;
        x += 8; // the size of a tile, not font width!
    }
    font::print(flags, x, y, text, 196, 196, 255 - help.glow);
}
