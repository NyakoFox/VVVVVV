#define KEY_DEFINITION
#include "KeyPoll.h"

#include <string.h>

#include "Alloc.h"
#include "ButtonGlyphs.h"
#include "Constants.h"
#include "Editor.h"
#include "Exit.h"
#include "Game.h"
#include "GlitchrunnerMode.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "Localization.h"
#include "LocalizationMaint.h"
#include "LocalizationStorage.h"
#include "Music.h"
#include "Screen.h"
#include "Touch.h"
#include "UTF8.h"
#include "UtilityClass.h"
#include "Vlogging.h"

bool SaveScreenshot(void);

int inline KeyPoll::getThreshold(void)
{
    switch (sensitivity)
    {
    case 0:
        return 28000;
    case 1:
        return 16000;
    case 2:
        return 8000;
    case 3:
        return 4000;
    case 4:
        return 2000;
    }

    return 8000;

}

KeyPoll::KeyPoll(void)
{
    xVel = 0;
    yVel = 0;
    // 0..5
    sensitivity = 2;

    keybuffer = "";
    imebuffer = "";
    imebuffer_start = 0;
    imebuffer_length = 0;
    leftbutton=0; rightbutton=0; middlebutton=0;
    mousex = 0;
    mousey = 0;
    resetWindow = 0;
    pressedbackspace = false;
    pressed_android_back = false;

    linealreadyemptykludge = false;

    isActive = true;

    was_using_touch = false;
    using_touch = false;

    next_finger_id = 1; // 0 is invalid
    fingers.clear();
    sdl_finger_to_vvv_finger.clear();

    swipe_finger = 0;
    swipe_previous = 0;
    swipe_dir = SwipeDirection_NONE;
}

void KeyPoll::enabletextentry(void)
{
    keybuffer = "";
    imebuffer = "";
    imebuffer_start = 0;
    imebuffer_length = 0;
    SDL_StartTextInput(gameScreen.m_window);
}

void KeyPoll::disabletextentry(void)
{
    SDL_StopTextInput(gameScreen.m_window);
    imebuffer = "";
    imebuffer_start = 0;
    imebuffer_length = 0;
}

bool KeyPoll::textentry(void)
{
    return SDL_TextInputActive(gameScreen.m_window) == true;
}

void KeyPoll::toggleFullscreen(void)
{
    gameScreen.toggleFullScreen();

    keymap.clear(); /* we lost the input due to a new window. */
    if (GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2))
    {
        game.press_left = false;
        game.press_right = false;
        game.press_action = true;
        game.press_map = false;
    }
}

static int changemousestate(
    int timeout,
    const bool show,
    const bool hide
) {
    bool visible;

    if (timeout > 0)
    {
        return --timeout;
    }

    /* If we want to both show and hide at the same time, prioritize showing */
    if (show)
    {
        visible = true;
    }
    else if (hide)
    {
        visible = false;
    }
    else
    {
        return timeout;
    }

    if (SDL_CursorVisible() == visible)
    {
        return timeout;
    }

    if (visible) {
        SDL_ShowCursor();
        timeout = 0;
    } else {
        SDL_HideCursor();
        timeout = 30;
    }

    return timeout;
}

/* Also used in Input.cpp. */
void recomputetextboxes(void);

