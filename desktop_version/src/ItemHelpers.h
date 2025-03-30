#ifndef ITEMHELPERS_H
#define ITEMHELPERS_H

#include "Game.h"
#include "Item.h"
#include "ItemStack.h"

#define FISHING_CHOOSE_MAX_TIME 40
#define FISHING_LINE_SMOOTHNESS 100

namespace Items
{
    extern Item* FISHING_ROD;
    extern Item* WORMS;
    extern Item* LARGEMOUTH_BASS;
    extern Item* SMALLMOUTH_BASS;
    extern Item* BULLHEAD;
    extern Item* CHUB;
    extern Item* PIKE;
    extern Item* WALLEYE;
    extern Item* CARP;
    extern Item* PERCH;
    extern Item* CATFISH;
    extern Item* MINNOW;
    extern Item* KOI_A;
    extern Item* KOI_B;
    extern Item* KOI_C;
    extern Item* RAINBOW_TROUT;
    extern Item* GOLDEN_TROUT;
    extern Item* GOLDFISH;
    extern Item* GOLD_GOLDFISH;
    extern Item* PRISMATIC_TROUT;
    extern Item* SPIKE_FISH;
    extern Item* TRINKETFIN;
    extern Item* YESFIN;
    extern Item* VIRIDIFIN;
    extern Item* EDGEFISH;
    extern Item* TERMINNOW;

    extern Item* TEST;
}

void giveItem(ItemStack stack);

static std::map<std::string, Item*> ITEM_REGISTRY;
void registerItems(void);
void cleanItems(void);

Item* getItem(std::string id);
std::string getItemID(Item* item);

ItemStack* getEquippedRod(void);

float easeOutCubic(float x);
float easeInCubic(float x);
float easeOutCirc(float x);
int lerpReal(int a, int b, float t);

#endif /* ITEMHELPERS_H */
