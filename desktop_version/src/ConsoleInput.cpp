#include <string>
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <limits>

#include <SDL.h>

#include "ConsoleInput.h"

#include "Script.h"
#include "VFormat.h"
#include "Vlogging.h"

#include "UtilityClass.h"

namespace consoleinput
{
    std::string input_text;
    bool has_input = false;
    bool created_thread = false;

    int thread(void* data)
    {
        if (std::getline(std::cin, input_text))
        {
            has_input = true;
        }
        else
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        created_thread = false;
        return 0;
    }

    void begin(void)
    {
        if (!created_thread)
        {
            created_thread = true;
            SDL_CreateThread(thread, "ConsoleInput", NULL);
        }
    }

    std::string getinput(void)
    {
        if (!created_thread) return "";
        if (has_input)
        {
            has_input = false;
            return input_text;
        }
        return "";
    }

    bool hasinput(void)
    {
        if (!created_thread) return false;
        return has_input;
    }
}