bool cycle_language(bool should_recompute_textboxes)
{
    extern KeyPoll key;

    if (game.gamestate == TITLEMODE
    && game.currentmenuname == Menu::translator_options_cutscenetest)
    {
        /* Unfortunately, despite how it may appear to be working, the options
         * are actually language-specific, and the order could be totally
         * different between languages too. So we can't cycle in this menu. */
        music.playef(Sound_CRY);
        return should_recompute_textboxes;
    }
    if (game.translator_cutscene_test)
    {
        /* Refuse cycling here for similar reasons, even if it seems like it's
         * working. The text boxes are based off of the language XML and
         * could be completely different between languages. */
        music.playef(Sound_CRY);
        return should_recompute_textboxes;
    }

    int i = loc::languagelist_curlang;
    if (key.keymap[SDLK_LSHIFT])
    {
        /* Backwards */
        i--;
    }
    else
    {
        /* Forwards */
        i++;
    }
    if (!loc::languagelist.empty())
    {
        i = POS_MOD(i, (int) loc::languagelist.size());

        loc::languagelist_curlang = i;
        loc::lang = loc::languagelist[i].code;
        loc::loadtext(false);
        graphics.grphx.init_translations();

        should_recompute_textboxes = true;
    }

    if (game.gamestate == TITLEMODE
    || (game.gamestate == EDITORMODE && ed.state == EditorState_MENU))
    {
        if (game.currentmenuname == Menu::translator_options_limitscheck)
        {
            loc::local_limits_check();
        }

        int temp = game.menucountdown;
        game.createmenu(game.currentmenuname, true);
        game.menucountdown = temp;

        if (game.currentmenuname == Menu::language)
        {
            game.currentmenuoption = i;
        }
    }

    return should_recompute_textboxes;
}

