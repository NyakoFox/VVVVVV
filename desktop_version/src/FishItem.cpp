#include "FishItem.h"

#include "Item.h"
#include "ItemStack.h"
#include "SDL.h"

#include "Graphics.h"
#include "Maths.h"
#include "UtilityClass.h"

#include <string.h>

FishItem::FishItem(ItemSettings settings, int min, int common, int max)
{
    this->settings = settings;
    this->min_size = min;
    this->common_size = common;
    this->max_size = max;
}

FishItem::FishItem(void)
{
}

void FishItem::getDefaultComponents(ItemStack* stack)
{
    // use a triangular distribution to determine the fish size

    double min = this->min_size;
    double common = this->common_size;
    double max = this->max_size;

    double r = (double)fRandom(); // Random number [0, 1]
    if (r < (double)(common - min) / (max - min)) {
        // Left side of the peak
        stack->fish_size = min + (int)sqrt(r * (common - min) * (max - min));
    }
    else {
        // Right side of the peak
        stack->fish_size = max - (int)sqrt((1 - r) * (max - common) * (max - min));
    }
}

std::string FishItem::getLongName(ItemStack* stack)
{
    // find the fish_size component
    int size = stack->fish_size;
    return this->settings.name + " - " + help.String(size) + "cm";
};
