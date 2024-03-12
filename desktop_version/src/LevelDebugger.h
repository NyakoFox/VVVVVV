#ifndef LEVELDEBUGGER_H
#define LEVELDEBUGGER_H

#include <SDL.h>

namespace level_debugger
{
    bool is_pausing(void);
    bool is_active(void);
    void input(void);
    void logic(void);
    void render(void);
    void set_forced(void);
    void open_flag_menu(void);
}

#endif /* LEVELDEBUGGER_H */
