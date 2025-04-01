#include "TrinketFinFishItem.h"

#include "Item.h"
#include "ItemStack.h"
#include "SDL.h"

#include "ItemHelpers.h"

#include "Graphics.h"
#include "Maths.h"
#include "UtilityClass.h"

#include <string.h>

TrinketFinFishItem::TrinketFinFishItem(ItemSettings settings)
{
    this->settings = settings;
}

TrinketFinFishItem::TrinketFinFishItem(void)
{
}

std::string TrinketFinFishItem::getLongName(ItemStack* stack)
{
    return this->settings.name + " (" + help.String(getTrinketFinCount()) + "/5)";
};

std::string TrinketFinFishItem::getCatchText(ItemStack* stack)
{
    std::string amount;
    switch (getTrinketFinCount())
    {
    case 0: amount = "One"; break;
    case 1: amount = "Two"; break;
    case 2: amount = "Three"; break;
    case 3: amount = "Four"; break;
    case 4: amount = "Five"; break;
    default: amount = "Lots"; break;
    }
    return this->settings.name + "\n\n" + amount + " out of Five";
}
