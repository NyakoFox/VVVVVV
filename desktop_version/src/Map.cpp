#define MAP_DEFINITION
#include "Map.h"

#include "Alloc.h"
#include "Constants.h"
#include "CustomLevels.h"
#include "Entity.h"
#include "Game.h"
#include "GlitchrunnerMode.h"
#include "Graphics.h"
#include "Localization.h"
#include "MakeAndPlay.h"
#include "Maths.h"
#include "Music.h"
#include "Script.h"
#include "Unused.h"
#include "UtilityClass.h"

mapclass::mapclass(void)
{
    //Start here!
    colstatedelay = 0;
    colsuperstate = 0;
    spikeleveltop = 0;
    spikelevelbottom = 0;
    oldspikeleveltop = 0;
    oldspikelevelbottom = 0;
    warpx = false;
    warpy = false;
    extrarow = 0;

    showteleporters = false;
    showtargets = false;
    showtrinkets = false;

    finalmode = false;
    finalstretch = false;

    cursorstate = 0;
    cursordelay = 0;

    towermode = false;
    cameraseekframe = 0;
    resumedelay = 0;

    final_colormode = false;
    final_colorframe = 0;
    final_colorframedelay = 0;
    final_mapcol = 0;
    final_aniframe = 0;
    final_aniframedelay = 0;

    custommode=false;
    custommodeforreal=false;
    custommmxoff=0; custommmyoff=0; custommmxsize=0; custommmysize=0;
    customzoom=0;
    customshowmm=true;

    rcol = 0;

    //This needs to be in map instead!
    invincibility = false;

    //We create a blank map
    SDL_memset(contents, 0, sizeof(contents));

    SDL_memset(roomdeaths, 0, sizeof(roomdeaths));
    SDL_memset(roomdeathsfinal, 0, sizeof(roomdeathsfinal));
    resetmap();

    setroomname("");
    hiddenname = "";

    roomname_special = false;
    specialroomnames.clear();
    roomnameset = false;

    tileset = 0;
    initmapdata();

    ypos = 0;
    oldypos = 0;

    background = 0;
    cameramode = 0;
    cameraseek = 0;
    minitowermode = false;
    roomtexton = false;

    nexttowercolour_set = false;
}

static char roomname_static[SCREEN_WIDTH_CHARS];
static char* roomname_heap;

void mapclass::destroy(void)
{
    VVV_free(roomname_heap);
}

//Areamap starts at 100,100 and extends 20x20
const int mapclass::areamap[] = {
    1,2,2,2,2,2,2,2,0,3,0,0,0,4,4,4,4,4,4,4,
    1,2,2,2,2,2,2,0,0,3,0,0,0,0,4,4,4,4,4,4,
    0,1,0,0,2,0,0,0,0,3,0,0,0,0,4,4,4,4,4,4,
    0,0,0,0,2,0,0,0,0,3,0,0,5,5,5,5,4,4,4,4,
    0,0,2,2,2,0,0,0,0,3,11,11,5,5,5,5,0,0,0,0,
    0,0,0,0,0,0,0,0,0,3,5,5,5,5,5,5,0,0,0,0,
    0,0,0,0,0,0,0,0,0,3,5,5,5,5,5,5,5,0,0,0,
    0,0,0,0,0,0,0,0,0,3,5,5,5,5,5,5,5,5,5,0,
    0,0,0,0,0,0,0,0,0,3,0,0,0,5,5,5,5,5,5,0,
    0,0,0,0,0,0,0,0,11,3,0,0,0,5,5,5,5,5,5,0,
    0,0,0,0,0,0,0,0,0,3,0,0,0,5,5,5,5,5,5,0,
    0,0,0,0,0,0,0,0,0,3,0,5,5,5,5,5,5,5,5,0,
    0,0,0,0,0,0,0,0,0,3,0,5,5,5,5,5,5,0,5,0,
    0,0,0,0,0,0,0,0,0,3,0,5,5,5,5,5,5,0,5,0,
    0,0,0,0,0,0,0,0,0,3,0,5,5,0,0,0,0,0,5,0,
    0,0,0,0,0,0,0,2,0,3,0,0,0,0,0,0,0,0,0,0,
    0,0,2,2,2,2,2,2,0,3,0,0,0,0,0,0,0,0,0,0,
    0,2,2,2,2,2,2,2,0,3,0,0,0,0,0,0,0,0,0,0,
    2,2,2,2,2,0,0,2,0,3,0,0,0,0,0,0,0,0,0,0,
    2,2,2,2,2,0,0,2,0,3,0,0,0,0,0,0,0,0,0,0,
};

int mapclass::getwidth(void)
{
    if (custommode)
    {
        return cl.mapwidth;
    }

    return 20;
}

int mapclass::getheight(void)
{
    if (custommode)
    {
        return cl.mapheight;
    }

    return 20;
}

int mapclass::intpol(int a, int b, float c)
{
    return static_cast<int>(a + ((b - a) * c));
}

void mapclass::setteleporter(int x, int y)
{
    if (x < 0 || x >= getwidth() || y < 0 || y >= getheight())
    {
        return;
    }

    SDL_Point temp;
    temp.x = x;
    temp.y = y;
    teleporters.push_back(temp);
}

void mapclass::settrinket(int x, int y)
{
    if (x < 0 || x >= getwidth() || y < 0 || y >= getheight())
    {
        return;
    }

    SDL_Point temp;
    temp.x = x;
    temp.y = y;
    shinytrinkets.push_back(temp);
}

void mapclass::setroomname(const char* name)
{
    VVV_free(roomname_heap);

    const size_t size = SDL_strlcpy(
        roomname_static, name, sizeof(roomname_static)
    ) + 1;
    roomname = roomname_static;

    if (size > sizeof(roomname_static))
    {
        roomname_heap = SDL_strdup(name);
        if (roomname_heap != NULL)
        {
            roomname = roomname_heap;
        }
    }
}

void mapclass::resetmap(void)
{
    //clear the explored area of the map
    SDL_memset(explored, 0, sizeof(explored));
}

void mapclass::updateroomnames(void)
{
    if (roomnameset)
    {
        return;
    }

    const int rx = game.roomx;
    const int ry = game.roomy;

    for (int i = specialroomnames.size() - 1; i >= 0; i--)
    {
        Roomname* roomname = &specialroomnames[i];
        if (rx == roomname->x && ry == roomname->y && (roomname->flag == -1 || (INBOUNDS_ARR(roomname->flag, obj.flags) && obj.flags[roomname->flag])))
        {
            roomname_special = true;
            if (roomname->type == RoomnameType_STATIC)
            {
                setroomname(roomname->text[0].c_str());
            }
            if (roomname->type == RoomnameType_GLITCH)
            {
                roomname->delay--;
                if (roomname->delay <= 0)
                {
                    roomname->progress = (roomname->progress + 1) % 2;
                    roomname->delay = 5;
                    if (roomname->progress == 0)
                    {
                        roomname->delay = 25 + (int) (fRandom() * 10);
                    }
                }
                setroomname(roomname->text[roomname->progress].c_str());
            }
            if (roomname->type == RoomnameType_TRANSFORM)
            {
                roomname->delay--;
                if (roomname->delay <= 0)
                {
                    roomname->progress++;
                    roomname->delay = 2;
                    if ((size_t) roomname->progress >= roomname->text.size())
                    {
                        roomname->progress = roomname->loop ? 0 : roomname->text.size() - 1;
                    }
                }
                setroomname(roomname->text[roomname->progress].c_str());
            }
            break;
        }
    }
}

void mapclass::initmapdata(void)
{
    if (custommode)
    {
        initcustommapdata();
        return;
    }

    teleporters.clear();
    shinytrinkets.clear();

    //Set up static map information like teleporters and shiny trinkets.
    setteleporter(0, 0);
    setteleporter(0, 16);
    setteleporter(2, 4);
    setteleporter(2, 11);
    setteleporter(7, 9);
    setteleporter(7, 15);
    setteleporter(8, 11);
    setteleporter(10, 5);
    setteleporter(11, 4);
    setteleporter(13, 2);
    setteleporter(13, 8);
    setteleporter(14, 19);
    setteleporter(15, 0);
    setteleporter(17, 12);
    setteleporter(17, 17);
    setteleporter(18, 1);
    setteleporter(18, 7);

    settrinket(14, 4);
    settrinket(13, 6);
    settrinket(11, 12);
    settrinket(15, 12);
    settrinket(14, 11);
    settrinket(18, 14);
    settrinket(11, 7);
    settrinket(9, 2);
    settrinket(9, 16);
    settrinket(2, 18);
    settrinket(7, 18);
    settrinket(6, 1);
    settrinket(17, 3);
    settrinket(10, 19);
    settrinket(5, 15);
    settrinket(1, 10);
    settrinket(3, 2);
    settrinket(10, 8);

    //Special room names
    specialroomnames.clear();

    {
        static const char* lines[] = {
            "Television Newsvel",
            "TelevisvonvNewsvel",
            "TvlvvvsvonvNevsvel",
            "vvvvvvsvovvNe svel",
            "vhv vvv'vvovv vevl",
            "vhv V v'Cvovv vewv",
            "vhe 9 v'Cvovv vewv",
            "vhe 9 v'Cvovv Newv",
            "The 9 O'Cvovk Newv",
            "The 9 O'Clock News"
        };

        roomnamechange(45, 51, lines, SDL_arraysize(lines));
    }

    {
        static const char* lines[] = {
            "Vwitvhed",
            "vVwivcvedv",
            "vvvwMvcvMdvv",
            "DvvvwMvfvvMdvvv",
            "Dvav Mvfvr Mdvvvv",
            "Diav M for Mdrver",
            "Dial M for Murder"
        };

        roomnamechange(46, 51, lines, SDL_arraysize(lines));
    }

    {
        static const char* lines[] = {
            "Gvnsmove",
            "Gvnvmovevv",
            "Gunvmove1vv6",
            "Vunsmoke 19v6",
            "Gunsmoke 1966"
        };

        roomnamechange(47, 51, lines, SDL_arraysize(lines));
    }

    {
        static const char* lines[] = {
            "Please envoy theve repeats",
            "Plse envoy tse rvpvas",
            "Plse envoy tse rvpvas",
            "Vl envoy te rvevs",
            "Vv evo tv vevs",
            "Iv vhv Mvrvivs",
            "In the Margins"
        };

        roomnamechange(50, 53, lines, SDL_arraysize(lines));
    }

    {
        static const char* lines[] = {
            "Try Viggling the Antenna",
            "TryJivglvng theAvtevna",
            "Tvvivglvng thAvtvvv",
            "Vvvgglvnv tvnvva",
            "Vvavvnvs vvtv",
            "Veavvn's Gvte",
            "Heaven's Gate"
        };

        roomnamechange(50, 54, lines, SDL_arraysize(lines));
    }

    roomnameglitch(42, 51, "Rear Window", "Rear Vindow");
    roomnameglitch(48, 51, "On the Waterfront", "On the Vaterfront");
    roomnameglitch(49, 51, "The Untouchables", "The Untouchavles");
}

void mapclass::roomnameglitch(int x, int y, const char* name, const char* text)
{
    Roomname roomname;
    roomname.x = x;
    roomname.y = y;
    roomname.type = RoomnameType_GLITCH;
    roomname.flag = -1;
    roomname.loop = false;
    roomname.progress = 1;
    roomname.delay = -1;

    roomname.text.push_back(name);
    roomname.text.push_back(text);

    specialroomnames.push_back(roomname);
}

void mapclass::roomnamechange(const int x, const int y, const char** lines, const size_t size)
{
    Roomname roomname;
    roomname.x = x;
    roomname.y = y;
    roomname.type = RoomnameType_TRANSFORM;
    roomname.flag = 72; // Flag 72 is synced with finalstretch
    roomname.loop = false;
    roomname.progress = 0;
    roomname.delay = 2;

    roomname.text.insert(roomname.text.end(), lines, lines + size);

    specialroomnames.push_back(roomname);
}

void mapclass::initcustommapdata(void)
{
    shinytrinkets.clear();

    for (size_t i = 0; i < customentities.size(); i++)
    {
        const CustomEntity& ent = customentities[i];
        if (ent.t != 9)
        {
            continue;
        }

        settrinket(ent.rx, ent.ry);
    }
}

int mapclass::finalat(int x, int y)
{
    //return the tile index of the final stretch tiles offset by the colour difference
    const int tile = contents[TILE_IDX(x, y)];
    if (tile == 740)
    {
        //Special case: animated tiles
        if (final_mapcol == 1)
        {
            int offset;
            if (game.noflashingmode)
            {
                offset = 0;
            }
            else
            {
                offset = int(fRandom() * 11) * 40;
            }
            return 737 + offset;
        }
        else
        {
            int offset;
            if (game.noflashingmode)
            {
                offset = 0;
            }
            else
            {
                offset = final_aniframe * 40;
            }
            return tile - (final_mapcol * 3) + offset;
        }
    }
    else if (tile >= 80)
    {
        return tile - (final_mapcol * 3);
    }
    else
    {
        return tile;
    }
}

