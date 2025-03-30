#ifndef BLOCKV_H
#define BLOCKV_H

#include <SDL.h>
#include <stdint.h>
#include <string>
#include <vector>

#define SPRINGS_PER_PIXEL 1
#define SPRING_ITERATIONS 3

#define SPRING_SPREAD 0.05f
#define SPRING_DAMPING 0.95f
#define SPRING_STIFFNESS 0.02f

struct Spring {
    float x;
    float y;
    float speed;
};

class blockclass
{
public:
    blockclass(void);
    void clear(void);

    void rectset(const int xi, const int yi, const int wi, const int hi);

    void process_springs(std::vector<Spring>* springs, int offset);

    void setblockcolour(const char* col);
    void update(void);
public:
    //Fundamentals
    SDL_Rect rect;
    int type;
    int trigger;
    int xp, yp, wp, hp;
    std::string script, prompt;
    int r, g, b;
    int activity_y;
    bool gettext;
    std::vector<Spring> springs;
    std::vector<Spring> bottom_springs;
};

#endif /* BLOCKV_H */
