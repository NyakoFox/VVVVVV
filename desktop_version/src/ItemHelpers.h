#ifndef ITEMHELPERS_H
#define ITEMHELPERS_H

#include "Game.h"
#include "Item.h"
#include "ItemStack.h"

#define FISHING_CHOOSE_MAX_TIME 40
#define FISHING_LINE_SMOOTHNESS 100

struct PoolData {
    Item* item;
    int weight;
};

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
    extern Item* TIRE;

    extern Item* RED_SNAPPER;
    extern Item* TUNA;
    extern Item* BLUE_MARLIN;
    extern Item* AMBERJACK;
    extern Item* COALFISH;
    extern Item* WAHOO;
    extern Item* ANCHOVY;
    extern Item* SARDINE;
    extern Item* POMPANO;
    extern Item* PUFFERFISH;
    extern Item* MAHIMAHI;
    extern Item* MORAY_EEL;
    extern Item* RIBBON_EEL;
    extern Item* SQUID;
    extern Item* OCTOPUS;

    extern Item* STAR_FISH;
    extern Item* FISHTRONAUT;
    extern Item* NAVAL_MINE;

    extern Item* OLD_BOOT;
    extern Item* SODA_CAN;
    extern Item* TIN_CAN;
    extern Item* CAR_BATTERY;
    extern Item* GAME_CARTRIDGE;
    extern Item* DRIFTWOOD;
    extern Item* SEA_GLASS;

    extern Item* THIRTY_SEVEN_FISH;
    extern Item* SQUISHFISH;

    extern Item* SALMON_1;
    extern Item* SALMON_2;
    extern Item* STURGEON;
    extern Item* SHORTNOSE_GAR;
    extern Item* LONGNOSE_GAR;
    extern Item* GROUPER;
    extern Item* BLUEFISH;
    extern Item* FLOUNDER;
    extern Item* FISHBONE;

    extern Item* KELP;
    extern Item* DULSE;
    extern Item* WATER_SILK;

    extern Item* YELLOW_KEY_FAKE;
    extern Item* YELLOW_KEY;
    extern Item* BLUE_KEY;
    extern Item* PURPLE_KEY;
    extern Item* RED_KEY;

    extern Item* BUCKET;
    extern Item* FRESHWATER_BUCKET;
    extern Item* SALTWATER_BUCKET;
    extern Item* JUNKWATER_BUCKET;
    extern Item* GREENWATER_BUCKET;
    extern Item* BLUEWATER_BUCKET;
    extern Item* PURPLEWATER_BUCKET;
    extern Item* VITELLARYWATER_BUCKET;

    extern Item* ENHANCED_BAIT;
    extern Item* DELUXE_BAIT;
    extern Item* ULTRA_BAIT;

    extern Item* GREEN_BOBBER;
    extern Item* BLUE_BOBBER;
    extern Item* PURPLE_BOBBER;
    extern Item* BIG_BOBBER;
    extern Item* FISH_SPINNER;
    extern Item* FEATHER_SPINNER;
    extern Item* COIN_SPINNER;
    extern Item* TRINKET_SPINNER;
    extern Item* VIRIDIAN_SPINNER;
    extern Item* GIANT_BOBBER;
    extern Item* NAVAL_MINE_BOBBER;
}

bool hasItem(Item* item);

void giveItem(ItemStack stack);
void giveItem(ItemStack stack, bool display);

static std::map<std::string, Item*> ITEM_REGISTRY;
void registerItems(void);
void cleanItems(void);

Item* getItem(std::string id);
std::string getItemID(Item* item);

bool canUpgradeBait(void);
void upgradeBait(void);

void removeEmptyInventorySlots(void);

bool hasBait(void);
void useBait(void);
int getBaitTier(void);

void decrementItem(ItemStack stack, int amount);

void updateFishCaughtInfo(void);

int getTrinketFinCount(void);
int getCaughtFishAmount(void);
ItemStack* getEquippedRod(void);

SDL_Color getWaterColorsForPool(std::string pool);
ItemStack getItemForPool(std::string pool, int flag);
std::vector<ItemStack> getShopItems(void);

static std::map<std::string, std::vector<PoolData> > POOLS;

static std::vector<Item*> BESTIARY_ITEMS;

std::vector<Item*> getBestiaryItems(void);
bool hasDiscovered(Item* item);
FishCatchInfo getFishCatchInfo(Item* item);

SDL_Texture* getBobberTexture(void);

float easeOutCubic(float x);
float easeInCubic(float x);
float easeOutCirc(float x);
int lerpReal(int a, int b, float t);

#endif /* ITEMHELPERS_H */
