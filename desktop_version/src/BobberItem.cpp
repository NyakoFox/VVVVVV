#include "BobberItem.h"

#include "Item.h"
#include "ItemStack.h"
#include "SDL.h"
#include "ItemHelpers.h"

#include "Graphics.h"
#include "Maths.h"
#include "Music.h"
#include "UtilityClass.h"

#include <string.h>

BobberItem::BobberItem(ItemSettings settings)
{
    this->settings = settings;
}

BobberItem::BobberItem(void)
{
}

void BobberItem::use(ItemStack* stack)
{
    bool wasEquipped = stack->isEquipped();
    for (int i = 0; i < game.inventory.size(); i++)
    {
        ItemStack& inv_stack = game.inventory[i];
        if (inv_stack.item == Items::GREEN_BOBBER)
        {
            inv_stack.setEquipped(false);
        }
        if (inv_stack.item == Items::BLUE_BOBBER)
        {
            inv_stack.setEquipped(false);
        }
        if (inv_stack.item == Items::PURPLE_BOBBER)
        {
            inv_stack.setEquipped(false);
        }
        if (inv_stack.item == Items::BIG_BOBBER)
        {
            inv_stack.setEquipped(false);
        }
        if (inv_stack.item == Items::FISH_SPINNER)
        {
            inv_stack.setEquipped(false);
        }
        if (inv_stack.item == Items::FEATHER_SPINNER)
        {
            inv_stack.setEquipped(false);
        }
        if (inv_stack.item == Items::COIN_SPINNER)
        {
            inv_stack.setEquipped(false);
        }
        if (inv_stack.item == Items::TRINKET_SPINNER)
        {
            inv_stack.setEquipped(false);
        }
        if (inv_stack.item == Items::VIRIDIAN_SPINNER)
        {
            inv_stack.setEquipped(false);
        }
        if (inv_stack.item == Items::GIANT_BOBBER)
        {
            inv_stack.setEquipped(false);
        }
        if (inv_stack.item == Items::NAVAL_MINE_BOBBER)
        {
            inv_stack.setEquipped(false);
        }
    }
    stack->setEquipped(!wasEquipped);
    music.playef(Sound_VIRIDIAN);
}
