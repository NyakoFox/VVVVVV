#include "ItemSettings.h"

#include "UtilityClass.h"

#include <string>

ItemSettings::ItemSettings(void)
{
    name = "";
    description = "";
    sprite_layers.clear();
    name_color = -1;
    rarity = Rarity_COMMON;
}

ItemSettings::ItemSettings(const ItemSettings& other)
{
    name = other.name;
    description = other.description;
    sprite_layers = other.sprite_layers;
    name_color = other.name_color;
    rarity = other.rarity;
}

ItemSettings ItemSettings::withName(std::string name)
{
    this->name = name;
    return *this;
}

ItemSettings ItemSettings::withDescription(std::string description)
{
    this->description = description;
    return *this;
}

ItemSettings ItemSettings::withLayer(std::string texture_id, int color)
{
    SpriteLayer layer;
    layer.texture_id = texture_id;
    layer.color = color;
    sprite_layers.push_back(layer);
    return *this;
}

ItemSettings ItemSettings::withLayer(SpriteLayer layer)
{
    sprite_layers.push_back(layer);
    return *this;
}

ItemSettings ItemSettings::withNameColor(int color)
{
    this->name_color = color;
    return *this;
}

ItemSettings ItemSettings::withRarity(ItemRarity rarity)
{
    this->rarity = rarity;
    return *this;
}
