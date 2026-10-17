#ifndef TOUCH_H
#define TOUCH_H

#include <SDL3/SDL.h>

#define VVV_FingerID Uint64

struct VVV_Finger
{
    float x;
    float y;

    // Rather than collapsing this into one state,
    // it's best if these are tracked separately.

    // If the user presses a button in less than a frame,
    // we can track that it was both pressed and released,
    // and then respond to just_pressed despite it no longer being down,
    // which lets us properly track quick taps without weird kludge.

    bool just_pressed;
    bool just_released;
    bool down;
};

#endif /* TOUCH_H */
