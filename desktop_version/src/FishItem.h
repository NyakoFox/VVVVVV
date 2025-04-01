#ifndef FISHITEM_H
#define FISHITEM_H

#include "Item.h"

#include <string>
#include <map>
#include <vector>

#include "SDL.h"

class FishItem : public Item
{
public:
    FishItem(ItemSettings settings, int min, int common, int max);
    FishItem(void);
    virtual void getDefaultComponents(ItemStack* stack) override;
    virtual std::string getLongName(ItemStack* stack) override;
    virtual std::string getCatchText(ItemStack* stack) override;
    virtual int getSellPrice(ItemStack* stack) override;
    int min_size;
    int common_size;
    int max_size;
};

#endif /* FISHITEM_H */