int mapclass::maptiletoenemycol(int t)
{
    //returns the colour index for enemies that matches the map colour t
    switch(t)
    {
    case 0:
        return 11;
        break;
    case 1:
        return 6;
        break;
    case 2:
        return 8;
        break;
    case 3:
        return 12;
        break;
    case 4:
        return 9;
        break;
    case 5:
        return 7;
        break;
    case 6:
        return 18;
        break;
    }
    return 11;
}

void mapclass::changefinalcol(int t)
{
    //change the map to colour t - for the game's final stretch.
    //First up, the tiles. This is just a setting:
    final_mapcol = t;
    const int temp = 6 - t;
    //Next, entities
    for (size_t i = 0; i < obj.entities.size(); i++)
    {
        if (obj.entities[i].type == 1) //something with a movement behavior
        {
            if (obj.entities[i].animate == 10 || obj.entities[i].animate == 11) //treadmill
            {
                if(temp<3)
                {
                    obj.entities[i].tile = 907 + (temp * 80);
                }
                else
                {
                    obj.entities[i].tile = 911 + ((temp-3) * 80);
                }
                if(obj.entities[i].animate == 10)    obj.entities[i].tile += 40;
            }
            else if (obj.entities[i].isplatform)
            {
                obj.entities[i].tile = 915+(temp*40);
            }
            else    //just an enemy
            {
                obj.entities[i].colour = maptiletoenemycol(temp);
            }
        }
        else if (obj.entities[i].type == 2)    //disappearing platforms
        {
            obj.entities[i].tile = 915+(temp*40);
        }
    }
}

void mapclass::setcol(TowerBG& bg_obj, const int r1, const int g1, const int b1 , const int r2, const int g2, const int b2, const int c)
{
    bg_obj.r = intpol(r1, r2, c / 5);
    bg_obj.g = intpol(g1, g2, c / 5);
    bg_obj.b = intpol(b1, b2, c / 5);
}

void mapclass::updatebgobj(TowerBG& bg_obj)
{
    const int check = bg_obj.colstate % 5; //current state of phase
    const int cmode = (bg_obj.colstate - check) / 5; // current colour transition;

    switch(cmode)
    {
    case 0:
        setcol(bg_obj, 255, 93, 107, 255, 255, 93, check);
        break;
    case 1:
        setcol(bg_obj, 255, 255, 93, 159, 255, 93, check);
        break;
    case 2:
        setcol(bg_obj, 159, 255, 93, 93, 245, 255, check);
        break;
    case 3:
        setcol(bg_obj, 93, 245, 255, 177, 93, 255, check);
        break;
    case 4:
        setcol(bg_obj, 177, 93, 255, 255, 93, 255, check);
        break;
    case 5:
        setcol(bg_obj, 255, 93, 255, 255, 93, 107, check);
        break;
    }

    bg_obj.tdrawback = true;
}

void mapclass::setbgobjlerp(TowerBG& bg_obj)
{
    bg_obj.bypos = ypos / 2;
    bg_obj.bscroll = (ypos - oldypos) / 2;
}

void mapclass::updatetowerglow(TowerBG& bg_obj)
{
    if (colstatedelay <= 0 || colsuperstate > 0)
    {
        if (colsuperstate > 0) bg_obj.colstate--;
        bg_obj.colstate++;
        if (bg_obj.colstate >= 30) bg_obj.colstate = 0;

        const int check = bg_obj.colstate % 5;
        updatebgobj(bg_obj);

        if (check == 0)
        {
            colstatedelay = 45;
        }
        else
        {
            colstatedelay = 0;
        }
        if (colsuperstate > 0) colstatedelay = 0;
    }
    else
    {
        colstatedelay--;
    }
}

void mapclass::nexttowercolour(void)
{
    /* Prevent cycling title BG more than once per frame. */
    if (nexttowercolour_set)
    {
        return;
    }
    nexttowercolour_set = true;

    graphics.titlebg.colstate+=5;
    if (graphics.titlebg.colstate >= 30) graphics.titlebg.colstate = 0;

    updatebgobj(graphics.titlebg);
}

void mapclass::settowercolour(int t)
{
    graphics.titlebg.colstate=t*5;
    if (graphics.titlebg.colstate >= 30) graphics.titlebg.colstate = 0;

    updatebgobj(graphics.titlebg);
}

bool mapclass::towerspikecollide(int x, int y)
{
    if (tower.at(x,y,0)>= 6 && tower.at(x,y,0) <= 11) return true;
    return false;
}

void mapclass::remove_large_tile_at(int x, int y)
{
    x /= 16;
    y /= 16;

    if (x < 0 || x >= room_width || y < 0 || y >= room_height) return;

    large_contents[x + y * room_width] = 0;
    obj.createentity(x * 16, y * 16, 116);
}

bool mapclass::collide(int x, int y, const bool invincible)
{
    if (largermode)
    {
        x /= 2;
        y /= 2;

        if (x < 0 || x >= room_width || y < 0 || y >= room_height) return false;

        if (smbmode())
        {
            if (x <= 0 || x >= room_width - 1)
            {
                return true;
            }

            switch (large_contents[x + y * room_width])
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 7:
            case 8:
            case 11:
            case 12:
            case 13:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 32:
            case 33:
            case 34:
            case 35:
            case 36:
            case 41:
            case 42:
                return true;
            default:
                return false;
            }
        }

        if (cavestorymode())
        {
            switch (large_contents[x + y * room_width])
            {
            case 1:
            case 2: // SLOPES
                //case 3: // SLOPES
                //case 4: // SLOPES
            case 5: // SLOPES
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 32:
            case 33:
            case 35:
            case 36:
            case 50: // SLOPES
                //case 51: // SLOPES
                //case 52: // SLOPES
            case 53: // SLOPES
            case 56: // Rolling spawner
            case 61: // Breakable block
            case 65:
            case 70:
            case 71:
            case 72:
            case 73:
            case 76:
            case 77:
            case 81:
            case 86:
            case 88:
            case 89:
            case 92:
            case 93:
            case 110:
            case 111:
                game.lastcollidedwallx = x;
                game.lastcollidedwally = y;
                return true;
            default:
                return false;
            }
        }

        return false;
    }
    if (towermode)
    {
        if (tower.at(x, y, 0) >= 12 && tower.at(x, y, 0) <= 27) return true;
        if (invincible)
        {
            if (tower.at(x, y, 0) >= 6 && tower.at(x, y, 0) <= 11) return true;
        }
    }
    else if (tileset == 2)
    {
        int tile;
        if (y == -1) return collide(x, y + 1, invincible);
        if (y == 29+extrarow) return collide(x, y - 1, invincible);
        if (x == -1) return collide(x + 1, y, invincible);
        if (x == 40) return collide(x - 1, y, invincible);
        if (x < 0 || y < 0 || x >= 40 || y >= 29 + extrarow) return false;
        tile = contents[TILE_IDX(x, y)];
        if (tile >= 12 && tile <= 27) return true;
        if (invincible)
        {
            if (tile >= 6 && tile <= 11) return true;
        }
    }
    else
    {
        int tile;
        if (y == -1) return collide(x, y + 1, invincible);
        if (y == 29+extrarow) return collide(x, y - 1, invincible);
        if (x == -1) return collide(x + 1, y, invincible);
        if (x == 40) return collide(x - 1, y, invincible);
        if (x < 0 || y < 0 || x >= 40 || y >= 29+extrarow) return false;
        tile = contents[TILE_IDX(x, y)];
        if (tile == 1) return true;
        if (tileset==0 && tile == 59) return true;
        if (tile>= 80 && tile < 680) return true;
        if (tile == 740 && tileset==1) return true;
        if (invincible)
        {
            if (tile>= 6 && tile <= 9) return true;
            if (tile>= 49 && tile <= 50) return true;
            if (tileset == 1)
            {
                if (tile>= 49 && tile < 80) return true;
            }
        }
    }
    return false;
}

void mapclass::settile(int xp, int yp, int t)
{
    if (largermode)
    {
        if (xp >= 0 && xp < room_width && yp >= 0 && yp < room_height)
        {
            large_contents[xp + yp * room_width] = t;
            // spawn dust
            obj.createentity(xp * 16, yp * 16, 116);
        }
        return;
    }
    if (xp >= 0 && xp < 40 && yp >= 0 && yp < 29+extrarow)
    {
        contents[TILE_IDX(xp, yp)] = t;
    }
}


int mapclass::area(int _rx, int _ry)
{
    //THIS IS THE BUG
    if (finalmode)
    {
        return 6;
    }
    else
    {
        int lookup = (_rx - 100) + ((_ry - 100) * 20);
        if(_rx-100>=0 && _rx-100<20 && _ry-100>=0 && _ry-100<20){
            return areamap[lookup];
        }
        else
        {
            return 6;
        }
    }
}

bool mapclass::isexplored(const int rx, const int ry)
{
    const int roomnum = rx + ry*20;
    if (INBOUNDS_ARR(roomnum, explored))
    {
        return explored[roomnum];
    }
    return false;
}

void mapclass::setexplored(const int rx, const int ry, const bool status)
{
    const int roomnum = rx + ry*20;
    if (INBOUNDS_ARR(roomnum, explored))
    {
        explored[roomnum] = status;
    }
}

void mapclass::exploretower(void)
{
    for (int i = 0; i < 20; i++)
    {
        setexplored(9, i, true);
    }
}

void mapclass::hideship(void)
{
    //remove the ship from the explored areas
    setexplored(2, 10, false);
    setexplored(3, 10, false);
    setexplored(4, 10, false);
    setexplored(2, 11, false);
    setexplored(3, 11, false);
    setexplored(4, 11, false);
}

void mapclass::showship(void)
{
    //show the ship in the explored areas
    setexplored(2, 10, true);
    setexplored(3, 10, true);
    setexplored(4, 10, true);
    setexplored(2, 11, true);
    setexplored(3, 11, true);
    setexplored(4, 11, true);
}

void mapclass::resetplayer(void)
{
    resetplayer(false);
}

void mapclass::resetplayer(const bool player_died)
{
    bool was_in_tower = towermode;
    if ((game.roomx != game.saverx || game.roomy != game.savery) || game.bossbattle || smbmode())
    {
        if (game.bossbattle)
        {
            music.haltdasmusik();
            game.bossbattle = false;
        }
        if (smbmode())
        {
            music.play(3);
            xpos = 0;
        }
        gotoroom(game.saverx, game.savery);
    }

    game.deathseq = -1;
    int i = obj.getplayer();
    if(INBOUNDS_VEC(i, obj.entities))
    {
        obj.entities[i].vx = 0;
        obj.entities[i].vy = 0;
        obj.entities[i].ax = 0;
        obj.entities[i].ay = 0;
        obj.entities[i].xp = game.savex;
        obj.entities[i].yp = game.savey;

        //Fix conveyor death loop glitch
        obj.entities[i].newxp = obj.entities[i].xp;
        obj.entities[i].newyp = obj.entities[i].yp;

        obj.entities[i].dir = game.savedir;
        obj.entities[i].colour = game.savecolour;
        if (player_died)
        {
            game.respawnseq = 30;
            game.lifeseq = 10;
            obj.entities[i].invis = true;
        }
        else
        {
            obj.entities[i].invis = false;
        }
        if (!GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2))
        {
            obj.entities[i].size = 0;
            obj.entities[i].cx = 6;
            obj.entities[i].cy = 2;
            obj.entities[i].w = 12;
            obj.entities[i].h = 21;
        }

        setplayer();

        // If we entered a tower as part of respawn, reposition camera
        if (!was_in_tower && towermode)
        {
            ypos = obj.entities[i].yp - 120;
            if (ypos < 0)
            {
                ypos = 0;
            }
            oldypos = ypos;
            setbgobjlerp(graphics.towerbg);
        }
    }

    if (game.state == 0 && !script.running && game.completestop)
    {
        /* Looks like a collection dialogue was interrupted.
         * Undo its effects! */
        game.advancetext = false;
        graphics.showcutscenebars = false;
        if (music.currentsong > -1)
        {
            music.fadeMusicVolumeIn(3000);
        }
    }

    game.scmhurt = false; //Just in case the supercrewmate is fucking this up!
    if (game.supercrewmate)
    {
        if (game.roomx == game.scmprogress + 41)
        {
            game.scmprogress = game.roomx - 41;
        }
        else
        {
            game.scmprogress = game.roomx - 40;
        }
    }
}