void KeyPoll::Poll(void)
{
    static int raw_mousex = 0;
    static int raw_mousey = 0;
    static int mousetoggletimeout = 0;
    bool showmouse = false;
    bool hidemouse = false;
    bool altpressed = false;
    bool fullscreenkeybind = false;
    SDL_Gamepad *controller = NULL;
    SDL_Event evt;
    bool should_recompute_textboxes = false;
    bool active_input_device_changed = false;
    bool keyboard_was_active = BUTTONGLYPHS_keyboard_is_active();

    int screen_width;
    int screen_height;
    gameScreen.GetScreenSize(&screen_width, &screen_height);

    pressed_android_back = false;

    // Update our finger states
    std::map<VVV_FingerID, VVV_Finger>::iterator it;
    for (it = fingers.begin(); it != fingers.end(); /* nothing */)
    {
        if (!it->second.down)
        {
            fingers.erase(it++);
        }
        else
        {
            if (it->second.just_pressed)
            {
                it->second.just_pressed = false;
            }

            ++it;
        }
    }

    while (SDL_PollEvent(&evt))
    {
        switch (evt.type)
        {
        /* Keyboard Input */
        case SDL_EVENT_KEY_DOWN:
        {
            keymap[evt.key.key] = true;

            if (evt.key.scancode == SDL_SCANCODE_AC_BACK)
            {
                pressed_android_back = true;
                return;
            }

            if (evt.key.key == SDLK_BACKSPACE)
            {
                pressedbackspace = true;
            }

#ifdef SDL_PLATFORM_APPLE /* OSX prefers the command keys over the alt keys. -flibit */
            altpressed = keymap[SDLK_LGUI] || keymap[SDLK_RGUI];
#else
            altpressed = keymap[SDLK_LALT] || keymap[SDLK_RALT];
#endif
            bool returnpressed = evt.key.key == SDLK_RETURN;
            bool fpressed = evt.key.key == SDLK_F;
            bool f11pressed = evt.key.key == SDLK_F11;
            if ((altpressed && (returnpressed || fpressed)) || f11pressed)
            {
                fullscreenkeybind = true;
            }

            if (loc::show_translator_menu && evt.key.key == SDLK_F8 && !evt.key.repeat)
            {
                if (keymap[SDLK_LCTRL])
                {
                    /* Debug keybind to cycle language. */
                    should_recompute_textboxes = cycle_language(should_recompute_textboxes);
                }
                else
                {
                    /* Reload language files */
                    loc::loadtext(false);
                    graphics.grphx.init_translations();
                    music.playef(Sound_COIN);
                }
            }

            if (evt.key.key == SDLK_F6 && !evt.key.repeat)
            {
                const bool success = SaveScreenshot();
                game.old_screenshot_border_timer = 255;
                game.screenshot_border_timer = 255;
                game.screenshot_saved_success = success;
            }

            BUTTONGLYPHS_keyboard_set_active(true);

            if (textentry())
            {
                if (evt.key.key == SDLK_BACKSPACE && !keybuffer.empty())
                {
                    keybuffer.erase(UTF8_backspace(keybuffer.c_str(), keybuffer.length()));
                    if (keybuffer.empty())
                    {
                        linealreadyemptykludge = true;
                    }
                }
                else if (    evt.key.key == SDLK_V &&
                        keymap[SDLK_LCTRL]    )
                {
                    char* text = SDL_GetClipboardText();
                    if (text != NULL)
                    {
                        keybuffer += text;
                        VVV_free(text);
                    }
                }
                else if (    evt.key.key == SDLK_X &&
                        keymap[SDLK_LCTRL]    )
                {
                    if (SDL_SetClipboardText(keybuffer.c_str()))
                    {
                        keybuffer = "";
                    }
                }
            }
            break;
        }
        case SDL_EVENT_KEY_UP:
            keymap[evt.key.key] = false;
            if (evt.key.key == SDLK_BACKSPACE)
            {
                pressedbackspace = false;
            }
            break;
        case SDL_EVENT_TEXT_INPUT:
            if (!altpressed)
            {
                keybuffer += evt.text.text;
            }
            break;
        case SDL_EVENT_TEXT_EDITING:
            imebuffer = evt.edit.text;
            imebuffer_start = evt.edit.start;
            imebuffer_length = evt.edit.length;
            break;

        /* Mouse Input */
        case SDL_EVENT_MOUSE_MOTION:
            raw_mousex = evt.motion.x;
            raw_mousey = evt.motion.y;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            switch (evt.button.button)
            {
            case SDL_BUTTON_LEFT:
                raw_mousex = evt.button.x;
                raw_mousey = evt.button.y;
                leftbutton = 1;
                break;
            case SDL_BUTTON_RIGHT:
                raw_mousex = evt.button.x;
                raw_mousey = evt.button.y;
                rightbutton = 1;
                break;
            case SDL_BUTTON_MIDDLE:
                raw_mousex = evt.button.x;
                raw_mousey = evt.button.y;
                middlebutton = 1;
                break;
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            switch (evt.button.button)
            {
            case SDL_BUTTON_LEFT:
                raw_mousex = evt.button.x;
                raw_mousey = evt.button.y;
                leftbutton=0;
                break;
            case SDL_BUTTON_RIGHT:
                raw_mousex = evt.button.x;
                raw_mousey = evt.button.y;
                rightbutton=0;
                break;
            case SDL_BUTTON_MIDDLE:
                raw_mousex = evt.button.x;
                raw_mousey = evt.button.y;
                middlebutton=0;
                break;
            }
            break;

        /* Controller Input */
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            buttonmap[(SDL_GamepadButton) evt.gbutton.button] = true;
            BUTTONGLYPHS_keyboard_set_active(false);

            controller = controllers[evt.gbutton.which];
            BUTTONGLYPHS_update_layout(controller);
            break;
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            buttonmap[(SDL_GamepadButton) evt.gbutton.button] = false;
            break;
        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        {
            const int threshold = getThreshold();
            switch (evt.gaxis.axis)
            {
            case SDL_GAMEPAD_AXIS_LEFTX:
                if (    evt.gaxis.value > -threshold &&
                    evt.gaxis.value < threshold    )
                {
                    xVel = 0;
                }
                else
                {
                    xVel = (evt.gaxis.value > 0) ? 1 : -1;
                }
                break;
            case SDL_GAMEPAD_AXIS_LEFTY:
                if (    evt.gaxis.value > -threshold &&
                    evt.gaxis.value < threshold    )
                {
                    yVel = 0;
                }
                else
                {
                    yVel = (evt.gaxis.value > 0) ? 1 : -1;
                }
                break;
            }
            BUTTONGLYPHS_keyboard_set_active(false);

            controller = controllers[evt.gaxis.which];
            BUTTONGLYPHS_update_layout(controller);
            break;
        }
        case SDL_EVENT_GAMEPAD_ADDED:
        {
            controller = SDL_OpenGamepad(evt.cdevice.which);
            vlog_info(
                "Opened SDL_Gamepad ID #%i, %s",
                evt.cdevice.which,
                SDL_GetGamepadName(controller)
            );
            controllers[SDL_GetJoystickID(SDL_GetGamepadJoystick(controller))] = controller;
            BUTTONGLYPHS_keyboard_set_active(false);
            BUTTONGLYPHS_update_layout(controller);
            break;
        }
        case SDL_EVENT_GAMEPAD_REMOVED:
        {
            controller = controllers[evt.cdevice.which];
            controllers.erase(evt.cdevice.which);
            vlog_info("Closing %s", SDL_GetGamepadName(controller));
            SDL_CloseGamepad(controller);
            if (controllers.empty())
            {
                BUTTONGLYPHS_keyboard_set_active(true);
            }
            break;
        }

        /* Touch Input */
        case SDL_EVENT_FINGER_DOWN:
        {
            using_touch = true;

            VVV_Finger finger;
            finger.just_pressed = true;
            finger.just_released = false;
            finger.down = true;
            finger.x = evt.tfinger.x * (float)screen_width;
            finger.y = evt.tfinger.y * (float)screen_height;
            finger.x_normalized = evt.tfinger.x;

            sdl_finger_to_vvv_finger[evt.tfinger.fingerID] = next_finger_id;
            fingers[next_finger_id++] = finger;

            break;
        }
        case SDL_EVENT_FINGER_MOTION:
        {
            VVV_Finger* finger = getFingerFromSDL(evt.tfinger.fingerID);
            if (finger != NULL)
            {
                finger->x = evt.tfinger.x * (float)screen_width;
                finger->y = evt.tfinger.y * (float)screen_height;
                finger->x_normalized = evt.tfinger.x;
            }
            break;
        }
        case SDL_EVENT_FINGER_UP:
        {
            VVV_Finger* finger = getFingerFromSDL(evt.tfinger.fingerID);
            if (finger != NULL)
            {
                finger->just_released = true;
                finger->down = false;
            }
            break;
        }
        case SDL_EVENT_FINGER_CANCELED:
        {
            // Uh oh, cancelled by the OS
            // Basically the same as up, except let's not trigger anything (no released state)

            VVV_Finger* finger = getFingerFromSDL(evt.tfinger.fingerID);
            if (finger != NULL)
            {
                finger->down = false;
            }
            break;
        }
        case SDL_EVENT_RENDER_TARGETS_RESET:
            gameScreen.recacheTextures();
            break;

        /* Window Resize */
        case SDL_EVENT_WINDOW_RESIZED:
            if (SDL_GetWindowFlags(
                SDL_GetWindowFromID(evt.window.windowID)
            ) & SDL_WINDOW_INPUT_FOCUS)
            {
                resetWindow = true;
            }
            break;

        /* Window Focus */
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            if (!game.disablepause)
            {
                isActive = true;
                if ((!game.disableaudiopause || !game.disabletemporaryaudiopause) && music.currentsong != -1)
                {
                    music.resume();
                    music.resumeef();
                }
            }
            if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0)
            {
                if (wasFullscreen)
                {
                    gameScreen.isWindowed = false;
                    SDL_SetWindowFullscreen(
                        SDL_GetWindowFromID(evt.window.windowID),
                        true
                    );
                }
            }
            SDL_DisableScreenSaver();
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            if (!game.disablepause)
            {
                isActive = false;
                if (!game.disableaudiopause || !game.disabletemporaryaudiopause)
                {
                    music.pause();
                    music.pauseef();
                }
            }

            if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0)
            {
                wasFullscreen = !gameScreen.isWindowed;
                gameScreen.isWindowed = true;
                SDL_SetWindowFullscreen(
                    SDL_GetWindowFromID(evt.window.windowID),
                    false
                );
            }
            SDL_EnableScreenSaver();
            break;

        /* Mouse Focus */
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
            SDL_DisableScreenSaver();
            break;
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
            SDL_EnableScreenSaver();
            break;

        /* Quit Event */
        case SDL_EVENT_QUIT:
            VVV_exit(0);
            break;
        }

        switch (evt.type)
        {
        case SDL_EVENT_KEY_DOWN:
            using_touch = false;

            if (evt.key.repeat == 0)
            {
                hidemouse = true;
            }
            break;
        case SDL_EVENT_TEXT_INPUT:
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            using_touch = false;
            hidemouse = true;
            break;
        case SDL_EVENT_MOUSE_MOTION:
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            showmouse = true;
            break;
        }
    }

    mousetoggletimeout = changemousestate(
        mousetoggletimeout,
        showmouse,
        hidemouse
    );

    if (fullscreenkeybind)
    {
        toggleFullscreen();
    }

    SDL_Point mouse = gameScreen.transform_screen_to_world(raw_mousex, raw_mousey);
    mousex = mouse.x;
    mousey = mouse.y;

    active_input_device_changed = keyboard_was_active != BUTTONGLYPHS_keyboard_is_active();
    should_recompute_textboxes |= active_input_device_changed;
    if (should_recompute_textboxes)
    {
        recomputetextboxes();
    }
}

