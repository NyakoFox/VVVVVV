#include "Item.h"

#include "SDL.h"

#include "Graphics.h"
#include "UtilityClass.h"

#include "FishItem.h"

#include <string.h>

class ItemStack; // Forward declaration

Item::Item(ItemSettings settings)
{
    this->settings = settings;
}

Item::Item(void)
{
}

void Item::getDefaultComponents(ItemStack* stack)
{
}

bool Item::canUse(ItemStack* stack)
{
    return false;
}

void Item::use(ItemStack* stack)
{
}

std::string Item::getName(ItemStack* stack)
{
    return this->settings.name;
}

std::string Item::getLongName(ItemStack* stack)
{
    return this->settings.name;
}

std::string Item::getDescription(ItemStack* stack)
{
    return this->settings.description;
}

SDL_Color Item::getNameColor(ItemStack* stack)
{
    if (this->settings.name_color != -1)
    {
        return graphics.getcol(this->settings.name_color);
    }
    switch (getRarity(stack))
    {
    case Rarity_JUNK: return graphics.getRGB(140, 140, 140);
    case Rarity_COMMON: return graphics.getRGB(196, 196, 255 - help.glow);
    case Rarity_UNCOMMON: return graphics.getRGB(135, 135, 255 - help.glow);
    case Rarity_RARE: return graphics.getRGB(128, 255 - help.glow, 128);
    case Rarity_ELUSIVE: return graphics.getRGB(240 - help.glow, 255 - help.glow, 120);
    case Rarity_LEGENDARY: return graphics.getRGB(255 - help.glow, 192 - help.glow, 0);
    }

    return graphics.getRGB(196, 196, 255 - help.glow);
}

void Item::draw(int x, int y)
{
    for (int i = 0; i < this->settings.sprite_layers.size(); i++)
    {
        // Draw sprite layer
        SDL_Texture* texture = graphics.item_sprites[this->settings.sprite_layers[i].texture_id];
        if (texture == NULL)
        {
            continue;
        }

        SDL_Color color = graphics.getcol(this->settings.sprite_layers[i].color);

        graphics.set_texture_color_mod(texture, color.r, color.g, color.b);
        graphics.set_texture_alpha_mod(texture, color.a);
        graphics.draw_texture(texture, x, y);
        graphics.set_texture_color_mod(texture, 255, 255, 255);
        graphics.set_texture_alpha_mod(texture, 255);
    }
}

std::string Item::getCatchText(ItemStack* stack)
{
    return "\n" + getName(stack) + "\n\n";
}

ItemRarity Item::getRarity(ItemStack* stack)
{
    return settings.rarity;
}

int Item::getSellPrice(ItemStack* stack)
{
    return settings.sell_price;
}

bool Item::canSell(ItemStack* stack)
{
    return settings.can_sell;
}

int Item::getBuyPrice(ItemStack* stack)
{
    return settings.buy_price;
}

bool Item::canBuy(ItemStack* stack)
{
    return settings.can_buy;
}