void mapclass::warpto(int rx, int ry , int t, int tx, int ty)
{
    gotoroom(rx, ry);
    game.teleport = false;
    if (INBOUNDS_VEC(t, obj.entities))
    {
        obj.entities[t].xp = tx * 8;
        obj.entities[t].yp = (ty * 8) - obj.entities[t].h;
        obj.entities[t].lerpoldxp = obj.entities[t].xp;
        obj.entities[t].lerpoldyp = obj.entities[t].yp;
    }
    game.gravitycontrol = 0;
}

void mapclass::gotoroom(int rx, int ry)
{
    int roomchangedir;
    std::vector<entclass> linecrosskludge;

    //First, destroy the current room
    obj.removeallblocks();
    game.activetele = false;
    game.readytotele = 0;
    game.oldreadytotele = 0;

    //Ok, let's save the position of all lines on the screen
    for (size_t i = 0; i < obj.entities.size(); i++)
    {
        if (obj.entities[i].type == 9)
        {
            //It's a horizontal line
            if (obj.entities[i].xp <= 0 || (obj.entities[i].xp + obj.entities[i].w) >= 312)
            {
                //it's on a screen edge
                obj.copylinecross(linecrosskludge, i);
            }
        }
    }

    /* Disable all entities in the room, and deallocate any unnecessary entity slots. */
    /* However don't disable player entities, but do preserve holes between them (if any). */
    bool player_found = false;
    for (int i = obj.entities.size() - 1; i >= 0; --i)
    {
        /* Iterate in reverse order to prevent unnecessary indice shifting */
        if (obj.entities[i].rule == 0)
        {
            player_found = true;
            continue;
        }

        if (!player_found)
        {
            obj.entities.erase(obj.entities.begin() + i);
        }
        else
        {
            obj.disableentity(i);
        }
    }

    if (rx < game.roomx)
    {
        roomchangedir = 0;
    }
    else
    {
        roomchangedir = 1;
    }

    if (finalmode)
    {
        //Ok, what way are we moving?
        game.roomx = rx;
        game.roomy = ry;

        if (game.roomy < 10)
        {
            game.roomy = 11;
        }

        if(game.roomx>=41 && game.roomy>=48 && game.roomx<61 && game.roomy<68 )
        {
            game.currentroomdeaths = roomdeathsfinal[game.roomx - 41 + (20 * (game.roomy - 48))];
        }
        else
        {
            game.currentroomdeaths = 0;
        }

        //Final level for time trial
        if (game.intimetrial && game.roomx == 46 && game.roomy == 54)
        {
            music.niceplay(Music_PREDESTINEDFATEREMIX);
        }
    }
    else if (custommode)
    {
        game.roomx = rx;
        game.roomy = ry;
        if (game.roomx < 100) game.roomx = 100 + cl.mapwidth-1;
        if (game.roomy < 100) game.roomy = 100 + cl.mapheight-1;
        if (game.roomx > 100 + cl.mapwidth-1) game.roomx = 100;
        if (game.roomy > 100 + cl.mapheight-1) game.roomy = 100;
    }
    else
    {
        game.roomx = rx;
        game.roomy = ry;
        if (game.roomx < 100) game.roomx = 119;
        if (game.roomy < 100) game.roomy = 119;
        if (game.roomx > 119) game.roomx = 100;
        if (game.roomy > 119) game.roomy = 100;

        game.currentroomdeaths = roomdeaths[game.roomx - 100 + (20 * (game.roomy - 100))];

        //Alright, change music depending on where we are:
        music.changemusicarea(game.roomx - 100, game.roomy - 100);
    }
    loadlevel(game.roomx, game.roomy);


    //Do we need to reload the background?
    bool redrawbg = game.roomx != game.prevroomx || game.roomy != game.prevroomy;

    if (redrawbg)
    {
        graphics.backgrounddrawn = false; //Used for background caching speedup
    }
    graphics.foregrounddrawn = false; //Used for background caching speedup

    game.prevroomx = game.roomx;
    game.prevroomy = game.roomy;

    //a very special case: if entering the communication room, room 13,4 before tag 5 is set, set the game state to a background
    //textbox thingy. if tag five is not set when changing room, reset the game state. (tag 5 is set when you get back to the ship)
    if(!game.intimetrial && !custommode)
    {
        if (!obj.flags[5] && !finalmode)
        {
            game.setstate(0);
            if (game.roomx == 113 && game.roomy == 104)
            {
                game.setstate(50);
            }
        }
    }

    //Ok, kludge to fix lines in crossing rooms - if we're intersecting a gravity line right now, let's
    //set it to an inactive state.

    //Alright! So, let's look at our lines from the previous rooms, and determine if any of them are actually
    //continuations!

    const int player_idx = obj.getplayer();
    if (INBOUNDS_VEC(player_idx, obj.entities))
    {
        entclass* player = &obj.entities[player_idx];
        player->oldxp = player->xp;
        player->oldyp = player->yp;
        player->lerpoldxp = player->xp - int(player->vx);
        player->lerpoldyp = player->yp - int(player->vy);
    }

    for (size_t i = 0; i < obj.entities.size(); i++)
    {
        if (obj.entities[i].type == 9)
        {
            //It's a horizontal line
            if (obj.entities[i].xp <= 0 || obj.entities[i].xp + obj.entities[i].w >= 312)
            {
                //it's on a screen edge
                for (size_t j = 0; j < linecrosskludge.size(); j++)
                {
                    if (obj.entities[i].yp == linecrosskludge[j].yp)
                    {
                        //y's match, how about x's?
                        //we're moving left:
                        if (roomchangedir == 0)
                        {
                            if (obj.entities[i].xp + obj.entities[i].w >= 312 && linecrosskludge[j].xp <= 0)
                            {
                                obj.revertlinecross(linecrosskludge, i, j);
                            }
                        }
                        else
                        {
                            if (obj.entities[i].xp <= 0 && linecrosskludge[j].xp + linecrosskludge[j].w >= 312)
                            {
                                obj.revertlinecross(linecrosskludge, i, j);
                            }
                        }
                    }
                }
            }
        }
    }

    if (game.companion > 0)
    {
        //We've changed room? Let's bring our companion along!
        spawncompanion();
    }
}

