#ifndef EQUIPPABLEITEM_H
#define EQUIPPABLEITEM_H

#include "Item.h"

#include <string>
#include <map>
#include <vector>

#include "SDL.h"

class EquippableItem : public Item
{
public:
    EquippableItem(ItemSettings settings);
    EquippableItem(void);
    virtual bool canUse(ItemStack* stack) override;
    virtual void use(ItemStack* stack) override;
    virtual std::string getName(ItemStack* stack) override;
    virtual std::string getLongName(ItemStack* stack) override;
    virtual SDL_Color getNameColor(ItemStack* stack) override;
};

#endif /* EQUIPPABLEITEM_H */
