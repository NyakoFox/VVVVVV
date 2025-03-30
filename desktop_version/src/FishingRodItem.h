#ifndef FISHINGRODITEM_H
#define FISHINGRODITEM_H

#include "Item.h"
#include "EquippableItem.h"

#include <string>
#include <map>
#include <vector>

#include "SDL.h"

class FishingRodItem : public EquippableItem
{
public:
    FishingRodItem(ItemSettings settings);
    FishingRodItem(void);
};

#endif /* FISHINGRODITEM_H */
