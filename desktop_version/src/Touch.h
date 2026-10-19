#ifndef TOUCH_H
#define TOUCH_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>

#define VVV_FingerID Uint64

#define TOUCH_SWIPE_SENSITIVITY 0.005

struct VVV_Finger
{
    float x;
    float y;
    float x_normalized;

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

enum SwipeDirection
{
    SwipeDirection_LEFT,
    SwipeDirection_NONE,
    SwipeDirection_RIGHT
};

enum TouchControlType
{
    TouchControlType_MENU_BUTTON,
    TouchControlType_MENU_BUTTON_ICON
};

struct TouchControl
{
    TouchControlType type;
    SDL_FRect rect;
    bool down;
    VVV_FingerID pressed_finger;
    bool active;

    // Menu buttons
    std::string text;
    int option_id;
    SDL_Texture* image;
};

namespace touch
{
    extern std::vector<TouchControl> dynamic_controls;

    void init(void);

    void input(void);

    void render_dynamic_controls(int r, int g, int b);

    void clear_dynamic_controls(void);

    TouchControl* create_control(TouchControlType type, int x, int y, int width, int height);
    TouchControl* create_menu_button(int x, int y, int width, int height, const std::string& text, int option_id, bool active);
    TouchControl* create_menu_button(int x, int y, int width, int height, const std::string& text, int option_id);
    TouchControl* create_menu_button(int x, int y, int width, int height, SDL_Texture* icon, int option_id);
}

#endif /* TOUCH_H */
