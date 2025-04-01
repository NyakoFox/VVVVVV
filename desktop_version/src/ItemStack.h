#ifndef ITEMSTACK_H
#define ITEMSTACK_H

#include <map>
#include <memory>
#include <string>

#include "Item.h"

#include "SDL.h"

#include "tinyxml2/tinyxml2.h"

class ItemStack
{
public:
    ItemStack(void);
    ItemStack(Item* item, int count);
    ItemStack(const ItemStack& other);

    Item* item;
    int count;

    void addComponent(std::string key, std::string value);

    void increment(int amount);
    void increment(void);
    bool decrement(int amount);
    bool decrement(void);

    bool isEmpty(void);

    bool canUse(void);
    void use(void);
    std::string getName(void);
    std::string getLongName(void);
    std::string getDescription(void);
    std::string getCatchText(void);
    SDL_Color getNameColor(void);
    ItemRarity getRarity(void);
    int getBuyPrice(void);
    bool canBuy(void);
    int getSellPrice(void);
    bool canSell(void);

    tinyxml2::XMLElement* serialize(tinyxml2::XMLDocument* doc);

    bool isEquipped(void);
    void setEquipped(bool equipped);

    int fish_size;
private:
    bool equipped;
};

#endif /* ITEMSTACK_H */
