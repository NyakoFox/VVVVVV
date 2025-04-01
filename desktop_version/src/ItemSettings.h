#ifndef ITEMSETTINGS_H
#define ITEMSETTINGS_H

#include <string>
#include <vector>

enum ItemRarity
{
    Rarity_JUNK,
    Rarity_COMMON,
    Rarity_UNCOMMON,
    Rarity_RARE,
    Rarity_ELUSIVE,
    Rarity_LEGENDARY
};

struct SpriteLayer
{
    std::string texture_id;
    int color;
};

class ItemSettings {
public:
    ItemSettings();
    // we're going to be using the builder pattern here
    // copy constructor
    ItemSettings(const ItemSettings& other);

    std::string name;
    std::string description;
    std::vector<SpriteLayer> sprite_layers;
    int name_color;
    ItemRarity rarity;

    int buy_price;
    int sell_price;
    bool can_buy;
    bool can_sell;

    ItemSettings withName(std::string name);
    ItemSettings withDescription(std::string description);
    ItemSettings withLayer(std::string texture_id, int color);
    ItemSettings withLayer(SpriteLayer layer);
    ItemSettings withNameColor(int color);
    ItemSettings withRarity(ItemRarity rarity);
    ItemSettings withSell(int price);
    ItemSettings withBuy(int price);
};

#endif /* ITEMSETTINGS_H */
