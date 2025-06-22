#include "FilledBucketItem.h"

#include "Item.h"
#include "ItemStack.h"
#include "ItemHelpers.h"
#include "SDL.h"

#include "Entity.h"
#include "Graphics.h"
#include "Maths.h"
#include "Music.h"
#include "UtilityClass.h"

#include <string.h>

FilledBucketItem::FilledBucketItem(ItemSettings settings)
{
    this->settings = settings;
}

FilledBucketItem::FilledBucketItem(void)
{
}

bool FilledBucketItem::canUse(ItemStack* stack)
{
    return true;
}

void FilledBucketItem::use(ItemStack* stack)
{
    stack->item = Items::BUCKET;
    music.playef(Sound_FILLBUCKET);

    // Are we polluting a water pool (come on man)
    if ((this == Items::GREENWATER_BUCKET) || (this == Items::BLUEWATER_BUCKET) || (this == Items::PURPLEWATER_BUCKET))
    {
        int i = obj.getplayer();
        if (INBOUNDS_VEC(i, obj.entities))
        {
            if (obj.entities[i].in_water && INBOUNDS_VEC(obj.entities[i].last_water, obj.blocks))
            {
                blockclass& block = obj.blocks[obj.entities[i].last_water];
                if ((block.script == "") ||
                    (block.script == "freshwater_small") ||
                    (block.script == "freshwater_large") ||
                    (block.script == "saltwater_small") ||
                    (block.script == "saltwater_large") ||
                    (block.script == "junk") ||
                    (block.script == "special_vitellary"))
                {
                    obj.flags[15] = true;
                }
            }
        }
    }
}
