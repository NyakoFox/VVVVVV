#include "Touch.h"

#include <SDL3/SDL.h>

#include "Font.h"
#include "Graphics.h"
#include "Input.h"
#include "KeyPoll.h"
#include "Screen.h"
#include "UtilityClass.h"

namespace touch
{
    std::vector<TouchControl> dynamic_controls;

    void init(void)
    {
        dynamic_controls.clear();
    }

    TouchControl* create_menu_button(int x, int y, int width, int height, const std::string& text, int option_id, bool active)
    {
        TouchControl control;
        control.type = TouchControlType_MENU_BUTTON;
        control.rect.x = x;
        control.rect.y = y;
        control.rect.w = width;
        control.rect.h = height;
        control.down = false;
        control.pressed_finger = 0;
        control.active = active;

        control.text = text;
        control.option_id = option_id;

        dynamic_controls.push_back(control);
        return &dynamic_controls.back();
    }

    TouchControl* create_menu_button(int x, int y, int width, int height, const std::string& text, int option_id)
    {
        return create_menu_button(x, y, width, height, text, option_id, true);
    }

    static void control_press(TouchControl* control)
    {
    }

    static void control_update(TouchControl* control)
    {
    }

    static void control_release(TouchControl* control)
    {
        switch (control->type)
        {
        case TouchControlType_MENU_BUTTON:
            if (key.isFingerInRect(control->pressed_finger, control->rect, true))
            {
                // We've released a press on a menu button, so let's simulate selecting it
                game.currentmenuoption = control->option_id;
                menuactionpress();
            }
            break;
        default:
            break;
        }
    }

    static void control_cancel(TouchControl* control)
    {
    }

    static void control_render(TouchControl* control, int r, int g, int b, int scale)
    {
        switch (control->type)
        {
        case TouchControlType_MENU_BUTTON:
        {
            int use_r = control->active ? r : 127;
            int use_g = control->active ? g : 127;
            int use_b = control->active ? b : 127;

            float shadow_div = 4;
            float inner_div = 1.5;

            int offset = control->down ? 1 : 0;

            int font_scale = SDL_min((scale - 1), 7);
            int height = font::height(PR_CJK_LOW | font_scale);

            graphics.fill_rect(control->rect.x + 4 * scale, control->rect.y + 4 * scale, control->rect.w, control->rect.h, use_r / shadow_div, use_g / shadow_div, use_b / shadow_div);

            graphics.fill_rect(control->rect.x + offset * scale, control->rect.y + offset * scale, control->rect.w, control->rect.h, use_r, use_g, use_b);
            graphics.fill_rect(control->rect.x + (offset + 2) * scale, control->rect.y + (2 + offset) * scale, control->rect.w - 4 * scale, control->rect.h - 4 * scale, use_r / inner_div, use_g / inner_div, use_b / inner_div);

            font::print(PR_CEN | PR_CJK_LOW | font_scale, control->rect.x + (control->rect.w / 2) + offset * scale, control->rect.y + ((control->rect.h - height) / 2 + offset * scale), control->text, 196, 196, 255 - help.glow);
            break;
        }
        default:
            break;
        }
    }

    void input(void)
    {
        for (size_t i = 0; i < dynamic_controls.size(); i++)
        {
            TouchControl* control = &dynamic_controls[i];

            if (!control->active)
            {
                control->down = false;
                control->pressed_finger = 0;
                control_cancel(control);
                continue;
            }

            if (!control->down)
            {
                // Find a finger over the control...
                VVV_FingerID id = key.capturePressedFingerAtRect(control->rect, true);
                if (id != 0)
                {
                    // We found one, let's use it

                    control->down = true;
                    control->pressed_finger = id;
                    control_press(control);
                }
            }

            if (control->down)
            {
                VVV_Finger* finger = key.getFinger(control->pressed_finger);
                if (finger == NULL)
                {
                    control->down = false;
                    control->pressed_finger = 0;
                    control_cancel(control);
                }
                else
                {
                    if (!finger->down)
                    {
                        if (finger->just_released)
                        {
                            control_release(control);
                        }

                        control->down = false;
                        control->pressed_finger = 0;
                    }
                    else
                    {
                        control_update(control);
                    }
                }
            }
        }
    }

    void render_dynamic_controls(int r, int g, int b)
    {
        for (size_t i = 0; i < dynamic_controls.size(); i++)
        {
            TouchControl* control = &dynamic_controls[i];

            control_render(control, r, g, b, 1);
        }
    }

    void clear_dynamic_controls(void)
    {
        dynamic_controls.clear();
    }
}
