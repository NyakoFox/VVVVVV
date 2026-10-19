#ifndef KEYPOLL_H
#define KEYPOLL_H

#include <map> // FIXME: I should feel very bad for using C++ -flibit
#include <SDL3/SDL.h>
#include <string>
#include <vector>

#include "Touch.h"

enum Kybrd
{
    KEYBOARD_UP = SDLK_UP,
    KEYBOARD_DOWN = SDLK_DOWN,
    KEYBOARD_LEFT = SDLK_LEFT,
    KEYBOARD_RIGHT = SDLK_RIGHT,
    KEYBOARD_ENTER = SDLK_RETURN,
    KEYBOARD_SPACE = SDLK_SPACE,

    KEYBOARD_w = SDLK_W,
    KEYBOARD_s = SDLK_S,
    KEYBOARD_a = SDLK_A,
    KEYBOARD_d = SDLK_D,
    KEYBOARD_e = SDLK_E,
    KEYBOARD_m = SDLK_M,
    KEYBOARD_n = SDLK_N,

    KEYBOARD_v = SDLK_V,
    KEYBOARD_z = SDLK_Z,

    KEYBOARD_BACKSPACE = SDLK_BACKSPACE
};

class KeyPoll
{
public:
    std::map<SDL_Keycode, bool> keymap;

    bool isActive;

    bool resetWindow;

    void toggleFullscreen(void);

    int sensitivity;

    int inline getThreshold(void);

    KeyPoll(void);

    void enabletextentry(void);

    void disabletextentry(void);

    void Poll(void);

    bool isDown(SDL_Keycode key);

    bool isDown(std::vector<SDL_GamepadButton> buttons);
    bool isDown(SDL_GamepadButton button);
    bool controllerButtonDown(void);
    bool controllerWantsLeft(bool includeVert);
    bool controllerWantsRight(bool includeVert);
    bool controllerWantsUp(void);
    bool controllerWantsDown(void);

    bool touchPressed(void);
    bool touchWantsLeft(void);
    bool touchWantsRight(void);
    bool touchWantsFlip(void);

    void resetSwipe(void);
    void updateSwipe(void);

	VVV_Finger* getFinger(SDL_FingerID finger_id);

    bool isFingerInRect(SDL_FingerID finger_id, const SDL_FRect& rect, bool scaled);
    SDL_FingerID capturePressedFingerAtRect(const SDL_FRect& rect, bool scaled);

    int leftbutton, rightbutton, middlebutton;
    int mousex;
    int mousey;

    bool textentry(void);
    bool pressedbackspace;
    bool pressed_android_back;
    std::string keybuffer;
    std::string imebuffer;
    int imebuffer_start;
    int imebuffer_length;

    bool linealreadyemptykludge;
    bool using_touch;

private:
    std::map<SDL_JoystickID, SDL_Gamepad*> controllers;
    std::map<SDL_GamepadButton, bool> buttonmap;
    int xVel, yVel;

    std::map<SDL_FingerID, VVV_Finger> fingers;
    Uint32 wasFullscreen;

    SDL_FingerID swipe_finger;
    float swipe_previous;
    SwipeDirection swipe_dir;
};

#ifndef KEY_DEFINITION
extern KeyPoll key;
#endif

#endif /* KEYPOLL_H */
