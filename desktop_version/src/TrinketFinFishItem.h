#ifndef TINKETFINFISHITEM_H
#define TINKETFINFISHITEM_H

#include "Item.h"

#include <string>
#include <map>
#include <vector>

#include "SDL.h"

class TrinketFinFishItem : public Item
{
public:
    TrinketFinFishItem(ItemSettings settings);
    TrinketFinFishItem(void);
    virtual std::string getLongName(ItemStack* stack) override;
    virtual std::string getCatchText(ItemStack* stack) override;
};

#endif /* TINKETFINFISHITEM_H */
