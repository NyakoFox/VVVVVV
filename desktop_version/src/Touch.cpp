#include "Touch.h"

#include <SDL3/SDL.h>

#include "Font.h"
#include "Graphics.h"
#include "Input.h"
#include "KeyPoll.h"
#include "Localization.h"
#include "Screen.h"
#include "UtilityClass.h"

namespace touch
{
    std::vector<TouchControl> dynamic_controls;

    void init(void)
    {
        dynamic_controls.clear();
    }

    TouchControl* create_control(TouchControlType type, int x, int y, int width, int height)
    {
        TouchControl control;
        control.type = type;
        control.rect.x = x;
        control.rect.y = y;
        control.rect.w = width;
        control.rect.h = height;
        control.down = false;
        control.pressed_finger = 0;
        control.active = true;

        dynamic_controls.push_back(control);
        return &dynamic_controls.back();
    }

    TouchControl* create_menu_button(int x, int y, int width, int height, const std::string& text, int option_id, bool active)
    {
        TouchControl* control = create_control(TouchControlType_MENU_BUTTON, x, y, width, height);
        control->active = active;

        control->text = text;
        control->text_flags = 0;
        control->option_id = option_id;

        return control;
    }

    TouchControl* create_menu_button(int x, int y, int width, int height, const std::string& text, int option_id)
    {
        return create_menu_button(x, y, width, height, text, option_id, true);
    }


    TouchControl* create_menu_button(int x, int y, int width, int height, SDL_Texture* icon, int option_id)
    {
        TouchControl* control = create_control(TouchControlType_MENU_BUTTON_ICON, x, y, width, height);
        control->image = const_cast<SDL_Texture*>(icon);
        control->option_id = option_id;
        return control;
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
        case TouchControlType_MENU_BUTTON_ICON:
        case TouchControlType_MENU_BUTTON_LANGUAGE:
        case TouchControlType_MENU_BUTTON_TELESAVE:
        case TouchControlType_MENU_BUTTON_QUICKSAVE:
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

    static void draw_button_background(TouchControl* control, int r, int g, int b, int back_r, int back_g, int back_b, int scale)
    {
        int use_r = control->active ? r : 127;
        int use_g = control->active ? g : 127;
        int use_b = control->active ? b : 127;

        int use_back_r = control->active ? back_r : 84;
        int use_back_g = control->active ? back_g : 84;
        int use_back_b = control->active ? back_b : 84;

        int offset = control->down ? 1 : 0;

        // The shadow
        graphics.fill_rect(
            control->rect.x + 4 * scale,
            control->rect.y + 4 * scale,
            control->rect.w,
            control->rect.h,
            use_r / 4, use_g / 4, use_b / 4
        );

        // The outer rectangle (the border)
        graphics.fill_rect(
            control->rect.x + offset * scale,
            control->rect.y + offset * scale,
            control->rect.w,
            control->rect.h,
            use_r, use_g, use_b
        );

        // The inner rectangle
        graphics.fill_rect(
            control->rect.x + (offset + 2) * scale,
            control->rect.y + (2 + offset) * scale,
            control->rect.w - 4 * scale,
            control->rect.h - 4 * scale,
            use_back_r, use_back_g, use_back_b
        );
    }

    static void draw_button_background(TouchControl* control, int r, int g, int b, int scale)
    {
        draw_button_background(control, r, g, b, r / 1.5, g / 1.5, b / 1.5, scale);
    }

    static void control_render(TouchControl* control, int r, int g, int b, int scale)
    {
        switch (control->type)
        {
        case TouchControlType_MENU_BUTTON_LANGUAGE:
            // Specifically for the language menu; dim buttons that aren't the current language
            if (control->option_id != loc::languagelist_curlang && control->option_id >= 0)
            {
                r /= 2;
                g /= 2;
                b /= 2;
            }

            SDL_FALLTHROUGH;
        case TouchControlType_MENU_BUTTON:
        {
            draw_button_background(control, r, g, b, scale);

            int offset = control->down ? 1 : 0;

            int font_scale = SDL_min((scale - 1), 7);
            int height = font::height(PR_CJK_LOW | font_scale | control->text_flags);

            font::print(
                PR_CEN | PR_CJK_LOW | font_scale | control->text_flags,
                control->rect.x + (control->rect.w / 2) + offset * scale,
                control->rect.y + ((control->rect.h - height) / 2 + offset * scale),
                control->text,
                196, 196, 255 - help.glow
            );

            break;
        }
        case TouchControlType_MENU_BUTTON_ICON:
        {
            draw_button_background(control, r, g, b, scale);

            int offset = control->down ? 1 : 0;

            int width;
            int height;

            graphics.query_texture(control->image, NULL, NULL, &width, &height);

            int x = control->rect.x + (control->rect.w - width) / 2;
            int y = control->rect.y + (control->rect.h - height) / 2;

            // Draw the icon
            graphics.set_texture_color_mod(control->image, 196, 196, 255 - help.glow);
            graphics.draw_texture(control->image, (x + offset) * scale, (y + offset) * scale, scale, scale);
            graphics.set_texture_color_mod(control->image, 255, 255, 255);

            break;
        }
        case TouchControlType_MENU_BUTTON_TELESAVE:
        case TouchControlType_MENU_BUTTON_QUICKSAVE:
        {
            draw_button_background(control, r, g, b, 65 / 6, 185 / 6, 207 / 6, 1);

            int offset = control->down ? 1 : 0;

            font::print(PR_CEN | PR_BOR, control->rect.x + (control->rect.w / 2) + offset, control->rect.y + offset + 6, control->text, 196, 196, 255 - help.glow);
            graphics.draw_summary(control->type == TouchControlType_MENU_BUTTON_TELESAVE  ? &game.last_telesave : &game.last_quicksave, offset, control->rect.y + 22 + offset);
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