VVV_Finger* KeyPoll::getFingerFromSDL(SDL_FingerID sdl_finger_id)
{
    if (sdl_finger_to_vvv_finger.count(sdl_finger_id) == 0)
    {
        // we don't have a lookup
        return NULL;
    }

    VVV_FingerID vvv_finger_id = sdl_finger_to_vvv_finger[sdl_finger_id];
    if (vvv_finger_id == 0)
    {
        // invalid finger id
        return NULL;
    }

    if (fingers.count(vvv_finger_id) == 0)
    {
        // we don't have a finger for this id
        return NULL;
    }

    return &fingers[vvv_finger_id];
}

bool KeyPoll::isDown(SDL_Keycode key)
{
    return keymap[key];
}

bool KeyPoll::isDown(std::vector<SDL_GamepadButton> buttons)
{
    for (size_t i = 0; i < buttons.size(); i += 1)
    {
        if (buttonmap[buttons[i]])
        {
            return true;
        }
    }
    return false;
}

bool KeyPoll::isDown(SDL_GamepadButton button)
{
    return buttonmap[button];
}

bool KeyPoll::controllerButtonDown(void)
{
    for (
        SDL_GamepadButton button = SDL_GAMEPAD_BUTTON_SOUTH;
        button < SDL_GAMEPAD_BUTTON_DPAD_UP;
        button = (SDL_GamepadButton) (button + 1)
    ) {
        if (isDown(button))
        {
            return true;
        }
    }
    return false;
}

