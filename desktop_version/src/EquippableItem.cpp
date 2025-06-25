#include "EquippableItem.h"

#include "Item.h"
#include "ItemStack.h"
#include "SDL.h"

#include "Graphics.h"
#include "Maths.h"
#include "Music.h"
#include "UtilityClass.h"

#include <string.h>

EquippableItem::EquippableItem(ItemSettings settings)
{
    this->settings = settings;
}

EquippableItem::EquippableItem(void)
{
}

std::string EquippableItem::getShortName(ItemStack* stack)
{
    if (!this->settings.uses_short_name)
    {
        return this->getName(stack);
    }

    if (stack == NULL)
    {
        return this->settings.short_name;
    }
    if (stack->isEquipped())
    {
        return "\u2605 " + this->settings.short_name;
    }
    return this->settings.short_name;
}

std::string EquippableItem::getName(ItemStack* stack)
{
    if (stack == NULL)
    {
        return this->settings.name;
    }
    if (stack->isEquipped())
    {
        return "\u2605 " + this->settings.name;
    }
    return this->settings.name;
}

std::string EquippableItem::getLongName(ItemStack* stack)
{
    if (stack == NULL)
    {
        return this->settings.name;
    }
    if (stack->isEquipped())
    {
        return "\u2605 " + this->settings.name + " (Equipped)";
    }
    return this->settings.name;
}


SDL_Color EquippableItem::getNameColor(ItemStack* stack)
{
    if (stack == NULL)
    {
        return Item::getNameColor(stack);
    }
    if (stack->isEquipped())
    {
        return graphics.getRGB(255 - help.glow, 255 - help.glow, 196);
    }
    return Item::getNameColor(stack);
}


bool EquippableItem::canUse(ItemStack* stack)
{
    return true;
}

void EquippableItem::use(ItemStack* stack)
{
    stack->setEquipped(!stack->isEquipped());
    music.playef(Sound_VIRIDIAN);
}
