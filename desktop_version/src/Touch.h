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
    TouchControlType_MENU_BUTTON, // A text button in the menu system
    TouchControlType_MENU_BUTTON_ICON, // A button in the menu system, which displays an icon instead of text
    TouchControlType_MENU_BUTTON_LANGUAGE, // A specific language option in the language selection menu
    TouchControlType_MENU_BUTTON_TELESAVE, // The telesave summary button in the continue menu
    TouchControlType_MENU_BUTTON_QUICKSAVE, // The quicksave summary button in the continue menu
    TouchControlType_MENU_BUTTON_OPTION, // A text button in a menu, which solely changes the current option
    TouchControlType_MENU_BUTTON_OPTION_ICON, // An icon button in a menu, which solely changes the current option
    TouchControlType_MENU_TOGGLE, // A checkbox toggle in the menu system
    TouchControlType_SLIDER // A slider control
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
    Uint8 text_flags;
    int option_id;
    SDL_Texture* image;

    // Menu toggles
    bool checked;

    // Sliders
    int min_value;
    int max_value;
    int* slider_var;
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
    TouchControl* create_menu_toggle(int x, int y, int width, int height, const std::string& text, int option_id, bool checked, bool active);
    TouchControl* create_menu_toggle(int x, int y, int width, int height, const std::string& text, int option_id, bool checked);

    TouchControl* create_slider(int x, int y, int width, int height, const std::string& text, int* var, int min_value, int max_value, bool active);
    TouchControl* create_slider(int x, int y, int width, int height, const std::string& text, int* var, int min_value, int max_value);
}

#endif /* TOUCH_H */
