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

    Item(const Item& other) = delete;
    Item& operator=(const Item& other) = delete;

    void draw(int x, int y);

    virtual bool canUse(ItemStack* stack);
    virtual void use(ItemStack* stack);
    virtual void getDefaultComponents(ItemStack* stack);
    virtual std::string getName(ItemStack* stack);
    virtual std::string getLongName(ItemStack* stack);
    virtual std::string getDescription(ItemStack* stack);
    virtual SDL_Color getNameColor(ItemStack* stack);
protected:
    ItemSettings settings;
};

#endif /* ITEM_H */
