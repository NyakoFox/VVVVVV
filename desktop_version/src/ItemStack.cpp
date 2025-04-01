#include "ItemStack.h"

#include "Graphics.h"
#include "Item.h"
#include "ItemHelpers.h"

#include "UtilityClass.h"

#include "SDL.h"
#include "tinyxml2/tinyxml2.h"

ItemStack::ItemStack()
{
    this->item = Items::FISHING_ROD;
    this->count = 1;
    this->fish_size = 0;
    this->equipped = false;
}

ItemStack::ItemStack(Item* item, int count)
{
    this->item = item;
    this->count = count;
    this->fish_size = 0;
    this->equipped = false;
}

ItemStack::ItemStack(const ItemStack& other)
{
    this->item = other.item;
    this->count = other.count;

    this->fish_size = other.fish_size;
    this->equipped = other.equipped;
}

void ItemStack::increment(int amount)
{
    this->count += amount;
}

void ItemStack::increment(void)
{
    this->count++;
}

bool ItemStack::decrement(int amount)
{
    while (amount > 0)
    {
        if (!this->decrement())
        {
            return false;
        }
        amount--;
    }
}

bool ItemStack::decrement(void)
{
    if (this->count == 0)
    {
        return false;
    }
    this->count--;
    if (this->count == 0)
    {
        this->item = nullptr;
        return false;
    }
    return true;
}

bool ItemStack::isEmpty(void)
{
    if (this->item == nullptr)
    {
        return true;
    }
    return this->count == 0;
}

bool ItemStack::canUse(void)
{
    if (this->item == nullptr)
    {
        return false;
    }
    return this->item->canUse(this);
}

void ItemStack::use(void)
{
    if (this->item == nullptr)
    {
        return;
    }
    this->item->use(this);
}

std::string ItemStack::getName(void)
{
    if (this->item == nullptr)
    {
        return "null";
    }
    return this->item->getName(this);
}

std::string ItemStack::getLongName(void)
{
    if (this->item == nullptr)
    {
        return "null";
    }
    return this->item->getLongName(this);
}

std::string ItemStack::getDescription(void)
{
    if (this->item == nullptr)
    {
        return "null";
    }
    return this->item->getDescription(this);
}

SDL_Color ItemStack::getNameColor(void)
{
    if (this->item == nullptr)
    {
        return graphics.getRGB(255, 0, 0);
    }
    return this->item->getNameColor(this);
}

bool ItemStack::isEquipped(void)
{
    return this->equipped;
}

void ItemStack::setEquipped(bool equipped)
{
    this->equipped = equipped;
}

static void serializeComponent(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* item_element, std::string key, std::string value)
{
    tinyxml2::XMLElement* component_element;
    component_element = doc->NewElement("component");
    component_element->SetAttribute("key", key.c_str());
    component_element->SetText(value.c_str());
    item_element->LinkEndChild(component_element);
}

tinyxml2::XMLElement* ItemStack::serialize(tinyxml2::XMLDocument* doc)
{
    tinyxml2::XMLElement* item_element;
    item_element = doc->NewElement("item");
    item_element->SetAttribute("item", getItemID(this->item).c_str());
    item_element->SetAttribute("count", this->count);
    // Serialize components

    serializeComponent(doc, item_element, "fish_size", std::to_string(this->fish_size));
    serializeComponent(doc, item_element, "equipped", std::to_string(this->equipped ? 1 : 0));

    return item_element;
}

void ItemStack::addComponent(std::string key, std::string value)
{
    if (key == "fish_size")
    {
        this->fish_size = ss_toi(value);
        return;
    }
    if (key == "equipped")
    {
        this->equipped = ss_toi(value) == 1;
        return;
    }
}

std::string ItemStack::getCatchText(void)
{
    if (this->item == nullptr)
    {
        return "null";
    }
    return this->item->getCatchText(this);
}

ItemRarity ItemStack::getRarity(void)
{
    if (this->item == nullptr)
    {
        return Rarity_JUNK;
    }
    return this->item->getRarity(this);
}

int ItemStack::getBuyPrice(void)
{
    if (this->item == nullptr)
    {
        return 0;
    }
    return this->item->getBuyPrice(this);
}

int ItemStack::getSellPrice(void)
{
    if (this->item == nullptr)
    {
        return 0;
    }
    return this->item->getSellPrice(this);
}

bool ItemStack::canBuy(void)
{
    if (this->item == nullptr)
    {
        return false;
    }
    return this->item->canBuy(this);
}

bool ItemStack::canSell(void)
{
    if (this->item == nullptr)
    {
        return false;
    }
    return this->item->canSell(this);
}