void mapclass::spawncompanion(void)
{
    int i = obj.getplayer();
    if (!INBOUNDS_VEC(i, obj.entities))
    {
        return;
    }

    //ok, we'll presume our companion has been destroyed in the room change. So:
    switch(game.companion)
    {
    case 6:
    {
        obj.createentity(obj.entities[i].xp, 121.0f, 15.0f,1);  //Y=121, the floor in that particular place!
        int j = obj.getcompanion();
        if (INBOUNDS_VEC(j, obj.entities))
        {
            obj.entities[j].vx = obj.entities[i].vx;
            obj.entities[j].dir = obj.entities[i].dir;
        }
        break;
    }
    case 7:
        if (game.roomy <= 105)   //don't jump after him!
        {
            if (game.roomx == 110)
            {
                obj.createentity(320, 86, 16, 1);  //Y=86, the ROOF in that particular place!
            }
            else
            {
                obj.createentity(obj.entities[i].xp, 86.0f, 16.0f, 1);  //Y=86, the ROOF in that particular place!
            }
            int j = obj.getcompanion();
            if (INBOUNDS_VEC(j, obj.entities))
            {
                obj.entities[j].vx = obj.entities[i].vx;
                obj.entities[j].dir = obj.entities[i].dir;
            }
        }
        break;
    case 8:
        if (game.roomy >= 104)   //don't jump after him!
        {
            if (game.roomx == 102)
            {
                obj.createentity(310, 177, 17, 1);
                int j = obj.getcompanion();
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[j].vx = obj.entities[i].vx;
                    obj.entities[j].dir = obj.entities[i].dir;
                }
            }
            else
            {
                obj.createentity(obj.entities[i].xp, 177.0f, 17.0f, 1);
                int j = obj.getcompanion();
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[j].vx = obj.entities[i].vx;
                    obj.entities[j].dir = obj.entities[i].dir;
                }
            }
        }
        break;
    case 9:
        if (!towermode)   //don't go back into the tower!
        {
            if (game.roomx == 110 && obj.entities[i].xp<20)
            {
                obj.createentity(100, 185, 18, 15, 0, 1);
            }
            else
            {
                obj.createentity(obj.entities[i].xp, 185.0f, 18.0f, 15, 0, 1);
            }
            int j = obj.getcompanion();
            if (INBOUNDS_VEC(j, obj.entities))
            {
                obj.entities[j].vx = obj.entities[i].vx;
                obj.entities[j].dir = obj.entities[i].dir;
            }
        }
        break;
    case 10:
        //intermission 2, choose colour based on lastsaved
        if (game.roomy == 51)
        {
            if (!obj.flags[59])
            {
                obj.createentity(225.0f, 169.0f, 18, graphics.crewcolour(game.lastsaved), 0, 10);
                int j = obj.getcompanion();
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[j].vx = obj.entities[i].vx;
                    obj.entities[j].dir = obj.entities[i].dir;
                }
            }
        }
        else    if (game.roomy >= 52)
        {
            if (obj.flags[59])
            {
                obj.createentity(160.0f, 177.0f, 18, graphics.crewcolour(game.lastsaved), 0, 18, 1);
                int j = obj.getcompanion();
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[j].vx = obj.entities[i].vx;
                    obj.entities[j].dir = obj.entities[i].dir;
                }
            }
            else
            {
                obj.flags[59] = true;
                obj.createentity(obj.entities[i].xp, -20.0f, 18.0f, graphics.crewcolour(game.lastsaved), 0, 10, 0);
                int j = obj.getcompanion();
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[j].vx = obj.entities[i].vx;
                    obj.entities[j].dir = obj.entities[i].dir;
                }
            }
        }
        break;
    case 11:
        //Intermission 1: We're using the SuperCrewMate instead!
        if(game.roomx-41==game.scmprogress)
        {
            switch(game.scmprogress)
            {
            case 0:
                obj.createentity(76, 161, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 1:
                obj.createentity(10, 169, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 2:
                obj.createentity(10, 177, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 3:
                obj.createentity(10, 177, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 4:
                obj.createentity(10, 185, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 5:
                obj.createentity(10, 185, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 6:
                obj.createentity(10, 185, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 7:
                obj.createentity(10, 41, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 8:
                obj.createentity(10, 169, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 9:
                obj.createentity(10, 169, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 10:
                obj.createentity(10, 129, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 11:
                obj.createentity(10, 129, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 12:
                obj.createentity(10, 65, 24, graphics.crewcolour(game.lastsaved), 2);
                break;
            case 13:
                obj.createentity(10, 177, 24, graphics.crewcolour(game.lastsaved));
                break;
            }
        }
        break;
    }
}

const char* mapclass::currentarea(const int roomx, const int roomy)
{
    /* For translation, the returned value is passed to loc::gettext_roomname_special().
     * Returned strings must therefore be found in roomnames_special.xml! */

    if (roomx >= 102 && roomx <= 104 && roomy >= 110 && roomy <= 111)
    {
        return "The Ship";
    }

    switch (area(roomx, roomy))
    {
    case 0:
        return "Dimension VVVVVV";
        break;
    case 1:
        return "Dimension VVVVVV";
        break;
    case 2:
        return "Laboratory";
        break;
    case 3:
        return "The Tower";
        break;
    case 4:
        return "Warp Zone";
        break;
    case 5:
        return "Space Station";
        break;
    case 6:
        return "Outside Dimension VVVVVV";
        break;
    case 7:
        return "Outside Dimension VVVVVV";
        break;
    case 8:
        return "Outside Dimension VVVVVV";
        break;
    case 9:
        return "Outside Dimension VVVVVV";
        break;
    case 10:
        return "Outside Dimension VVVVVV";
        break;
    case 11:
        return "The Tower";
        break;
    }
    return "???";
}

static void copy_short_to_int(int* dest, const short* src, const size_t size)
{
    size_t i;
    for (i = 0; i < size; ++i)
    {
        dest[i] = src[i];
    }
}

bool mapclass::cavestorymode()
{
    return game.specialmode == SpecialMode_CAVE;
}

bool mapclass::smbmode()
{
    return game.specialmode == SpecialMode_SMB;
}

void mapclass::setplayer()
{
    int i = obj.getplayer();
    if (INBOUNDS_VEC(i, obj.entities))
    {
        switch (game.specialmode)
        {
        case SpecialMode_CAVE:
            obj.entities[i].tile = 192;
            obj.entities[i].w = 14;
            obj.entities[i].h = 15;
            obj.entities[i].cx = 5;
            obj.entities[i].cy = 8;
            break;
        case SpecialMode_SMB:
            obj.entities[i].tile = 0;
            obj.entities[i].w = 14;
            obj.entities[i].h = 15;
            obj.entities[i].cx = 5;
            obj.entities[i].cy = 8;
            break;
        case SpecialMode_NONE:
            if (obj.entities[i].tile == 192)
            {
                obj.entities[i].tile = 0;
            }
            obj.entities[i].size = 0;
            obj.entities[i].cx = 6;
            obj.entities[i].cy = 2;
            obj.entities[i].w = 12;
            obj.entities[i].h = 21;
            break;
        }
    }
}

void mapclass::load_large_map(int rx, int ry)
{
    if (rx == 100 && ry == 101)
    {
        static const short room_contents[] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,52,53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,52,52,53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,52,53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,52,52,53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,52,53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,52,52,53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,52,53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,52,52,53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,51,52,53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,52,53,0,0,0,0,0,0,0,0,67,68,69,0,0,0,0,0,51,52,52,52,53,0,0,0,0,67,68,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,52,53,0,0,0,0,0,0,0,0,67,68,69,0,0,0,0,0,51,52,52,52,53,0,0,0,0,67,68,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,52,53,0,0,0,0,0,0,0,0,67,68,69,0,0,0,0,0,51,52,52,52,53,0,0,0,0,67,68,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,52,53,0,0,0,0,0,0,0,0,67,68,69,0,0,0,0,0,51,52,52,52,53,0,0,0,0,67,68,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,51,52,53,0,0,0,0,0,0,0,0,67,68,69,0,0,0,0,0,51,52,52,52,53,0,0,0,0,0,0,0,0,67,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,67,68,68,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,67,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,67,68,68,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,67,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,67,68,68,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,67,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,67,68,68,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,67,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,67,68,68,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,2,2,2,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,2,3,3,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,0,0,0,0,0,0,0,0,5,0,0,0,0,21,21,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,2,7,2,3,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,17,0,0,0,0,0,0,0,0,0,12,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,7,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,2,41,0,0,0,0,3,0,0,3,0,0,3,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,4,0,0,4,0,0,0,0,0,0,0,0,0,0,4,4,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,2,2,3,2,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,5,0,0,0,0,38,37,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,17,0,0,0,0,0,0,32,33,0,0,9,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,0,0,4,4,0,0,0,0,9,0,0,0,4,4,4,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,4,0,0,0,0,9,0,0,0,5,0,0,0,21,22,22,22,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,25,26,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,16,17,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,32,33,0,24,25,26,0,0,0,0,0,32,33,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,25,26,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,0,0,4,4,4,0,0,24,25,26,0,4,4,4,4,0,0,4,4,4,0,0,0,9,0,16,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,17,0,4,4,4,4,4,4,4,4,0,0,0,24,25,26,0,0,5,0,0,0,37,37,23,37,37,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,25,10,25,26,0,0,0,0,0,0,48,49,49,49,50,24,25,26,0,0,0,0,48,49,50,0,0,32,33,0,0,0,0,0,0,0,0,32,33,0,48,49,49,50,0,32,33,24,25,10,25,26,0,0,0,0,32,33,48,49,49,49,50,24,25,26,0,0,0,0,48,49,50,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,48,49,49,50,0,0,0,24,25,10,25,26,0,0,0,0,0,0,48,49,49,49,50,24,25,26,0,0,0,0,48,49,50,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,49,49,4,4,4,4,24,25,10,25,4,4,4,4,4,0,0,4,4,4,4,50,24,25,26,32,33,0,0,48,49,50,0,0,0,0,0,0,0,0,0,32,33,4,4,4,4,4,4,4,4,4,0,0,24,25,10,25,26,0,4,0,0,0,37,37,40,42,37,50,24,25,26,0,0,0,0,48,49,50,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
        };

        room_width = 224;
        room_height = 15;

        large_contents.assign(room_contents, room_contents + SDL_arraysize(room_contents));

        setroomname("World 1-1");

        obj.createblock(TRIGGER, 288, 0, 16, 208, 0, "custom_smbintro_load");
        obj.createblock(TRIGGER, 1104, 240, 32, 16, 1, "custom_kill_load");
        obj.createblock(TRIGGER, 1376, 240, 48, 16, 2, "custom_kill_load");
        obj.createblock(TRIGGER, 2448, 240, 32, 16, 3, "custom_kill_load");
        obj.createblock(TRIGGER, 3168, 48, 16, 48, 4, "custom_flag_load");

        obj.customactivitytext = "Press {button} to enter pipe";
        obj.customscript = "enter_pipe_load";
        obj.createblock(ACTIVITY, 912, 128, 32, 16, 35);

        obj.customactivitytext = "Press {button} to enter castle";
        obj.customscript = "enter_castle_load";
        obj.createblock(ACTIVITY, 3264, 176, 16, 32, 35);
    }

    if (rx == 101 && ry == 100)
    {
        static const short room_contents[] = {
            33,16,17,32,33,32,33,32,33,32,33,32,33,16,17,1,1,1,1,16,17,35,32,33,36,35,36,35,36,35,36,35,36,35,32,33,36,1,108,1,32,33,1,1,1,1,1,20,1,20,1,20,1,20,1,1,1,1,1,0,1,1,1,35,36,35,36,35,36,35,36,35,36,35,36,35,16,17,1,1,0,1,16,17,19,20,19,20,19,20,19,20,19,20,19,20,19,32,33,1,7,0,8,32,33,35,36,35,36,35,36,35,36,35,36,35,16,17,1,1,1,0,0,22,16,17,19,20,19,20,19,20,19,20,19,20,19,32,33,0,0,0,0,0,24,32,33,35,36,35,36,35,36,35,36,35,16,17,1,0,0,0,0,1,1,108,1,1,19,20,19,20,19,20,19,20,19,32,33,0,0,0,0,0,0,1,38,38,1,35,36,35,36,35,36,35,16,17,1,0,0,0,0,0,16,17,1,38,39,1,19,20,19,20,19,20,19,32,33,0,0,0,0,0,0,32,33,1,108,1,19,35,36,35,36,35,16,17,1,0,0,0,0,0,16,17,36,1,38,38,1,35,19,20,19,20,19,32,33,0,0,0,0,0,0,32,33,1,1,39,38,16,17,35,36,35,16,17,1,0,0,0,0,0,16,17,36,1,36,1,1,108,32,33,19,20,19,32,33,0,0,0,0,0,24,32,33,1,19,20,1,0,0,8,19,35,16,17,1,7,0,0,0,0,16,17,36,1,36,35,36,1,0,0,24,35,17,32,33,1,0,0,0,0,0,32,33,1,1,20,19,20,1,23,52,16,17,33,7,66,1,0,0,0,16,17,36,1,36,1,16,17,16,17,108,35,32,33,20,9,0,1,0,82,83,32,33,1,19,20,1,32,33,32,33,1,19,1,19,36,25,0,1,0,16,17,1,1,36,2,3,1,0,1,1,0,1,35,1,35,17,9,0,0,0,32,33,1,16,17,0,0,60,0,66,67,0,0,8,16,17,33,51,0,0,0,4,5,1,32,33,0,0,60,0,0,0,82,83,0,32,33,16,17,0,0,0,0,0,1,66,67,0,0,1,1,0,0,1,1,0,1,19,32,33,23,0,0,0,0,1,0,0,0,0,1,36,23,24,35,1,0,1,35,19,1,1,0,0,0,0,60,0,0,0,0,1,20,19,20,19,7,0,16,17,35,36,9,0,0,0,0,60,0,0,0,0,1,36,35,36,35,23,0,32,33,19,20,25,0,0,0,0,60,0,0,0,0,1,20,19,20,19,1,25,38,1,35,36,23,83,0,0,24,1,0,0,0,0,1,36,35,36,35,1,0,38,1,16,17,16,17,24,16,17,1,0,0,0,0,16,17,19,20,16,17,0,16,17,32,33,32,33,35,32,33,1,16,17,16,17,32,33,16,17,32,33,0,32,33,16,17,16,17,16,17,16,17,32,33,32,33,16,17,32,33,0,0,0,1,19,32,33,32,33,32,33,32,33,35,36,35,36,32,33,0,0,0,0,0,1,35,16,17,16,17,16,17,16,17,19,20,19,16,17,1,0,0,0,0,0,16,17,32,33,32,33,32,33,32,33,35,36,35,32,33,1,0,0,1,16,17,32,33,16,17,2,3,0,0,0,0,66,67,66,0,66,1,0,0,4,32,33,1,19,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,1,35,16,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,17,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,19,16,17,0,0,0,16,17,16,17,16,17,16,17,16,17,16,17,16,17,1,35,32,33,0,0,0,32,33,32,33,32,33,32,33,32,33,32,33,32,33,16,17,16,17,0,0,0,110,111,16,17,16,17,16,17,16,17,16,17,16,17,32,33,32,33,0,0,0,110,111,32,33,32,33,32,33,32,33,32,33,32,33,1,19,16,17,0,0,0,110,111,16,17,16,17,16,17,16,17,16,17,16,17,1,35,32,33,0,0,0,110,111,32,33,32,33,32,33,32,33,32,33,32,33,16,17,16,17,0,0,0,16,17,16,17,16,17,16,17,16,17,16,17,16,17,32,33,32,33,0,0,0,32,33,32,33,32,33,32,33,32,33,32,33,32,33,1,19,19,1,38,38,38,1,35,36,35,36,35,36,35,36,35,36,35,36,35,1,35,16,17,0,0,0,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,32,33,0,0,0,32,33,32,33,32,33,32,33,32,33,32,33,32,33,32,33,16,17,0,0,0,110,111,16,17,16,17,16,17,16,17,16,17,16,17,1,19,32,33,0,0,0,110,111,32,33,32,33,32,33,32,33,32,33,32,33,1,35,16,17,0,0,0,110,111,16,17,16,17,16,17,16,17,16,17,16,17,16,17,32,33,0,0,0,110,111,32,33,32,33,32,33,32,33,32,33,32,33,32,33,16,17,0,0,0,16,17,16,17,16,17,16,17,16,17,16,17,16,17,1,19,32,33,0,0,0,32,33,32,33,32,33,32,33,32,33,32,33,32,33,1,35,19,1,38,38,38,1,35,36,35,36,35,36,35,36,35,36,35,36,35,16,17,16,17,0,0,6,16,17,16,17,16,17,16,17,16,17,16,17,16,17,32,33,32,33,0,0,0,32,33,32,33,32,33,32,33,32,33,32,33,32,33,1,19,16,17,0,0,0,110,111,16,17,16,17,16,17,16,17,16,17,16,17,1,35,32,33,0,0,0,110,111,32,33,32,33,32,33,32,33,32,33,32,33,16,17,16,17,0,0,0,110,111,16,17,16,17,16,17,16,17,16,17,16,17,32,33,32,33,0,0,0,110,111,32,33,32,33,32,33,32,33,32,33,32,33,1,19,16,17,0,0,0,16,17,16,17,16,17,16,17,16,17,16,17,16,17,1,35,32,33,0,0,22,32,33,32,33,32,33,32,33,32,33,32,33,32,33,16,17,19,1,38,38,38,1,35,36,35,36,35,36,35,36,35,36,35,36,35,32,33,16,17,0,0,0,16,17,16,17,16,17,16,17,16,17,16,17,16,17,1,19,32,33,0,0,0,32,33,32,33,32,33,32,33,32,33,32,33,32,33,1,35,16,17,0,0,0,110,111,16,17,16,17,16,17,16,17,16,17,16,17,16,17,32,33,0,0,0,110,111,32,33,32,33,32,33,32,33,32,33,32,33,32,33,16,17,0,0,0,110,111,16,17,16,17,16,17,16,17,16,17,16,17,1,19,32,33,0,0,0,110,111,32,33,32,33,32,33,32,33,32,33,32,33,1,35,16,17,0,0,6,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,32,33,0,0,22,32,33,32,33,32,33,32,33,32,33,32,33,32,33,32,33,19,1,38,38,38,1,35,36,35,36,35,36,35,36,35,36,35,36,35,1,19,16,17,25,0,0,16,17,16,17,16,17,16,17,16,17,16,17,16,17,1,35,32,33,9,0,0,32,33,32,33,32,33,32,33,32,33,32,33,32,33,16,17,16,17,25,0,0,110,111,16,17,16,17,16,17,16,17,16,17,16,17,32,33,32,33,0,0,0,110,111,32,33,32,33,32,33,32,33,32,33,32,33,1,19,16,17,0,0,0,110,111,16,17,16,17,16,17,16,17,16,17,16,17,1,35,32,33,0,0,0,110,111,32,33,32,33,32,33,32,33,32,33,32,33,16,17,16,17,9,0,0,16,17,16,17,16,17,16,17,16,17,16,17,16,17,32,33,32,33,25,0,0,32,33,32,33,32,33,32,33,32,33,32,33,32,33,1,19,19,1,38,38,38,1,35,36,35,36,35,36,35,36,35,36,35,36,35,1,35,16,17,0,0,0,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,32,33,0,0,0,32,33,32,33,32,33,32,33,32,33,32,33,32,33,32,33,16,17,0,0,0,110,111,16,17,16,17,16,17,16,17,16,17,16,17,1,19,32,33,0,0,0,110,111,32,33,32,33,32,33,32,33,32,33,32,33,1,35,16,17,0,0,0,110,111,16,17,16,17,16,17,16,17,16,17,16,17,16,17,32,33,0,0,6,110,111,32,33,32,33,32,33,32,33,32,33,32,33,32,33,16,17,0,0,22,16,17,16,17,16,17,16,17,16,17,16,17,16,17,1,19,32,33,9,0,0,32,33,32,33,32,33,32,33,32,33,32,33,32,33,1,35,19,1,38,38,38,38,38,1,19,20,19,20,19,20,19,20,19,20,19,16,17,16,17,0,0,0,38,39,1,35,36,35,36,35,36,35,36,35,36,35,32,33,32,33,38,38,38,38,1,1,1,1,1,1,1,1,1,1,1,1,1,1,19,16,17,0,0,0,0,0,0,0,0,66,67,0,0,66,43,27,26,1,1,35,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,10,41,16,17,16,17,25,0,0,0,0,0,0,0,0,0,0,0,0,0,59,26,57,32,33,32,33,23,0,0,0,0,0,0,0,0,0,0,0,0,0,18,18,16,17,19,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,32,33,35
        };

        room_width = 21;
        room_height = 100;

        large_contents.assign(room_contents, room_contents + SDL_arraysize(room_contents));

        obj.customactivitytext = "Press {button} to enter door";
        obj.customscript = "door1_load";
        obj.createblock(ACTIVITY, 288, 1536, 16, 32, 35);

        obj.createentity(304, 240, 20, 1);
        obj.customscript = "welcometohell";
        obj.createblock(ACTIVITY, 304, 240, 16, 16, 35);

        if (!obj.flags[0])
        {
            obj.customactivitytext = "Press {button} to pick up Violet";
            obj.customscript = "violet_pickup_load";
            obj.createblock(ACTIVITY, 272, 1552, 16, 16, 35);

            obj.createentity(272 - 7, 1552 - 6, 105, 0);
        }
        else
        {
            obj.createentity(0, 0, 105, obj.flags[4] ? 2 : 1);
        }

        obj.createblock(TRIGGER, 208, 320, 16, 32, 0, "custom_lore1");
        obj.createblock(TRIGGER, 256, 1520, 16, 48, 1, "custom_lore2");
        setroomname("Sacred Grounds");

        // SPIKES

        // 2x2

        obj.createentity(240, 112, 100); // Spike staircase
        obj.createentity(208, 144, 100); // Spike staircase
        obj.createentity(176, 176, 100); // Spike staircase
        obj.createentity(144, 208, 100); // Spike staircase
        obj.createentity(112, 240, 100); // Spike staircase
        obj.createentity(80,  272, 100); // Spike staircase

        obj.createentity(34, 416, 100);
        obj.createentity(80, 416, 100);
        obj.createentity(80, 416, 100);
        obj.createentity(128, 432, 100);
        obj.createentity(160, 432, 100);
        obj.createentity(224, 352, 100);

        obj.createentity(80, 592, 100);
        obj.createentity(112, 592, 100);
        obj.createentity(144, 592, 100);
        obj.createentity(176, 592, 100);
        obj.createentity(208, 592, 100);
        obj.createentity(240, 592, 100);
        obj.createentity(272, 592, 100);

        obj.createentity(32, 1552, 100);
        obj.createentity(64, 1552, 100);
        obj.createentity(96, 1552, 100);
        obj.createentity(128, 1552, 100);
        obj.createentity(160, 1552, 100);
        obj.createentity(192, 1552, 100);

        // 1x1

        obj.createentity(16, 336, 101, 1);  // Up
        obj.createentity(32, 368, 101, 1);  // Up
        obj.createentity(64, 448, 101, 1);  // Up
        obj.createentity(112, 416, 101, 1); // Up

        obj.createentity(64, 672, 101, 0); // Left
        obj.createentity(64, 688, 101, 0); // Left

        obj.createentity(224, 1568, 101, 1); // Up
        obj.createentity(240, 1568, 101, 1); // Up

        // Checkpoint
        obj.createentity(96, 1488, 10, 1, 6969); // Checkpoint
    }
    if (rx == 102 && ry == 100)
    {
        static const short room_contents[] = {
            16,17,16,17,16,17,16,17,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,1,1,32,33,32,33,32,33,32,33,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,16,17,16,17,16,17,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,0,0,32,33,16,17,32,33,32,33,32,33,32,33,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,16,17,16,17,16,17,16,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,17,32,33,32,33,32,33,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,16,17,16,17,16,17,16,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,17,32,33,32,33,32,33,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,16,17,11,10,11,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,17,32,33,27,26,27,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,16,17,11,10,11,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,17,32,33,27,26,27,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,52,1,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,16,17,41,10,11,10,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,17,0,0,0,0,0,0,1,16,17,25,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,82,83,52,16,17,1,1,16,17,54,54,54,54,54,54,16,17,1,1,16,17,32,33,57,26,27,26,27,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,16,17,32,33,23,0,0,0,0,0,38,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,82,83,0,0,0,0,0,0,0,0,37,0,0,34,37,0,16,17,16,17,32,33,19,20,32,33,1,38,38,38,39,1,32,33,19,20,32,33,16,17,16,17,16,17,16,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,82,83,52,32,33,1,1,1,0,0,0,0,1,38,0,0,83,38,52,51,82,83,0,0,52,1,1,51,0,0,0,52,53,1,1,51,0,1,1,23,83,0,52,53,1,1,50,53,1,32,33,32,33,16,17,35,36,16,17,35,1,39,38,1,36,16,17,35,36,16,17,32,33,32,33,32,33,32,33,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,1,1,16,17,32,33,16,17,32,33,16,17,38,38,16,17,32,33,16,17,32,33
        };

        room_width = 160;
        room_height = 16;

        large_contents.assign(room_contents, room_contents + SDL_arraysize(room_contents));

        obj.customactivitytext = "Press {button} to enter door";
        obj.customscript = "door1_back_load";
        obj.createblock(ACTIVITY, 32, 192, 16, 32, 35);

        setroomname("Sacred Grounds");

        obj.createentity(0, 0, 105, obj.flags[4] ? 2 : 1);

        obj.createentity(1424 + 8, 224, 10, 1, 6972); // Checkpoint

        obj.createblock(TRIGGER, 1520, 32, 16, 144, 0, "custom_lore3");
        obj.createblock(TRIGGER, 2400, 240, 32, 16, 1, "custom_lore4");
        obj.createblock(TRIGGER, 2400, 256, 32, 16, 2, "custom_door2_load");
    }
    if (rx == 103 && ry == 100)
    {
        static const short room_contents[] = {
            1,1,1,1,1,1,1,70,71,1,1,1,70,71,1,1,1,1,1,1,1,1,1,1,70,71,88,88,70,71,70,71,70,71,70,71,70,71,88,88,70,71,1,70,71,70,71,70,71,1,70,71,1,1,1,1,1,1,1,1,1,1,1,1,1,1,16,17,1,1,1,88,88,88,1,1,1,1,1,16,17,1,16,17,1,16,17,1,16,17,1,16,17,1,1,1,1,1,89,89,89,1,1,1,16,17,1,16,17,1,16,17,1,16,17,1,1,70,71,1,1,1,1,1,1,1,1,1,1,1,1,1,16,17,1,16,17,1,16,17,1,16,17,1,16,17,1,16,17,1,16,17,1,38,38,1,16,17,16,17,1,1,1,1,1,1,70,71,18,18,18,18,18,70,71,1,1,1,1,1,1,1,18,0,0,0,0,0,0,0,0,0,61,0,0,0,0,0,0,0,0,0,1,0,0,0,74,0,0,74,0,0,1,1,18,1,1,0,0,0,0,0,0,0,0,0,32,33,38,1,1,0,0,0,38,38,38,38,38,32,33,1,32,33,1,32,33,1,32,33,1,32,33,1,1,16,17,1,88,88,88,1,1,1,32,33,0,32,33,0,32,33,0,32,33,0,0,1,0,0,1,0,1,1,1,1,1,1,2,3,4,5,32,33,1,32,33,1,32,33,1,32,33,1,32,33,1,32,33,1,32,33,54,54,54,54,32,33,32,33,17,20,19,20,19,70,71,1,18,18,18,18,18,1,70,71,19,20,19,20,16,17,18,0,0,0,0,0,0,0,0,0,21,0,0,0,0,0,0,0,0,0,65,0,0,0,90,0,0,90,0,0,16,17,18,0,0,0,0,0,0,0,0,0,0,0,65,0,38,38,38,0,0,0,38,38,38,38,39,38,38,38,38,38,38,38,38,38,38,1,1,1,38,38,38,32,33,0,0,0,0,4,5,1,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,8,1,20,19,20,19,1,19,20,19,20,19,1,16,17,0,0,0,0,0,0,0,0,0,0,0,1,33,36,35,36,70,71,35,1,18,18,18,18,18,1,35,70,71,35,35,36,32,33,18,0,0,0,0,0,0,0,0,0,74,0,0,0,0,0,0,0,0,0,81,0,0,0,106,0,0,90,0,0,32,33,7,0,0,0,0,0,0,0,0,0,0,0,81,0,0,38,38,0,0,0,38,38,39,38,38,38,39,38,38,38,38,38,38,39,38,38,38,39,38,38,38,18,0,0,0,0,0,0,0,65,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,6,1,36,35,36,35,1,35,36,35,36,1,1,32,33,0,0,0,0,0,0,0,0,0,0,0,110,17,20,19,70,71,20,19,1,18,18,18,18,18,1,19,20,70,71,19,20,16,17,18,0,0,0,0,0,0,0,0,0,90,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,90,0,0,16,17,51,0,0,0,0,0,0,0,0,0,0,1,70,71,0,0,38,0,0,0,38,0,0,38,39,38,38,38,39,38,38,0,38,38,38,38,0,38,0,0,0,18,0,0,0,0,0,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,22,1,1,19,20,1,39,1,20,19,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,110,33,36,70,71,35,36,35,1,18,18,18,18,18,1,35,36,35,70,71,36,32,33,18,0,0,0,0,0,0,0,0,0,90,0,0,0,0,0,0,0,0,69,1,69,0,0,85,0,0,106,0,68,32,33,18,70,71,0,0,0,0,0,0,0,0,0,0,16,17,0,0,0,0,0,38,0,0,0,0,38,0,39,38,39,38,0,38,38,38,0,0,0,0,0,0,18,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,0,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,16,17,70,71,20,19,16,17,1,18,18,18,18,18,1,16,17,19,20,70,71,16,17,18,0,0,0,0,0,0,0,0,0,106,0,0,0,0,0,0,0,68,1,1,1,84,0,0,0,0,1,0,68,16,17,18,3,0,0,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,38,38,0,38,0,38,38,0,0,0,0,0,0,0,18,0,0,0,0,0,0,0,1,1,70,71,1,70,71,1,70,71,76,77,70,71,16,17,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,33,36,70,71,35,32,33,1,18,18,67,18,18,1,32,33,35,70,71,36,32,33,18,0,0,0,0,0,0,0,0,1,21,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,85,0,68,32,33,18,0,0,0,0,0,0,0,0,0,0,70,71,16,17,1,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,38,0,0,0,0,0,0,16,17,1,0,0,0,0,0,0,0,0,61,61,61,61,38,61,61,61,1,92,93,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,66,0,67,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,82,83,0,0,0,0,0,110,1,70,71,70,71,2,5,1,2,3,0,4,5,1,2,5,70,71,70,71,1,1,18,0,0,0,0,0,0,0,0,0,66,0,0,0,0,0,0,0,0,0,65,0,0,0,0,0,0,0,0,0,1,1,70,71,0,0,0,0,0,0,0,0,0,0,0,32,33,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,32,33,1,51,0,0,0,0,0,0,0,0,0,61,61,38,61,61,0,65,0,0,0,6,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,70,71,76,77,70,71,9,0,110,17,56,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,81,0,0,0,0,1,0,0,0,0,1,1,18,0,0,0,0,0,0,0,0,0,0,70,71,16,17,16,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,16,17,23,34,0,0,0,0,0,0,61,61,38,61,61,0,81,0,0,0,0,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,5,92,93,2,3,0,0,16,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,1,70,71,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,12,0,0,0,0,18,1,70,71,0,0,0,0,0,0,0,0,0,0,0,32,33,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,70,71,1,32,33,1,50,51,82,83,0,83,61,38,61,38,61,38,61,1,0,0,0,0,18,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,76,77,0,0,0,0,32,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,65,0,65,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,68,70,71,0,0,0,28,0,0,0,0,0,0,65,0,0,0,0,0,0,0,0,0,70,71,70,71,1,70,71,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,70,71,36,2,3,4,70,71,70,71,70,71,70,71,70,71,70,71,70,71,1,0,0,0,0,18,0,0,68,70,1,71,84,0,0,0,0,12,0,0,0,37,82,83,34,0,0,12,0,0,0,0,1,0,0,0,0,0,0,0,6,92,93,0,0,0,0,110,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,81,0,81,0,0,0,0,70,71,0,0,0,0,0,0,0,0,0,0,0,0,65,0,0,0,0,28,0,0,0,0,0,0,81,0,0,0,0,0,0,0,0,0,12,85,85,66,0,12,0,12,0,0,0,37,0,0,0,0,0,16,17,0,0,0,0,0,0,0,0,0,0,67,66,0,0,0,0,65,0,0,0,66,67,0,0,0,0,67,67,0,0,0,0,0,0,1,0,0,0,0,12,0,0,0,0,0,0,28,0,37,52,53,20,19,50,51,83,44,34,0,0,0,12,0,0,0,0,0,0,0,22,76,77,9,0,0,0,110,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56,18,1,18,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,81,0,0,0,0,44,0,0,0,0,18,1,18,0,0,0,0,0,0,0,0,0,28,0,0,0,0,28,0,44,82,83,52,53,1,0,0,1,1,32,33,38,0,16,17,0,0,0,0,82,0,0,0,0,0,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,1,1,0,0,0,44,0,34,0,0,0,37,44,52,53,36,35,36,35,36,35,36,1,50,51,34,0,28,0,0,0,0,0,0,0,0,92,93,25,0,0,0,16,33,16,17,73,16,17,1,1,1,1,1,1,1,1,1,16,17,73,16,17,1,1,18,1,69,69,69,69,1,1,69,69,69,69,69,69,69,69,69,69,69,69,1,1,1,69,69,70,71,69,69,69,1,1,18,1,1,16,17,69,16,17,69,0,44,16,17,23,0,44,1,1,1,1,1,1,7,69,69,0,1,1,39,38,38,32,33,69,69,1,1,20,1,1,50,51,52,1,1,1,23,83,0,0,82,83,0,0,0,0,0,82,0,0,83,0,24,16,17,1,16,17,16,17,50,51,0,52,53,1,20,19,20,19,20,19,20,19,20,19,20,19,50,51,44,0,45,0,76,77,69,69,52,76,77,69,69,1,69,32,17,32,33,89,32,33,70,71,38,38,38,38,38,70,71,32,33,89,32,33,1,1,1,1,70,71,70,71,1,70,71,73,73,73,70,71,70,71,70,71,70,71,70,71,70,71,70,71,1,73,73,73,1,1,1,1,1,32,33,1,32,33,1,1,1,32,33,1,1,1,1,1,1,1,1,70,71,16,17,70,71,16,17,70,71,16,17,70,71,16,17,70,71,16,17,70,71,16,17,70,71,16,17,70,71,70,71,70,71,16,17,70,71,16,17,70,71,32,33,1,32,33,32,33,1,1,1,1,1,1,1,1,36,35,1,1,36,35,36,1,1,1,36,1,1,1,1,1,92,93,1,1,1,92,93,1,1,1,1,32
        };

        obj.flags[4] = true; // curly awake

        room_width = 160;
        room_height = 16;

        large_contents.assign(room_contents, room_contents + SDL_arraysize(room_contents));

        setroomname("Sacred Grounds");
        obj.createentity(0, 0, 105, 2);

        obj.createblock(TRIGGER, 2448, -16, 32, 16, 0, "custom_door2_back_load");

        obj.createblock(TRIGGER, 2416, 32, 16, 96, 1, "custom_brutespawner1_load");
        obj.createblock(TRIGGER, 1648, 112, 16, 64, 2, "custom_brutespawner2_load");
        obj.createblock(TRIGGER, 1104, 32, 16, 80, 3, "custom_brutespawner3_load");
        obj.createblock(TRIGGER, 784, 16, 16, 208, 4, "custom_brutespawner4_load");
        obj.createblock(TRIGGER, 480, 16, 16, 176, 5, "custom_brutespawner5_load");
        obj.createblock(TRIGGER, 464, 16, 16, 176, 6, "custom_brutespawner6_load");

        obj.createblock(TRIGGER, 448, 16, 16, 176, 7, "custom_brutespawner7_load"); // ALSO HANDLES LORE

        obj.createblock(TRIGGER, 1776, 192, 16, 48, 10, "custom_removewall1_load");
        obj.createblock(TRIGGER, 1456, 192, 16, 32, 11, "custom_removewall2_load");

        obj.createblock(TRIGGER, 160, 144, 16, 80, 20, "custom_heavypress_load");

        obj.createblock(TRIGGER, 1568, 192, 16, 32, 32, "custom_lore5");

        obj.createblock(TRIGGER, 224, 144, 16, 80, 33, "custom_setcheckpoint_load");

        obj.createblock(TRIGGER, 128, 256, 80, 16, 34, "custom_endhell_load");

        // Checkpoint
        obj.createentity(2456, 112, 10, 1, 6970); // Checkpoint
        obj.createentity(1312, 224, 10, 1, 6971); // Checkpoint

        // Presses
        obj.createentity(2160, 80 + 8, 108);
        obj.createentity(2224, 64 + 8, 108);
        obj.createentity(2304, 64 + 8, 108);

        // Rollings
        obj.createentity(1600, 192, 109, 0);
        obj.createentity(1856, 224, 109, 2);

        // SPIKES
        // 1x1
        obj.createentity(1712, 160, 101, 1);  // Up
        obj.createentity(1744, 160, 101, 1);  // Up
        // 2x2
        obj.createentity(1040, 192, 100);

        // Mesas
        obj.createentity(1776 - 16, 64 - 8, 110, 1);
        obj.createentity(1696 - 16, 64 - 8, 110, 1);
        obj.createentity(1176, 168, 110, 1);
        obj.createentity(1152 - 16, 192 - 8, 110, 1);
        obj.createentity(992, 192, 110, 1);
        obj.createentity(864, 32 + 8, 110, 1);

        // Bow brutes
        obj.createentity(1936, 160, 112, 1);

        obj.createentity(1536, 128, 112, 1);
        obj.createentity(1552, 128, 112, 1);

        obj.createentity(1488, 96, 112, 0);
        obj.createentity(1472, 112, 112, 0);
        obj.createentity(1488, 144, 112, 0);
        obj.createentity(1456, 160, 112, 0);

        obj.createentity(1296, 176, 112, 1);
        obj.createentity(1264, 192, 112, 1);

        obj.createentity(1088, 64, 112, 1);
        obj.createentity(1104, 96, 112, 1);
        obj.createentity(1120, 128, 112, 1);
        obj.createentity(1136, 160, 112, 1);

        obj.createentity(1040, 96, 112, 0);
        obj.createentity(1040, 128, 112, 0);
        obj.createentity(1040, 160, 112, 0);

        obj.createentity(896, 64, 112, 1);
        obj.createentity(880, 112, 112, 1);
        obj.createentity(880, 144, 112, 1);

        obj.createentity(528, 96, 112, 1);
        obj.createentity(496, 96, 112, 1);

        // Sword brutes
        obj.createentity(2384, 208, 114, 1);
        obj.createentity(2272, 208, 114, 1);
        obj.createentity(2208, 192, 114, 1);
        obj.createentity(2096, 192, 114, 1);
        obj.createentity(2080, 208, 114, 1);
        obj.createentity(2016, 224, 114, 1);
        obj.createentity(1984, 208, 114, 1);
        obj.createentity(1952, 208, 114, 1);
        obj.createentity(1744, 80, 114, 1);
        obj.createentity(1728, 80, 114, 1);

        // Flying brutes (NOT spawners)
        obj.createentity(512, 160, 111, 0);
        obj.createentity(528, 144, 111, 0);
        obj.createentity(544, 128, 111, 0);
        obj.createentity(560, 48, 111, 0);
        obj.createentity(576, 64, 111, 0);
        obj.createentity(592, 80, 111, 0);

        // Trinkle
        obj.createentity(512, 16, 9, 0);

        // Heavy Press
        obj.createentity(128, 16 + 4, 115);
    }

    if (cavestorymode())
    {
        for (size_t i = 0; i < large_contents.size(); i++)
        {
            int x = (i % room_width) * 16;
            int y = (i / room_width) * 16;
            int tile = large_contents[i];

            switch (tile)
            {
            case 61:
                // BREAKABLE BLOCK
                break;
            case 68:
                // SPIKE - LEFT
                obj.createblock(DAMAGE, x + 8, y, 8, 16);
                break;
            case 69:
                // SPIKE - UP
                obj.createblock(DAMAGE, x, y + 8, 16, 8);
                break;
            case 84:
                // SPIKE - RIGHT
                obj.createblock(DAMAGE, x, y, 8, 16);
                break;
            case 85:
                // SPIKE - DOWN
                obj.createblock(DAMAGE, x, y, 16, 8);
                break;
            case 65:
                // DELEET - TOP BLOCK
                obj.createentity(x - 4, y + 4, 107);
                break;
            default:
                break;
            }
        }
    }
}

void mapclass::loadlevel(int rx, int ry)
{
    int t;
    if (!finalmode)
    {
        setexplored(rx - 100, ry - 100, true);
        if (rx == 109 && !custommode)
        {
            exploretower();
        }
    }


    roomtexton = false;
    roomtext.clear();
    roomnameset = false;

    obj.platformtile = 0;
    obj.customplatformtile=0;
    obj.vertplatforms = false;
    obj.horplatforms = false;
    setroomname("");
    hiddenname = "";
    background = 1;
    warpx = false;
    warpy = false;

    towermode = false;
    ypos = 0;
    oldypos = 0;
    extrarow = 0;
    spikeleveltop = 0;
    spikelevelbottom = 0;
    oldspikeleveltop = 0;
    oldspikelevelbottom = 0;

    //Custom stuff for warplines
    obj.customwarpmode=false;
    obj.customwarpmodevon=false;
    obj.customwarpmodehon=false;

    if (finalmode)
    {
        t = 6;
        //check if we're in the towers
        if (rx == 49 && ry == 52)
        {
            //entered tower 1
            t = 7;
        }
        else if (rx == 49 && ry == 53)
        {
            //re entered tower 1
            t = 8;
        }
        else if (rx == 51 && ry == 54)
        {
            //entered tower 2
            t = 9;
        }
        else if (rx == 51 && ry == 53)
        {
            //re entered tower 2
            t = 10;
        }
    }
    else if (custommode)
    {
        t= 12;
    }
    else
    {
        t = area(rx, ry);

        if (t == 3)
        {
            //correct position for tower
            if (ry == 109)
            {
                //entered from ground floor
                int player = obj.getplayer();
                if (INBOUNDS_VEC(player, obj.entities))
                {
                    obj.entities[player].yp += (671 * 8);
                }

                ypos = (700-29) * 8;
                oldypos = ypos;
                setbgobjlerp(graphics.towerbg);
                cameramode = 0;
                graphics.towerbg.colstate = 0;
                colsuperstate = 0;
            }
            else if (ry == 104)
            {
                //you've entered from the top floor
                ypos = 0;
                oldypos = ypos;
                setbgobjlerp(graphics.towerbg);
                cameramode = 0;
                graphics.towerbg.colstate = 0;
                colsuperstate = 0;
            }
        }

        if (t < 2) //on the world map, want to test if we're in the secret lab
        {
            if (rx >= 116)
            {
                if (ry >= 105)
                {
                    if (ry <= 107)
                    {
                        if (rx == 119 && ry == 105)
                        {
                            //Ah, this is just a normal area
                        }
                        else
                        {
                            //in the secret lab! Crazy background!
                            background = 2;
                            if (rx == 116 && ry == 105) graphics.rcol = 1;
                            if (rx == 117 && ry == 105) graphics.rcol = 5;
                            if (rx == 118 && ry == 105) graphics.rcol = 4;
                            if (rx == 117 && ry == 106) graphics.rcol = 2;
                            if (rx == 118 && ry == 106) graphics.rcol = 0;
                            if (rx == 119 && ry == 106) graphics.rcol = 3;
                            if (rx == 119 && ry == 107) graphics.rcol = 1;
                        }
                    }
                }
            }
        }
    }

    if (rx == 119 && ry == 108 && !custommode)
    {
        background = 5;
        graphics.rcol = 3;
        warpx = true;
        warpy = true;
    }

    roomname_special = false;

    switch(t)
    {
#ifdef MAKEANDPLAY
        UNUSED(copy_short_to_int);
#else
    case 0:
    case 1: //World Map
    {
        tileset = 1;
        extrarow = 1;
        const short* tmap = otherlevel.loadlevel(rx, ry);
        copy_short_to_int(contents, tmap, SDL_arraysize(contents));
        setroomname(otherlevel.roomname);
        roomname_special = otherlevel.roomname_special;
        hiddenname = otherlevel.hiddenname;
        tileset = otherlevel.roomtileset;
        break;
    }
    case 2: //The Lab
    {
        const short* tmap = lablevel.loadlevel(rx, ry);
        copy_short_to_int(contents, tmap, SDL_arraysize(contents));
        setroomname(lablevel.roomname);
        roomname_special = lablevel.roomname_special;
        tileset = 1;
        background = 2;
        graphics.rcol = lablevel.rcol;
        break;
    }
    case 3: //The Tower
        graphics.towerbg.tdrawback = true;
        minitowermode = false;
        tower.minitowermode = false;
        graphics.towerbg.scrolldir = 0;
        setbgobjlerp(graphics.towerbg);

        setroomname("The Tower");
        tileset = 1;
        background = 3;
        towermode = true;

        //All the entities for here are just loaded here; it's essentially one room after all


        obj.createentity(48, 5456, 10, 1, 505007); // (savepoint)
        obj.createentity(224, 4528, 10, 1, 505017); // (savepoint)
        obj.createentity(232, 4168, 10, 0, 505027); // (savepoint)
        obj.createentity(280, 3816, 10, 1, 505037); // (savepoint)
        obj.createentity(152, 3552, 10, 1, 505047); // (savepoint)
        obj.createentity(216, 3280, 10, 0, 505057); // (savepoint)
        obj.createentity(216, 4808, 10, 1, 505067); // (savepoint)
        obj.createentity(72, 3096, 10, 0, 505077); // (savepoint)
        obj.createentity(176, 2600, 10, 0, 505087); // (savepoint)
        obj.createentity(216, 2392, 10, 0, 505097); // (savepoint)
        obj.createentity(152, 1184, 10, 1, 505107); // (savepoint)
        obj.createentity(152, 912, 10, 1, 505117); // (savepoint)
        obj.createentity(152, 536, 10, 1, 505127); // (savepoint)
        obj.createentity(120, 5136, 10, 0, 505137); // (savepoint)
        obj.createentity(144, 1824, 10, 0, 505147); // (savepoint)
        obj.createentity(72, 2904, 10, 0, 505157); // (savepoint)
        obj.createentity(224, 1648, 10, 1, 505167); // (savepoint)
        obj.createentity(112, 5280, 10, 1, 50517); // (savepoint)

        obj.createentity(24, 4216, 9, 7); // (shiny trinket)
        obj.createentity(280, 3216, 9, 8); // (shiny trinket)
        break;
    case 4: //The Warpzone
    {
        const short* tmap = warplevel.loadlevel(rx, ry);
        copy_short_to_int(contents, tmap, SDL_arraysize(contents));
        setroomname(warplevel.roomname);
        roomname_special = warplevel.roomname_special;
        tileset = 1;
        background = 3;
        graphics.rcol = warplevel.rcol;
        graphics.backgrounddrawn = false;

        warpx = warplevel.warpx;
        warpy = warplevel.warpy;
        background = 5;
        if (warpy) background = 4;
        if (warpx) background = 3;
        if (warpx && warpy) background = 5;
        break;
    }
    case 5: //Space station
    {
        const short* tmap = spacestation2.loadlevel(rx, ry);
        copy_short_to_int(contents, tmap, SDL_arraysize(contents));
        setroomname(spacestation2.roomname);
        roomname_special = spacestation2.roomname_special;
        tileset = 0;
        break;
    }
    case 6: //final level
    {
        const short* tmap = finallevel.loadlevel(rx, ry);
        copy_short_to_int(contents, tmap, SDL_arraysize(contents));
        setroomname(finallevel.roomname);
        roomname_special = finallevel.roomname_special;
        tileset = 1;
        background = 3;
        graphics.backgrounddrawn = false;

        if (finalstretch)
        {
            background = 6;
        }
        else
        {
            warpx = finallevel.warpx;
            warpy = finallevel.warpy;
            background = 5;
            if (warpy) background = 4;
            if (warpx) background = 3;
            if (warpx && warpy) background = 5;
        }

        graphics.rcol = 6;
        changefinalcol(final_mapcol);
        break;
    }
    case 7: //Final Level, Tower 1
        graphics.towerbg.tdrawback = true;
        minitowermode = true;
        tower.minitowermode = true;
        graphics.towerbg.scrolldir = 1;
        setbgobjlerp(graphics.towerbg);

        setroomname("Panic Room");
        tileset = 1;
        background = 3;
        towermode = true;

        tower.loadminitower1();

        ypos = 0;
        oldypos = 0;
        setbgobjlerp(graphics.towerbg);
        cameramode = 0;
        graphics.towerbg.colstate = 0;
        colsuperstate = 0;
        break;
    case 8: //Final Level, Tower 1 (reentered from below)
    {
        graphics.towerbg.tdrawback = true;
        minitowermode = true;
        tower.minitowermode = true;
        graphics.towerbg.scrolldir = 1;
        setbgobjlerp(graphics.towerbg);

        setroomname("Panic Room");
        tileset = 1;
        background = 3;
        towermode = true;

        tower.loadminitower1();

        int i = obj.getplayer();
        if (INBOUNDS_VEC(i, obj.entities))
        {
            obj.entities[i].yp += (71 * 8);
        }
        game.roomy--;

        ypos = (100-29) * 8;
        oldypos = ypos;
        setbgobjlerp(graphics.towerbg);
        cameramode = 0;
        graphics.towerbg.colstate = 0;
        colsuperstate = 0;}
        break;
    case 9: //Final Level, Tower 2
    {
        graphics.towerbg.tdrawback = true;
        minitowermode = true;
        tower.minitowermode = true;
        graphics.towerbg.scrolldir = 0;
        setbgobjlerp(graphics.towerbg);

        setroomname("The Final Challenge");
        tileset = 1;
        background = 3;
        towermode = true;

        tower.loadminitower2();

        obj.createentity(56, 556, 11, 136); // (horizontal gravity line)
        obj.createentity(184, 592, 10, 0, 50500); // (savepoint)
        obj.createentity(184, 644, 11, 88); // (horizontal gravity line)
        obj.createentity(56, 460, 11, 136); // (horizontal gravity line)
        obj.createentity(216, 440, 10, 0, 50501); // (savepoint)
        obj.createentity(104, 508, 11, 168); // (horizontal gravity line)
        obj.createentity(219, 264, 12, 56); // (vertical gravity line)
        obj.createentity(120, 332, 11, 96); // (horizontal gravity line)
        obj.createentity(219, 344, 12, 56); // (vertical gravity line)
        obj.createentity(224, 332, 11, 48); // (horizontal gravity line)
        obj.createentity(56, 212, 11, 144); // (horizontal gravity line)
        obj.createentity(32, 20, 11, 96); // (horizontal gravity line)
        obj.createentity(72, 156, 11, 200); // (horizontal gravity line)

        int i = obj.getplayer();
        if (INBOUNDS_VEC(i, obj.entities))
        {
            obj.entities[i].yp += (71 * 8);
        }
        game.roomy--;

        ypos = (100-29) * 8;
        oldypos = ypos;
        setbgobjlerp(graphics.towerbg);
        cameramode = 0;
        graphics.towerbg.colstate = 0;
        colsuperstate = 0;
        break;
    }
    case 10: //Final Level, Tower 2
    {

        graphics.towerbg.tdrawback = true;
        minitowermode = true;
        tower.minitowermode = true;
        graphics.towerbg.scrolldir = 0;
        setbgobjlerp(graphics.towerbg);

        setroomname("The Final Challenge");
        tileset = 1;
        background = 3;
        towermode = true;

        tower.loadminitower2();

        obj.createentity(56, 556, 11, 136); // (horizontal gravity line)
        obj.createentity(184, 592, 10, 0, 50500); // (savepoint)
        obj.createentity(184, 644, 11, 88); // (horizontal gravity line)
        obj.createentity(56, 460, 11, 136); // (horizontal gravity line)
        obj.createentity(216, 440, 10, 0, 50501); // (savepoint)
        obj.createentity(104, 508, 11, 168); // (horizontal gravity line)
        obj.createentity(219, 264, 12, 56); // (vertical gravity line)
        obj.createentity(120, 332, 11, 96); // (horizontal gravity line)
        obj.createentity(219, 344, 12, 56); // (vertical gravity line)
        obj.createentity(224, 332, 11, 48); // (horizontal gravity line)
        obj.createentity(56, 212, 11, 144); // (horizontal gravity line)
        obj.createentity(32, 20, 11, 96); // (horizontal gravity line)
        obj.createentity(72, 156, 11, 200); // (horizontal gravity line)

        ypos = 0;
        oldypos = 0;
        setbgobjlerp(graphics.towerbg);
        cameramode = 0;
        graphics.towerbg.colstate = 0;
        colsuperstate = 0;
        break;
    }
    case 11: //Tower Hallways //Content is held in final level routine
    {
        const short* tmap = finallevel.loadlevel(rx, ry);
        copy_short_to_int(contents, tmap, SDL_arraysize(contents));
        setroomname(finallevel.roomname);
        roomname_special = finallevel.roomname_special;
        tileset = 2;
        if (rx == 108)
        {
            background = 7;
            rcol = 15;
        }
        if (rx == 110)
        {
            background = 8;
            rcol = 10;
        }
        if (rx == 111)
        {
            background = 9;
            rcol = 0;
        }
        break;
    }
#endif
    case 12: //Custom level
    {
        roomtexton = false;
        roomtext.clear();

        game.specialmode = SpecialMode_NONE;
        largermode = false;
        large_contents.clear();

        if (rx >= 101 && rx <= 103 && ry == 100)
        {
            if (!obj.flags[3])
            {
                obj.flags[3] = true;
            }
            largermode = true;
            game.specialmode = SpecialMode_CAVE;

            load_large_map(rx, ry);

            setplayer();

            /*room_width = 80;
            room_height = 60;
            large_contents.resize(room_width * room_height);
            for (int i = 0; i < room_width * room_height; i++)
            {
                large_contents[i] = 0;
            }*/
            return;
        }

        if (rx == 100 && ry == 101)
        {
            roomtexton = false;
            roomtext.clear();

            largermode = true;
            game.specialmode = SpecialMode_SMB;

            load_large_map(rx, ry);

            setplayer();
        }

        const RoomProperty* const room = cl.getroomprop(rx - 100, ry - 100);
        game.customcol = cl.getlevelcol(room->tileset, room->tilecol) + 1;
        obj.customplatformtile = game.customcol * 12;

        switch (room->tileset)
        {
        case 0: // Space Station
            tileset = 0;
            background = 1;
            break;
        case 1: // Outside
            tileset = 1;
            background = 1;
            break;
        case 2: // Lab
            tileset = 1;
            background = 2;
            graphics.rcol = room->tilecol;
            break;
        case 3: // Warp Zone/intermission
            tileset = 1;
            background = 6;
            break;
        case 4: // Ship
            tileset = 1;
            background = 1;
            break;
        default:
            tileset = 1;
            background = 1;
            break;
        }

        // If screen warping, then override all that:
        bool redrawbg = game.roomx != game.prevroomx || game.roomy != game.prevroomy;
        if (redrawbg)
        {
            graphics.backgrounddrawn = false;
        }

        switch (room->warpdir)
        {
        case 1:
            warpx = true;
            background = 3;
            graphics.rcol = cl.getwarpbackground(rx - 100, ry - 100);
            break;
        case 2:
            warpy = true;
            background = 4;
            graphics.rcol = cl.getwarpbackground(rx - 100, ry - 100);
            break;
        case 3:
            warpx = true;
            warpy = true;
            background = 5;
            graphics.rcol = cl.getwarpbackground(rx - 100, ry - 100);
            break;
        }

        setroomname(room->roomname.c_str());
        extrarow = 1;
        const int* tmap = cl.loadlevel(rx, ry);
        SDL_memcpy(contents, tmap, sizeof(contents));

        // Entities have to be created HERE, akwardly
        int tempcheckpoints = 0;
        int tempscriptbox = 0;
        for (size_t edi = 0; edi < customentities.size(); edi++)
        {
            // If entity is in this room, create it
            const CustomEntity& ent = customentities[edi];
            if (ent.rx != rx - 100 || ent.ry != ry - 100)
            {
                continue;
            }

            const int ex = ent.x * 8;
            const int ey = ent.y * 8;

            // Platform and enemy bounding boxes
            int bx1 = 0, by1 = 0, bx2 = 0, by2 = 0;

            bool enemy = ent.t == 1;
            bool moving_plat = ent.t == 2 && ent.p1 <= 4;
            if (enemy || moving_plat)
            {
                if (enemy)
                {
                    bx1 = room->enemyx1;
                    by1 = room->enemyy1;
                    bx2 = room->enemyx2;
                    by2 = room->enemyy2;
                }
                else if (moving_plat)
                {
                    bx1 = room->platx1;
                    by1 = room->platy1;
                    bx2 = room->platx2;
                    by2 = room->platy2;
                }

                // Enlarge bounding boxes to fix warping entities
                if (warpx && bx1 == 0 && bx2 == 320)
                {
                    bx1 -= 100;
                    bx2 += 100;
                }
                if (warpy && by1 == 0 && by2 == 240)
                {
                    by1 -= 100;
                    by2 += 100;
                }
            }

            switch (ent.t)
            {
            case 1: // Enemies
                obj.customenemy = room->enemytype;
                obj.createentity(ex, ey, 56, ent.p1, 4, bx1, by1, bx2, by2);
                break;
            case 2: // Platforms and conveyors
                if (ent.p1 <= 4)
                {
                    obj.createentity(ex, ey, 2, ent.p1, room->platv, bx1, by1, bx2, by2);
                }
                else if (ent.p1 >= 5 && ent.p1 <= 8) // Conveyor
                {
                    obj.createentity(ex, ey, 2, ent.p1 + 3, 4);
                }
                break;
            case 3: // Disappearing platforms
                obj.createentity(ex, ey, 3);
                break;
            case 9: // Trinkets
                obj.createentity(ex, ey, 9, cl.findtrinket(edi));
                break;
            case 10: // Checkpoints
                obj.createentity(ex, ey, 10, ent.p1, (rx + ry*100) * 20 + tempcheckpoints);
                tempcheckpoints++;
                break;
            case 11: // Gravity Lines
                if (ent.p1 == 0) //Horizontal
                {
                    obj.createentity(ent.p2 * 8, ey + 4, 11, ent.p3);
                }
                else //Vertical
                {
                    obj.createentity(ex + 3, ent.p2 * 8, 12, ent.p3);
                }
                break;
            case 13: // Warp Tokens
                obj.createentity(ex, ey, 13, ent.p1, ent.p2);
                break;
            case 15: // Collectable crewmate
                obj.createentity(ex - 4, ey + 1, 55, cl.findcrewmate(edi), ent.p1, ent.p2);
                break;
            case 17: // Roomtext!
            {
                roomtexton = true;
                Roomtext text;
                text.x = ex / 8;
                text.y = ey / 8;
                text.text = ent.scriptname.c_str();
                text.rtl = ent.p1;
                roomtext.push_back(text);
                break;
            }
            case 18: // Terminals
            {
                obj.customscript = ent.scriptname;

                int usethistile = ent.p1;
                int usethisy = ey;

                // This isn't a boolean: we just swap 0 and 1 around and leave the rest alone
                if (usethistile == 0)
                {
                    usethistile = 1; // Unflipped
                }
                else if (usethistile == 1)
                {
                    usethistile = 0; // Flipped;
                    usethisy -= 8;
                }

                obj.createentity(ex, usethisy + 8, 20 + SDL_clamp(ent.p2, 0, 1), usethistile);

                for (size_t i = 0; i < script.customscripts.size(); i++)
                {
                    if (script.customscripts[i].name == obj.customscript)
                    {
                        obj.createblock(ACTIVITY, ex - 8, usethisy + 8, 20, 16, 35);
                        break;
                    }
                }
                break;
            }
            case 19: //Script Box
                if (INBOUNDS_ARR(tempscriptbox, game.customscript))
                {
                    game.customscript[tempscriptbox] = ent.scriptname;
                }
                obj.createblock(TRIGGER, ex, ey, ent.p1 * 8, ent.p2 * 8, 300 + tempscriptbox, "custom_" + ent.scriptname);
                tempscriptbox++;
                break;
            case 50: // Warp Lines
                obj.customwarpmode=true;
                switch (ent.p1)
                {
                case 0: // Vertical, left
                    obj.createentity(ex + 4, ent.p2 * 8, 51, ent.p3);
                    break;
                case 1: //Horizontal, right
                    obj.createentity(ex + 4, ent.p2 * 8, 52, ent.p3);
                    break;
                case 2: //Vertical, top
                    obj.createentity(ent.p2 * 8, ey + 7, 53, ent.p3);
                    break;
                case 3: // Horizontal, bottom
                    obj.createentity(ent.p2 * 8, ey, 54, ent.p3);
                    break;
                }
                break;
            }
        }

        //do the appear/remove roomname here
        break;
    }
    }
    //The room's loaded: now we fill out damage blocks based on the tiles.
    if (largermode)
    {

    }
    else if (towermode)
    {

    }
    else
    {
        for (int j = 0; j < 29 + extrarow; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                int tile = contents[TILE_IDX(i, j)];
                //Damage blocks
                if(tileset==0)
                {
                    if (tile == 6 || tile == 8)
                    {
                        //sticking up
                        obj.createblock(2, (i * 8), (j * 8)+4, 8, 4);
                    }
                    if (tile == 7 || tile == 9)
                    {
                        //Sticking down
                        obj.createblock(2, (i * 8), (j * 8), 8, 4);
                    }
                    if (tile == 49 || tile == 50)
                    {
                        //left or right
                        obj.createblock(2, (i * 8), (j * 8)+3, 8, 2);
                    }
                }
                else if(tileset==1)
                {
                    if ((tile >= 63 && tile <= 74) ||
                            (tile >= 6 && tile <= 9))
                    {
                        //sticking up) {
                        if (tile < 10) tile++;
                        //sticking up
                        if(tile%2==0)
                        {
                            obj.createblock(2, (i * 8), (j * 8), 8, 4);
                        }
                        else
                        {
                            //Sticking down
                            obj.createblock(2, (i * 8), (j * 8) + 4, 8, 4);
                        }
                        if (tile < 11) tile--;
                    }
                    if (tile >= 49 && tile <= 62)
                    {
                        //left or right
                        obj.createblock(2, (i * 8), (j * 8)+3, 8, 2);
                    }
                }
                else if(tileset==2)
                {
                    if (tile == 6 || tile == 8)
                    {
                        //sticking up
                        obj.createblock(2, (i * 8), (j * 8)+4, 8, 4);
                    }
                    if (tile == 7 || tile == 9)
                    {
                        //Sticking down
                        obj.createblock(2, (i * 8), (j * 8), 8, 4);
                    }
                }
                //Breakable blocks
                if (tile == 10)
                {
                    settile(i, j, 0);
                    obj.createentity(i * 8, j * 8, 4);
                }
                //Directional blocks
                if (tile >= 14 && tile <= 17)
                {
                    obj.createblock(3, i * 8, j * 8, 8, 8, tile-14);
                }
            }
        }

        for (size_t i = 0; i < obj.entities.size(); i++)
        {
            if (obj.entities[i].type == 1 && obj.entities[i].behave >= 8 && obj.entities[i].behave < 10)
            {
                //put a block underneath
                int temp = obj.entities[i].xp / 8.0f;
                int temp2 = obj.entities[i].yp / 8.0f;
                settile(temp, temp2, 1);
                settile(temp+1, temp2, 1);
                settile(temp+2, temp2, 1);
                settile(temp+3, temp2, 1);
                if (obj.entities[i].w == 64)
                {
                    settile(temp+4, temp2, 1);
                    settile(temp+5, temp2, 1);
                    settile(temp+6, temp2, 1);
                    settile(temp+7, temp2, 1);
                }
            }
        }
    }

    //Special scripting: Create objects and triggers based on what crewmembers are rescued.
    if (!finalmode && !custommode)
    {
        //First up: the extra bits:
        //Vermilion's quest:
        if (rx == 100 && ry == 105)     //On path to verdigris
        {
            if (game.crewstats[3] && !game.crewstats[4])
            {
                obj.createentity(87, 105, 18, 15, 0, 18);
                obj.createblock(5, 87-32, 0, 32+32+32, 240, 3);
            }
        }
        else if (rx == 107 && ry == 100)    //victoria
        {
            if (game.crewstats[3] && !game.crewstats[5])
            {
                obj.createentity(140, 137, 18, 15, 0, 18);
                obj.createblock(5, 140-32, 0, 32+32+32, 240, 3);
            }
        }
        else if (rx == 114 && ry == 109)
        {
            if (game.crewstats[3] && !game.crewstats[2])
            {
                obj.createentity(235, 81, 18, 15, 0, 18);
                obj.createblock(5, 235-32, 0, 32+32+32, 240, 3);
            }
        }

        //Verdigris fixing the ship
        if (rx == 101 && ry == 109)
        {
            if (game.crewstats[4])
            {
                if(game.crewrescued()>4 && game.crewrescued()!=6)
                {
                    obj.createentity(175, 121, 18, 13, 0, 18);
                    obj.createblock(5, 175-32, 0, 32+32+32, 240, 4);
                }
            }
        }
        else if (rx == 103 && ry == 109)
        {
            if (game.crewstats[4])
            {
                if(game.crewrescued()<=4 && game.crewrescued()!=6)
                {
                    obj.createentity(53, 161, 18, 13, 1, 18);
                    obj.createblock(5, 53-32, 0, 32+32+32, 240, 4);
                }
            }
        }

        if (rx == 104 && ry == 111)
        {
            //Red
            //First: is he rescued?
            if (game.crewstats[3])
            {
                //If so, red will always be at his post
                obj.createentity(107, 121, 18, 15, 0, 18);
                //What script do we use?
                obj.createblock(5, 107-32, 0, 32+32+32, 240, 3);
            }
        }
        else if (rx == 103 && ry == 111)
        {
            //Yellow
            //First: is he rescued?
            if (game.crewstats[2])
            {
                obj.createentity(198, 105, 18, 14, 0, 18);
                //What script do we use?
                obj.createblock(5, 198-32, 0, 32+32+32, 240, 2);
            }
        }
        else if (rx == 103 && ry == 110)
        {
            //Green
            //First: is he rescued?
            if (game.crewstats[4])
            {
                obj.createentity(242, 177, 18, 13, 0, 18);
                //What script do we use?
                obj.createblock(5, 242-32, 177-20, 32+32+32, 40, 4);
            }
        }
        else if (rx == 104 && ry == 110)
        {
            //Purple
            //First: is she rescued?
            if (game.crewstats[1])
            {
                obj.createentity(140, 177, 18, 20, 0, 18);
                //What script do we use?
                obj.createblock(5, 140-32, 0, 32+32+32, 240, 1);
            }
        }
        else if (rx == 102 && ry == 110)
        {
            //Blue
            //First: is she rescued?
            if (game.crewstats[5])
            {
                //A slight varation - she's upside down
                obj.createentity(249, 62, 18, 16, 0, 18);
                int j = obj.getcrewman(BLUE);
                if (INBOUNDS_VEC(j, obj.entities))
                {
                    obj.entities[j].rule = 7;
                    obj.entities[j].tile +=6;
                }
                //What script do we use?
                obj.createblock(5, 249-32, 0, 32+32+32, 240, 5);
            }
        }
    }
}

void mapclass::twoframedelayfix(void)
{
    // Fixes the two-frame delay in custom levels that use scripts to spawn an entity upon room load.
    // Because when the room loads and newscript is set to run, newscript has already ran for that frame,
    // and when the script gets loaded script.run() has already ran for that frame, too.
    // A bit kludge-y, but it's the least we can do without changing the frame ordering.

    if (GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2)
    || !custommode
    || game.deathseq != -1)
        return;

    int block_idx = -1;
    // obj.checktrigger() sets block_idx
    int activetrigger = obj.checktrigger(&block_idx);
    if (activetrigger <= -1
    || !INBOUNDS_VEC(block_idx, obj.blocks)
    || activetrigger < 300)
    {
        return;
    }

    game.newscript = obj.blocks[block_idx].script;
    obj.removetrigger(activetrigger);
    game.setstate(0);
    game.setstatedelay(0);
    script.load(game.newscript);
}
