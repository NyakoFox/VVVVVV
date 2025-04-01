#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <map>
#include <vector>

#include "SDL.h"

#include "ItemSettings.h"

class ItemStack; // Forward declaration

class Item
{
public:
    Item(ItemSettings settings);
    Item(void);

    void draw(int x, int y);

    virtual bool canUse(ItemStack* stack);
    virtual void use(ItemStack* stack);
    virtual void getDefaultComponents(ItemStack* stack);
    virtual std::string getName(ItemStack* stack);
    virtual std::string getLongName(ItemStack* stack);
    virtual std::string getDescription(ItemStack* stack);
    virtual std::string getCatchText(ItemStack* stack);
    virtual SDL_Color getNameColor(ItemStack* stack);
    virtual ItemRarity getRarity(ItemStack* stack);
    virtual int getBuyPrice(ItemStack* stack);
    virtual int getSellPrice(ItemStack* stack);
    virtual bool canBuy(ItemStack* stack);
    virtual bool canSell(ItemStack* stack);
protected:
    ItemSettings settings;
};

#endif /* ITEM_H */
