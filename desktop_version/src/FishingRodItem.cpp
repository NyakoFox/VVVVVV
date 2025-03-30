#include "FishingRodItem.h"

#include "Item.h"
#include "ItemStack.h"
#include "SDL.h"

#include "Graphics.h"
#include "Maths.h"
#include "Music.h"
#include "UtilityClass.h"

#include <string.h>

FishingRodItem::FishingRodItem(ItemSettings settings)
{
    this->settings = settings;
}

FishingRodItem::FishingRodItem(void)
{
}