bool KeyPoll::controllerWantsLeft(bool includeVert)
{
    return (    buttonmap[SDL_GAMEPAD_BUTTON_DPAD_LEFT] ||
            xVel < 0 ||
            (    includeVert &&
                (    buttonmap[SDL_GAMEPAD_BUTTON_DPAD_UP] ||
                    yVel < 0    )    )    );
}

bool KeyPoll::controllerWantsRight(bool includeVert)
{
    return (    buttonmap[SDL_GAMEPAD_BUTTON_DPAD_RIGHT] ||
            xVel > 0 ||
            (    includeVert &&
                (    buttonmap[SDL_GAMEPAD_BUTTON_DPAD_DOWN] ||
                    yVel > 0    )    )    );
}

bool KeyPoll::controllerWantsUp(void)
{
    return buttonmap[SDL_GAMEPAD_BUTTON_DPAD_UP] || yVel < 0;
}

bool KeyPoll::controllerWantsDown(void)
{
    return buttonmap[SDL_GAMEPAD_BUTTON_DPAD_DOWN] || yVel > 0;
}

bool KeyPoll::touchPressed(void)
{
    std::map<VVV_FingerID, VVV_Finger>::iterator it;

    for (it = fingers.begin(); it != fingers.end(); ++it)
    {
        if (it->second.just_pressed)
        {
            return true;
        }
    }

    return false;
}

