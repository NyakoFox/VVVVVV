#include "BucketItem.h"

#include "Item.h"
#include "ItemStack.h"
#include "SDL.h"
#include "ItemHelpers.h"

#include "Entity.h"
#include "Graphics.h"
#include "Maths.h"
#include "Music.h"
#include "UtilityClass.h"

#include <string.h>

BucketItem::BucketItem(ItemSettings settings)
{
    this->settings = settings;
}

BucketItem::BucketItem(void)
{
}

bool BucketItem::canUse(ItemStack* stack)
{
    return true;
}

void BucketItem::use(ItemStack* stack)
{
    int i = obj.getplayer();
    if (INBOUNDS_VEC(i, obj.entities))
    {
        if (obj.entities[i].in_water && (INBOUNDS_VEC(obj.entities[i].last_water, obj.blocks)))
        {
            blockclass& block = obj.blocks[obj.entities[i].last_water];
            if (block.script == "") stack->item = Items::FRESHWATER_BUCKET;
            else if (block.script == "freshwater_small") stack->item = Items::FRESHWATER_BUCKET;
            else if (block.script == "freshwater_large") stack->item = Items::FRESHWATER_BUCKET;
            else if (block.script == "saltwater_small") stack->item = Items::SALTWATER_BUCKET;
            else if (block.script == "saltwater_large") stack->item = Items::SALTWATER_BUCKET;
            else if (block.script == "junk") stack->item = Items::JUNKWATER_BUCKET;
            else if (block.script == "special_green") stack->item = Items::GREENWATER_BUCKET;
            else if (block.script == "special_blue") stack->item = Items::BLUEWATER_BUCKET;
            else if (block.script == "special_purple") stack->item = Items::PURPLEWATER_BUCKET;
            else
            {
                music.playef(Sound_ERROR);
                return;
            }
            music.playef(Sound_FILLBUCKET);
            return;
        }
    }
    music.playef(Sound_ALARM);
}
