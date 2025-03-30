#include "BlockV.h"

#include <SDL_stdinc.h>
#include <vector>

#include "Editor.h"
#include "Entity.h"
#include "Font.h"
#include "KeyPoll.h"
#include "Script.h"

blockclass::blockclass(void)
{
    clear();
}

void blockclass::clear(void)
{
    type = 0;
    trigger = 0;

    xp = 0;
    yp = 0;
    wp = 0;
    hp = 0;
    rect.x = xp;
    rect.y = yp;
    rect.w = wp;
    rect.h = hp;

    r = 0;
    g = 0;
    b = 0;

    activity_y = 0;

    /* std::strings get initialized automatically, but this is
     * in case this function gets called again after construction */
    script.clear();
    prompt.clear();

    gettext = true;

    springs.clear();
    bottom_springs.clear();
}

void blockclass::rectset(const int xi, const int yi, const int wi, const int hi)
{
    rect.x = xi;
    rect.y = yi;
    rect.w = wi;
    rect.h = hi;

    if (type == WATER)
    {
        for (int i = 0; i < (wi * SPRINGS_PER_PIXEL); i++)
        {
            Spring newspring;
            newspring.x = ((float)i / (wi * SPRINGS_PER_PIXEL)) * (float)wi;
            newspring.y = 0;
            newspring.speed = 0;
            springs.push_back(newspring);

            Spring newspring2;
            newspring2.x = ((float)i / (wi * SPRINGS_PER_PIXEL)) * (float)wi;
            newspring2.y = hi;
            newspring2.speed = 0;
            bottom_springs.push_back(newspring2);
        }
        // add a spring to the end as well
        Spring newspring;
        newspring.x = wi;
        newspring.y = 0;
        newspring.speed = 0;
        springs.push_back(newspring);

        // same for bottom
        Spring newspring2;
        newspring2.x = wi;
        newspring2.y = hi;
        newspring2.speed = 0;
        bottom_springs.push_back(newspring2);
    }
}

void blockclass::process_springs(std::vector<Spring>* springs, int offset)
{
    for (size_t j = 0; j < springs->size(); j++)
    {
        float force = 0;

        // Baseline force, so that the spring wants to return to its original position

        float force_baseline = SPRING_STIFFNESS * (offset - (*springs)[j].y);

        float force_left = 0;
        float force_right = 0;

        if (j > 0)
        {
            // Left
            force_left = SPRING_SPREAD * ((*springs)[j - 1].y - (*springs)[j].y);
        }

        if (j < springs->size() - 1)
        {
            // Right
            force_right = SPRING_SPREAD * ((*springs)[j + 1].y - (*springs)[j].y);
        }

        force = force_baseline + force_left + force_right;

        // if each spring had mass we'd do (force / mass) but we don't

        (*springs)[j].speed += force;

        (*springs)[j].speed *= SPRING_DAMPING;

        (*springs)[j].y += (*springs)[j].speed;
    }
}

void blockclass::update(void)
{
    if (type == WATER)
    {
        for (int i = 0; i < SPRING_ITERATIONS; i++)
        {
            process_springs(&springs, 0);
            process_springs(&bottom_springs, rect.h);
        }
    }
}

void blockclass::setblockcolour(const char* col)
{
    bool exists = ::script.textbox_colours.count(col) != 0;

    r = ::script.textbox_colours[exists ? col : "gray"].r;
    g = ::script.textbox_colours[exists ? col : "gray"].g;
    b = ::script.textbox_colours[exists ? col : "gray"].b;
}