bool KeyPoll::touchDown(void)
{
    std::map<VVV_FingerID, VVV_Finger>::iterator it;

    for (it = fingers.begin(); it != fingers.end(); ++it)
    {
        if (it->second.down)
        {
            return true;
        }
    }

    return false;
}

void KeyPoll::resetSwipe(void)
{
    swipe_finger = 0;
    swipe_dir = SwipeDirection_NONE;
}

void KeyPoll::updateSwipe(void)
{
    if (fingers.count(swipe_finger) == 0)
    {
        resetSwipe();
    }

    if (swipe_finger == 0)
    {
        int screen_width;
        int screen_height;
        gameScreen.GetScreenSize(&screen_width, &screen_height);

        // no swipe finger -- does a finger exist on the left side?
        std::map<VVV_FingerID, VVV_Finger>::iterator it;

        for (it = fingers.begin(); it != fingers.end(); ++it)
        {
            if (it->second.just_pressed && it->second.x_normalized < 0.5)
            {
                // This is the one we want! Let's consume the press & use it
                it->second.just_pressed = false;

                swipe_finger = it->first;

                swipe_previous = it->second.x_normalized;
                swipe_dir = SwipeDirection_NONE;
                break;
            }
        }
    }

    // We have a swipe finger
    if (swipe_finger != 0)
    {
        VVV_Finger* finger = &fingers[swipe_finger];

        // extra sanity check
        if (finger == NULL)
        {
            resetSwipe();
            return;
        }

        float delta = finger->x_normalized - swipe_previous;

        if (delta > TOUCH_SWIPE_SENSITIVITY)
        {
            swipe_dir = SwipeDirection_RIGHT;
            swipe_previous = finger->x_normalized;
        }
        else if (delta < -TOUCH_SWIPE_SENSITIVITY)
        {
            swipe_dir = SwipeDirection_LEFT;
            swipe_previous = finger->x_normalized;
        }
    }
}

bool KeyPoll::touchWantsLeft(void)
{
    return swipe_dir == SwipeDirection_LEFT;
}

bool KeyPoll::touchWantsRight(void)
{
    return swipe_dir == SwipeDirection_RIGHT;
}

bool KeyPoll::touchWantsFlip(void)
{
    std::map<VVV_FingerID, VVV_Finger>::iterator it;

    for (it = fingers.begin(); it != fingers.end(); ++it)
    {
        if (it->second.down && (it->first != swipe_finger) && (it->second.x_normalized >= 0.5))
        {
            return true;
        }
    }

    return false;
}

VVV_Finger* KeyPoll::getFinger(VVV_FingerID finger_id)
{
    if (fingers.count(finger_id) == 0)
    {
        return NULL;
    }
    return &fingers[finger_id];
}

bool KeyPoll::isFingerInRect(VVV_FingerID finger_id, const SDL_FRect& rect, bool scaled)
{
    if (fingers.count(finger_id) == 0)
    {
        return false;
    }

    VVV_Finger* finger = &fingers[finger_id];

    if (finger == NULL)
    {
        return false;
    }

    SDL_FPoint point;

    if (scaled)
    {
        SDL_Point transformed = gameScreen.transform_screen_to_world(finger->x, finger->y);
        point.x = transformed.x;
        point.y = transformed.y;
    }
    else
    {
        point.x = finger->x;
        point.y = finger->y;
    }
    return SDL_PointInRectFloat(&point, &rect);
}

VVV_FingerID KeyPoll::capturePressedFingerAtRect(const SDL_FRect& rect, bool scaled)
{
    std::map<VVV_FingerID, VVV_Finger>::iterator it;
    for (it = fingers.begin(); it != fingers.end(); ++it)
    {
        if (it->second.just_pressed && isFingerInRect(it->first, rect, scaled))
        {
            // Capture it, and return
            it->second.just_pressed = false;

            return it->first;
        }
    }

    return 0;
}
