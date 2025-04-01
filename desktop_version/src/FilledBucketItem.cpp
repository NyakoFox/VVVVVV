#include "FilledBucketItem.h"

#include "Item.h"
#include "ItemStack.h"
#include "ItemHelpers.h"
#include "SDL.h"

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
}
