#include "ItemHelpers.h"

#include "FishItem.h"
#include "FishingRodItem.h"
#include "Game.h"
#include "Graphics.h"
#include "Entity.h"
#include "ItemStack.h"
#include "Item.h"
#include "ItemDefinitions.h"
#include "Maths.h"
#include "Music.h"
#include "UtilityClass.h"
#include "BucketItem.h"
#include "FilledBucketItem.h"
#include "TrinketFinFishItem.h"
#include "BobberItem.h"

namespace Items
{
    Item* FISHING_ROD;
    Item* WORMS;
    Item* LARGEMOUTH_BASS;
    Item* SMALLMOUTH_BASS;
    Item* BULLHEAD;
    Item* CHUB;
    Item* PIKE;
    Item* WALLEYE;
    Item* CARP;
    Item* PERCH;
    Item* CATFISH;
    Item* MINNOW;
    Item* KOI_A;
    Item* KOI_B;
    Item* KOI_C;
    Item* RAINBOW_TROUT;
    Item* GOLDEN_TROUT;
    Item* GOLDFISH;
    Item* GOLD_GOLDFISH;
    Item* PRISMATIC_TROUT;
    Item* SPIKE_FISH;
    Item* TRINKETFIN;
    Item* YESFIN;
    Item* VIRIDIFIN;
    Item* EDGEFISH;
    Item* TERMINNOW;
    Item* TIRE;

    Item* RED_SNAPPER;
    Item* TUNA;
    Item* BLUE_MARLIN;
    Item* AMBERJACK;
    Item* COALFISH;
    Item* WAHOO;
    Item* ANCHOVY;
    Item* SARDINE;
    Item* POMPANO;
    Item* PUFFERFISH;
    Item* MAHIMAHI;
    Item* MORAY_EEL;
    Item* RIBBON_EEL;
    Item* SQUID;
    Item* OCTOPUS;

    Item* STAR_FISH;
    Item* FISHTRONAUT;
    
    Item* NAVAL_MINE;
    Item* OLD_BOOT;
    Item* SODA_CAN;
    Item* TIN_CAN;
    Item* CAR_BATTERY;
    Item* GAME_CARTRIDGE;
    Item* DRIFTWOOD;
    Item* SEA_GLASS;

    Item* THIRTY_SEVEN_FISH;
    Item* SQUISHFISH;

    Item* YELLOW_KEY_FAKE;
    Item* YELLOW_KEY;
    Item* BLUE_KEY;
    Item* PURPLE_KEY;
    Item* RED_KEY;

    Item* BUCKET;
    Item* FRESHWATER_BUCKET;
    Item* SALTWATER_BUCKET;
    Item* JUNKWATER_BUCKET;
    Item* GREENWATER_BUCKET;
    Item* BLUEWATER_BUCKET;
    Item* PURPLEWATER_BUCKET;

    Item* ENHANCED_BAIT;
    Item* DELUXE_BAIT;
    Item* ULTRA_BAIT;

    Item* GREEN_BOBBER;
    Item* BLUE_BOBBER;
    Item* PURPLE_BOBBER;
    Item* BIG_BOBBER;
    Item* FISH_SPINNER;
    Item* FEATHER_SPINNER;
    Item* COIN_SPINNER;
    Item* TRINKET_SPINNER;
    Item* VIRIDIAN_SPINNER;
    Item* GIANT_BOBBER;
}

bool hasItem(Item* item)
{
    for (auto& inv_item : game.inventory)
    {
        if (inv_item.item == item)
        {
            return true;
        }
    }
    return false;
}

void giveItem(ItemStack stack)
{
    giveItem(stack, true);
}

void giveItem(ItemStack stack, bool display)
{
    if (stack.item == NULL) {
        // dont give null stacks...
        return;
    }

    if (display)
    {
        game.play_item_get = true;

        ItemGetDisplay display;
        display.stack = stack;
        display.timer = 0;
        game.item_get_displays.push_back(display);
    }

    // okay first... i guess let's just check if we have any matching stacks.
    for (auto& inv_stack : game.inventory)
    {
        if (stack.item == inv_stack.item)
        {
            // they're the same item at least, so... check components? i guess?
            bool components_match = true;

            if (stack.fish_size != inv_stack.fish_size) components_match = false;

            if (components_match)
            {
                inv_stack.increment(stack.count);
                return;
            }
        }
    }
    game.inventory.push_back(stack);
}

static Item* registerItem(std::string id, Item* item)
{
    if (item == NULL)
    {
        SDL_assert(false && "Attempt to register null item");
        return NULL;
    }
    if (ITEM_REGISTRY.count(id) > 0)
    {
        SDL_assert(false && "Item ID already registered");
        return NULL;
    }
    ITEM_REGISTRY[id] = item;
    return item;
}

void cleanItems(void)
{
    for (auto& item : ITEM_REGISTRY)
    {
        delete item.second;
    }
    ITEM_REGISTRY.clear();
}

Item* getItem(std::string id)
{
    if (ITEM_REGISTRY.count(id) == 0)
    {
        return NULL;
    }
    return ITEM_REGISTRY[id];
}

std::string getItemID(Item* item)
{
    for (auto& entry : ITEM_REGISTRY)
    {
        if (entry.second == item)
        {
            return entry.first;
        }
    }
    return "null";
}

void updateFishCaughtInfo(void)
{
    Item* item = game.fishing_item.item;

    if (item == Items::KOI_B) item = Items::KOI_A;
    if (item == Items::KOI_C) item = Items::KOI_A;

    for (int i = 0; i < game.fish_catch_info.size(); i++)
    {
        FishCatchInfo& info = game.fish_catch_info[i];
        if (info.item == item)
        {
            info.smallest = SDL_min(info.smallest, game.fishing_item.fish_size);
            info.largest = SDL_max(info.largest, game.fishing_item.fish_size);
            info.amount += game.fishing_item.count;
            return;
        }
    }

    FishCatchInfo info;
    info.item = item;
    info.smallest = game.fishing_item.fish_size;
    info.largest = game.fishing_item.fish_size;
    info.amount = game.fishing_item.count;
    game.fish_catch_info.push_back(info);
}

bool canUpgradeBait(void)
{
    for (auto& inv_item : game.inventory)
    {
        bool bait_upgrade_3 = obj.flags[9];
        bool bait_upgrade_2 = obj.flags[8] || bait_upgrade_3;
        bool bait_upgrade_1 = obj.flags[7] || bait_upgrade_2 || bait_upgrade_3;

        if (inv_item.item == Items::WORMS && bait_upgrade_1) return true;
        if (inv_item.item == Items::ENHANCED_BAIT && bait_upgrade_2) return true;
        if (inv_item.item == Items::DELUXE_BAIT && bait_upgrade_3) return true;
    }
    return false;
}

void upgradeBait(void)
{
    for (auto& inv_item : game.inventory)
    {
        bool bait_upgrade_3 = obj.flags[9];
        bool bait_upgrade_2 = obj.flags[8] || bait_upgrade_3;
        bool bait_upgrade_1 = obj.flags[7] || bait_upgrade_2 || bait_upgrade_3;

        if (inv_item.item == Items::WORMS && bait_upgrade_1)
        {
            inv_item.item = Items::ENHANCED_BAIT;
        }
        if (inv_item.item == Items::ENHANCED_BAIT && bait_upgrade_2)
        {
            inv_item.item = Items::DELUXE_BAIT;
        }
        if (inv_item.item == Items::DELUXE_BAIT && bait_upgrade_3)
        {
            inv_item.item = Items::ULTRA_BAIT;
        }
    }

    // ok now we have to forcibly stack the bait
    std::vector<ItemStack> old_inv = game.inventory;
    game.inventory.clear();
    for (int i = 0; i < old_inv.size(); i++)
    {
        giveItem(old_inv[i], false);
    }
}

bool hasBait(void)
{
    for (auto& inv_item : game.inventory)
    {
        if (inv_item.item == Items::WORMS) return true;
        if (inv_item.item == Items::ENHANCED_BAIT) return true;
        if (inv_item.item == Items::DELUXE_BAIT) return true;
        if (inv_item.item == Items::ULTRA_BAIT) return true;
    }
    return false;
}

void removeEmptyInventorySlots(void)
{
    for (int i = game.inventory.size() - 1; i >= 0; i--)
    {
        if (game.inventory[i].count <= 0 || game.inventory[i].item == nullptr)
        {
            game.inventory.erase(game.inventory.begin() + i);
        }
    }
}

void useBait(void)
{
    // use the best bait. im crunching rn man just make it work
    for (auto& inv_item : game.inventory)
    {
        if (inv_item.item == Items::ULTRA_BAIT)
        {
            inv_item.decrement();
            removeEmptyInventorySlots();
            return;
        }
    }
    for (auto& inv_item : game.inventory)
    {
        if (inv_item.item == Items::DELUXE_BAIT)
        {
            inv_item.decrement();
            removeEmptyInventorySlots();
            return;
        }
    }
    for (auto& inv_item : game.inventory)
    {
        if (inv_item.item == Items::ENHANCED_BAIT)
        {
            inv_item.decrement();
            removeEmptyInventorySlots();
            return;
        }
    }
    for (auto& inv_item : game.inventory)
    {
        if (inv_item.item == Items::WORMS)
        {
            inv_item.decrement();
            removeEmptyInventorySlots();
            return;
        }
    }
}

int getBaitTier(void)
{
    // use the best bait. im crunching rn man just make it work
    for (auto& inv_item : game.inventory)
    {
        if (inv_item.item == Items::ULTRA_BAIT) return 3;
    }
    for (auto& inv_item : game.inventory)
    {
        if (inv_item.item == Items::DELUXE_BAIT) return 2;
    }
    for (auto& inv_item : game.inventory)
    {
        if (inv_item.item == Items::ENHANCED_BAIT) return 1;
    }
    for (auto& inv_item : game.inventory)
    {
        if (inv_item.item == Items::WORMS) return 0;
    }
    return -1;
}

static void toPool(std::string pool, Item* item, int weight)
{
    if (item == NULL)
    {
        SDL_assert(false && "Attempt to add null item to pool");
    }
    if (!POOLS.count(pool))
    {
        POOLS[pool] = std::vector<PoolData>();
    }
    PoolData poolData;
    poolData.item = item;
    poolData.weight = weight;
    POOLS[pool].push_back(poolData);
}

void registerItems(void)
{
    Items::FISHING_ROD = registerItem("fishing_rod", new FishingRodItem(ItemSettings().withName("Fishing Rod").withDescription("A basic fishing rod.").withLayer("fishing_rod_basic", 200).withLayer("fishing_line", 201)));
    Items::WORMS = registerItem("worms", new Item(ItemSettings().withName("Worms").withBuy(5).withDescription("A perfect bait for beginners. Notably not a fish.").withLayer("worm_layer_1", 204).withLayer("worm_layer_2", 205)));
    Items::LARGEMOUTH_BASS = registerItem("largemouth_bass", new FishItem(ItemSettings().withName("Largemouth Bass").withHabitat(Habitat_FRESHWATER).withSell(22).withDescription("A carnivorous, freshwater member of the sunfish family.").withLayer("largemouth_bass_layer_1", 202).withLayer("largemouth_bass_layer_2", 203), 30, 40, 75));
    Items::SMALLMOUTH_BASS = registerItem("smallmouth_bass", new FishItem(ItemSettings().withName("Smallmouth Bass").withHabitat(Habitat_FRESHWATER).withSell(30).withDescription("An adaptive freshwater fish known to be feistier than other bass.").withRarity(Rarity_UNCOMMON).withLayer("smallmouth_bass_layer_1", 206).withLayer("smallmouth_bass_layer_2", 207), 22, 27, 65));
    Items::BULLHEAD = registerItem("bullhead", new FishItem(ItemSettings().withName("Bullhead").withHabitat(Habitat_FRESHWATER).withSell(20).withDescription("This bottom dwelling scavenger will eat almost anything.").withLayer("bullhead_layer_1", 208).withLayer("bullhead_layer_2", 209), 18, 23, 60));
    Items::CHUB = registerItem("chub", new FishItem(ItemSettings().withName("Chub").withHabitat(Habitat_FRESHWATER).withSell(32).withDescription("A voracious eater, it will even eat fruit off trees above the water.").withRarity(Rarity_UNCOMMON).withLayer("chub_layer_1", 210).withLayer("chub_layer_2", 211), 15, 30, 60));
    Items::PIKE = registerItem("pike", new FishItem(ItemSettings().withName("Pike").withHabitat(Habitat_FRESHWATER).withSell(70).withDescription("This large species is aggressive and quick when chasing prey.").withRarity(Rarity_RARE).withLayer("pike_layer_1", 212).withLayer("pike_layer_2", 213), 40, 55, 150));
    Items::WALLEYE = registerItem("walleye", new FishItem(ItemSettings().withName("Walleye").withHabitat(Habitat_FRESHWATER).withSell(65).withDescription("Its unique pearlescent eyes give this fish powerful night vision.").withRarity(Rarity_RARE).withLayer("walleye_layer_1", 214).withLayer("walleye_layer_2", 215), 36, 54, 107));
    Items::CARP = registerItem("carp", new FishItem(ItemSettings().withName("Carp").withHabitat(Habitat_FRESHWATER).withSell(18).withDescription("This common freshwater fish is highly tolerant of turbid waters.").withLayer("carp_layer_1", 216).withLayer("carp_layer_2", 217), 25, 31, 120));
    Items::PERCH = registerItem("perch", new FishItem(ItemSettings().withName("Perch").withHabitat(Habitat_FRESHWATER).withSell(35).withDescription("This yellow fish is a critical food source for bass and walleye.").withRarity(Rarity_UNCOMMON).withLayer("perch_layer_1", 218).withLayer("perch_layer_2", 219), 10, 19, 50));
    Items::CATFISH = registerItem("catfish", new FishItem(ItemSettings().withName("Catfish").withHabitat(Habitat_FRESHWATER).withSell(65).withDescription("These whiskered fish use pheremones to communicate. They have a social hierarchy.").withRarity(Rarity_RARE).withLayer("catfish_layer_1", 220).withLayer("catfish_layer_2", 221), 43, 57, 132));
    Items::MINNOW = registerItem("minnow", new FishItem(ItemSettings().withName("Minnow").withHabitat(Habitat_FRESHWATER).withSell(30).withDescription("These little fish travel in large shoals to avoid predators.").withRarity(Rarity_UNCOMMON).withLayer("minnow_layer_1", 222).withLayer("minnow_layer_2", 223), 5, 7, 14));
    Items::KOI_A = registerItem("koi_a", new FishItem(ItemSettings().withName("Koi").withHabitat(Habitat_FRESHWATER).withSell(60).withDescription("A domesticated variety of carp with beautiful colored scales.").withRarity(Rarity_RARE).withLayer("koi_1_layer_1", 224).withLayer("koi_1_layer_2", 225), 25, 31, 60));
    Items::KOI_B = registerItem("koi_b", new FishItem(ItemSettings().withName("Koi").withHabitat(Habitat_FRESHWATER).withSell(60).withDescription("A domesticated variety of carp with beautiful colored scales.").withRarity(Rarity_RARE).withLayer("koi_2_layer_1", 224).withLayer("koi_2_layer_2", 225), 25, 31, 60));
    Items::KOI_C = registerItem("koi_c", new FishItem(ItemSettings().withName("Koi").withHabitat(Habitat_FRESHWATER).withSell(60).withDescription("A domesticated variety of carp with beautiful colored scales.").withRarity(Rarity_RARE).withLayer("koi_3_layer_1", 224).withLayer("koi_3_layer_2", 225), 25, 31, 60));
    Items::RAINBOW_TROUT = registerItem("rainbow_trout", new FishItem(ItemSettings().withName("Rainbow Trout").withHabitat(Habitat_FRESHWATER).withSell(75).withDescription("This multicolored fish is known for leaping out of the water.").withRarity(Rarity_RARE).withLayer("rainbow_trout_layer_1", 226).withLayer("rainbow_trout_layer_2", 227).withLayer("rainbow_trout_layer_3", 228).withLayer("rainbow_trout_layer_4", 229), 30, 60, 122));
    Items::GOLDEN_TROUT = registerItem("golden_trout", new FishItem(ItemSettings().withName("Golden Trout").withHabitat(Habitat_FRESHWATER).withSell(100).withDescription("This beautiful golden trout is out-competed by other species.").withRarity(Rarity_ELUSIVE).withLayer("golden_trout_layer_1", 230).withLayer("golden_trout_layer_2", 231).withLayer("golden_trout_layer_3", 232), 15, 30, 71));
    Items::GOLDFISH = registerItem("goldfish", new FishItem(ItemSettings().withName("Goldfish").withHabitat(Habitat_FRESHWATER).withSell(50).withDescription("Commonly kept as a pet. It gets bored easily without accessories.").withRarity(Rarity_RARE).withLayer("goldfish_layer_1", 233).withLayer("goldfish_layer_2", 234), 11, 15, 41));
    Items::GOLD_GOLDFISH = registerItem("gold_goldfish", new FishItem(ItemSettings().withName("Gold Goldfish").withHabitat(Habitat_FRESHWATER).withSell(200).withDescription("Its scales are made of gold leaf. It is sensitive to temperature.").withRarity(Rarity_LEGENDARY).withLayer("gold_goldfish_layer_1", 235).withLayer("gold_goldfish_layer_2", 236).withLayer("gold_goldfish_layer_3", 237).withLayer("gold_goldfish_layer_4", 238).withLayer("gold_goldfish_layer_5", 239).withLayer("gold_goldfish_layer_6", 240), 11, 15, 41));
    Items::PRISMATIC_TROUT = registerItem("prismatic_trout", new FishItem(ItemSettings().withName("Prismatic Trout").withHabitat(Habitat_FRESHWATER).withSell(200).withDescription("The mesmerizing scales of this fish can match any colour.").withRarity(Rarity_LEGENDARY).withNameColor(241).withLayer("prismatic_trout", 241), 30, 60, 100));
    Items::YESFIN = registerItem("yesfin", new FishItem(ItemSettings().withName("Yesfin").withHabitat(Habitat_FRESHWATER).withSell(100).withDescription("With a natural briefcase, this fish is perfect for the workplace.").withRarity(Rarity_ELUSIVE).withLayer("yesfin", 9), 20, 30, 70));
    Items::VIRIDIFIN = registerItem("viridifin", new FishItem(ItemSettings().withName("Viridifin").withHabitat(Habitat_FRESHWATER).withSell(222).withDescription("It has an uncanny resemblance to you, to say the least...").withRarity(Rarity_LEGENDARY).withLayer("viridifin", 0), 40, 60, 100));
    Items::EDGEFISH = registerItem("edgefish", new FishItem(ItemSettings().withName("Edgefish").withHabitat(Habitat_FRESHWATER).withSell(190).withDescription("This fish subsists on pellets. No trademark infringement intended.").withRarity(Rarity_LEGENDARY).withLayer("edgefish", 8), 15, 30, 60));
    Items::TERMINNOW = registerItem("terminnow", new FishItem(ItemSettings().withName("Terminnow").withHabitat(Habitat_FRESHWATER).withSell(200).withDescription("      -= PERSONAL LOG =-      \nI can't believe this fish even has Wi-Fi.").withRarity(Rarity_LEGENDARY).withLayer("terminnow", 4), 10, 15, 20));
    Items::SPIKE_FISH = registerItem("spike_fish", new FishItem(ItemSettings().withName("Spike Fish").withHabitat(Habitat_ANYWHERE).withSell(15).withDescription("They leave behind spiky corpses when they die. An invasive species found everywhere.").withLayer("spike_fish", -1), 40, 40, 40));
    Items::TRINKETFIN = registerItem("trinketfin", new TrinketFinFishItem(ItemSettings().withName("Trinketfin").withHabitat(Habitat_ANYWHERE).withDescription("Congratulations! You have found a shiny trinketfin!").withRarity(Rarity_RARE).withLayer("trinketfin", 3)));

    Items::RED_SNAPPER = registerItem("red_snapper", new FishItem(ItemSettings().withName("Red Snapper").withSell(20).withHabitat(Habitat_SALTWATER).withDescription("A sociable fish, it will even school with other species.").withLayer("red_snapper_layer_1", 242).withLayer("red_snapper_layer_2", 243), 39, 60, 100));
    Items::TUNA = registerItem("tuna", new FishItem(ItemSettings().withName("Tuna").withSell(40).withHabitat(Habitat_SALTWATER).withDescription("This extremely popular fish can grow to huge sizes.").withRarity(Rarity_UNCOMMON).withLayer("tuna_layer_1", 244).withLayer("tuna_layer_2", 245).withLayer("tuna_layer_3", 246), 97, 200, 458));
    Items::BLUE_MARLIN = registerItem("blue_marlin", new FishItem(ItemSettings().withName("Blue Marlin").withSell(80).withHabitat(Habitat_SALTWATER).withDescription("A highly prized catch with a long bill. Large ones are called \"granders\".").withRarity(Rarity_RARE).withLayer("blue_marlin_layer_1", 247).withLayer("blue_marlin_layer_2", 248), 101, 290, 500));
    Items::AMBERJACK = registerItem("amberjack", new FishItem(ItemSettings().withName("Amberjack").withSell(20).withHabitat(Habitat_SALTWATER).withDescription("A sea predator that likes to live in kelp forests and shipwrecks.").withLayer("amberjack_layer_1", 249).withLayer("amberjack_layer_2", 250).withLayer("amberjack_layer_3", 251), 88, 100, 190));
    Items::COALFISH = registerItem("coalfish", new FishItem(ItemSettings().withName("Coalfish").withSell(20).withHabitat(Habitat_SALTWATER).withDescription("Also known as a coley, pollock, or saithe. It has a lot of names.").withLayer("coalfish_layer_1", 252).withLayer("coalfish_layer_2", 253), 39, 60, 130));
    Items::WAHOO = registerItem("wahoo", new FishItem(ItemSettings().withName("Wahoo").withSell(35).withHabitat(Habitat_SALTWATER).withDescription("This active fish feeds on smaller fish and squid. Wahoo!").withRarity(Rarity_UNCOMMON).withLayer("wahoo_layer_1", 254).withLayer("wahoo_layer_2", 255), 99, 170, 250));
    Items::ANCHOVY = registerItem("anchovy", new FishItem(ItemSettings().withName("Anchovy").withSell(15).withHabitat(Habitat_SALTWATER).withDescription("A small, highly abundant species. They move in gigantic schools.").withLayer("anchovy_layer_1", 256).withLayer("anchovy_layer_2", 257), 9, 13, 20));
    Items::SARDINE = registerItem("sardine", new FishItem(ItemSettings().withName("Sardine").withSell(15).withHabitat(Habitat_SALTWATER).withDescription("A popular commercial catch. It feeds on surface plankton at night.").withLayer("sardine_layer_1", 258).withLayer("sardine_layer_2", 259), 13, 17, 28));
    Items::POMPANO = registerItem("pompano", new FishItem(ItemSettings().withName("Pompano").withSell(35).withHabitat(Habitat_SALTWATER).withDescription("These fast swimmers enjoy shallow waters and surf flats.").withRarity(Rarity_UNCOMMON).withLayer("pompano_layer_1", 260).withLayer("pompano_layer_2", 261).withLayer("pompano_layer_3", 262), 25, 40, 64));
    Items::PUFFERFISH = registerItem("pufferfish", new FishItem(ItemSettings().withName("Pufferfish").withSell(115).withHabitat(Habitat_SALTWATER).withDescription("This fish puffs up when threatened! Its spines protect it from predators.").withRarity(Rarity_ELUSIVE).withLayer("pufferfish_layer_1", 263).withLayer("pufferfish_layer_2", 264), 15, 30, 50));
    Items::MAHIMAHI = registerItem("mahimahi", new FishItem(ItemSettings().withName("Mahi-mahi").withSell(65).withHabitat(Habitat_SALTWATER).withDescription("Highly prized for its bright colours and size. It has a distinctive square 'forehead'.").withRarity(Rarity_RARE).withLayer("mahi-mahi_layer_1", 265).withLayer("mahi-mahi_layer_2", 266).withLayer("mahi-mahi_layer_3", 267), 65, 100, 210));
    Items::MORAY_EEL = registerItem("moray_eel", new FishItem(ItemSettings().withName("Moray Eel").withSell(70).withHabitat(Habitat_SALTWATER).withDescription("If it swims in a reef and has sharp pointy teeth, that's a moray!").withRarity(Rarity_RARE).withLayer("moray_eel_layer_1", 268).withLayer("moray_eel_layer_2", 269), 69, 80, 150));
    Items::RIBBON_EEL = registerItem("ribbon_eel", new FishItem(ItemSettings().withName("Ribbon Eel").withSell(70).withHabitat(Habitat_SALTWATER).withDescription("This eel is known for its long, thin body and vibrant colours.").withRarity(Rarity_RARE).withLayer("ribbon_eel_layer_1", 270).withLayer("ribbon_eel_layer_2", 271), 95, 100, 130));
    Items::SQUID = registerItem("squid", new FishItem(ItemSettings().withName("Squid").withSell(95).withHabitat(Habitat_SALTWATER).withDescription("This little mollusk can change its colour in order to camouflage.").withRarity(Rarity_ELUSIVE).withLayer("squid_layer_1", 272).withLayer("squid_layer_2", 273), 13, 17, 28));
    Items::OCTOPUS = registerItem("octopus", new FishItem(ItemSettings().withName("Octopus").withSell(120).withHabitat(Habitat_SALTWATER).withDescription("A highly intelligent mollusk. They're known to keep 'gardens' of marine plants!").withRarity(Rarity_ELUSIVE).withLayer("octopus_layer_1", 274).withLayer("octopus_layer_2", 275), 90, 110, 150));

    Items::STAR_FISH = registerItem("star_fish", new FishItem(ItemSettings().withName("Star Fish").withSell(100).withHabitat(Habitat_SALTWATER).withDescription("This is a star fish, not a starfish.").withRarity(Rarity_ELUSIVE).withLayer("star_fish", 276), 80, 100, 120));
    Items::FISHTRONAUT = registerItem("fishtronaut", new FishItem(ItemSettings().withName("Fishtronaut").withSell(269).withHabitat(Habitat_SALTWATER).withDescription("One small step for fish, one giant leap for fishkind!").withRarity(Rarity_LEGENDARY).withLayer("fishtronaut_layer_1", 277).withLayer("fishtronaut_layer_2", 278).withLayer("fishtronaut_layer_3", 279).withLayer("fishtronaut_layer_4", 280).withLayer("fishtronaut_layer_5", 281).withLayer("fishtronaut_layer_6", 282).withLayer("fishtronaut_layer_7", 283), 25, 30, 70));

    Items::THIRTY_SEVEN_FISH = registerItem("thirty_seven_fish", new FishItem(ItemSettings().withName("37 Fish").withHabitat(Habitat_SALTWATER).withSell(237).withDescription("Do you remember how many fish you've caught?").withRarity(Rarity_LEGENDARY).withLayer("37_fish", 1), 15, 20, 35));
    Items::SQUISHFISH = registerItem("squishfish", new FishItem(ItemSettings().withName("Squishfish").withHabitat(Habitat_SALTWATER).withSell(203).withDescription("Rumor has it that eating this fish grants special power to those who believe.").withRarity(Rarity_LEGENDARY).withLayer("squishfish_layer_1", 286).withLayer("squishfish_layer_2", 287).withLayer("squishfish_layer_3", 288), 80, 140, 180));

    // keys
    Items::YELLOW_KEY_FAKE = registerItem("yellow_key_fake", new Item(ItemSettings().withName("Metallic Object").withDescription("A mysterious metal object. It has a tag which says \"key\" on it.").withLayer("key", 26)));
    Items::YELLOW_KEY = registerItem("yellow_key", new Item(ItemSettings().withName("Yellow Key").withDescription("A \"key\" which opens yellow gates.").withLayer("key", 26)));
    Items::BLUE_KEY = registerItem("blue_key", new Item(ItemSettings().withName("Blue Key").withBuy(1000).withDescription("A \"key\" which opens blue gates.").withLayer("key", 300)));
    Items::PURPLE_KEY = registerItem("purple_key", new Item(ItemSettings().withName("Purple Key").withBuy(1500).withDescription("A \"key\" which opens purple gates.").withLayer("key", 301)));
    Items::RED_KEY = registerItem("red_key", new Item(ItemSettings().withName("Red Key").withBuy(1000).withDescription("A \"key\" which opens red gates.").withLayer("key", 302)));

    // buckets
    Items::BUCKET = registerItem("bucket", new BucketItem(ItemSettings().withName("Bucket (Empty)").withDescription("It's a bucket. You cannot wear it on your head.").withLayer("bucket", 303)));
    Items::FRESHWATER_BUCKET = registerItem("freshwater_bucket", new FilledBucketItem(ItemSettings().withName("Bucket (Filled)").withDescription("It's a bucket filled with fresh water.").withLayer("bucket", 303).withLayer("bucket_fill",304)));
    Items::SALTWATER_BUCKET = registerItem("saltwater_bucket", new FilledBucketItem(ItemSettings().withName("Bucket (Filled)").withDescription("It's a bucket filled with salt water.").withLayer("bucket", 303).withLayer("bucket_fill",305)));
    Items::JUNKWATER_BUCKET = registerItem("junkwater_bucket", new FilledBucketItem(ItemSettings().withName("Bucket (Filled)").withDescription("It's a bucket filled with... gross water.").withLayer("bucket", 303).withLayer("bucket_fill",306)));
    Items::GREENWATER_BUCKET = registerItem("greenwater_bucket", new FilledBucketItem(ItemSettings().withName("Bucket (Filled)").withDescription("It's a bucket filled with green chemicals.").withLayer("bucket", 303).withLayer("bucket_fill",307)));
    Items::BLUEWATER_BUCKET = registerItem("bluewater_bucket", new FilledBucketItem(ItemSettings().withName("Bucket (Filled)").withDescription("It's a bucket filled with blue chemicals.").withLayer("bucket", 303).withLayer("bucket_fill",308)));
    Items::PURPLEWATER_BUCKET = registerItem("purplewater_bucket", new FilledBucketItem(ItemSettings().withName("Bucket (Filled)").withDescription("It's a bucket filled with purple chemicals.").withLayer("bucket", 303).withLayer("bucket_fill",309)));

    // bait
    Items::ENHANCED_BAIT = registerItem("enhanced_bait", new Item(ItemSettings().withName("Enhanced Bait").withDescription("Upgraded by Vitellary using the power of science. Still not a fish.").withLayer("worm_layer_1", 310).withLayer("worm_layer_2", 311)));
    Items::DELUXE_BAIT = registerItem("deluxe_bait", new Item(ItemSettings().withName("Deluxe Bait").withDescription("Upgraded even further. Has science gone too far? Not a fish.").withLayer("worm_layer_1", 312).withLayer("worm_layer_2", 313)));
    Items::ULTRA_BAIT = registerItem("ultra_bait", new Item(ItemSettings().withName("Ultra Bait").withDescription("It's a good thing this wasn't called something else.").withLayer("worm_layer_1", 314).withLayer("worm_layer_2", 315)));

    // bobbers
    Items::GREEN_BOBBER = registerItem("green_bobber", new BobberItem(ItemSettings().withName("Green Bobber").withDescription("Equip this to change your bobber to a green one.").withBuy(250).withLayer("bobber_green", -1)));
    Items::BLUE_BOBBER = registerItem("blue_bobber", new BobberItem(ItemSettings().withName("Blue Bobber").withDescription("Equip this to change your bobber to a blue one.").withBuy(250).withLayer("bobber_blue", -1)));
    Items::PURPLE_BOBBER = registerItem("purple_bobber", new BobberItem(ItemSettings().withName("Purple Bobber").withDescription("Equip this to change your bobber to a purple one.").withBuy(250).withLayer("bobber_purple", -1)));
    Items::BIG_BOBBER = registerItem("big_bobber", new BobberItem(ItemSettings().withName("Big Bobber").withDescription("Equip this to change your bobber to a big one.").withBuy(300).withLayer("bobber_big", -1)));
    Items::FISH_SPINNER = registerItem("fish_spinner", new BobberItem(ItemSettings().withName("Fish Spinner").withDescription("Equip this to change your bobber to a fish-shaped one.").withBuy(500).withLayer("bobber_fish", -1)));
    Items::FEATHER_SPINNER = registerItem("feather_spinner", new BobberItem(ItemSettings().withName("Feather Spinner").withDescription("Equip this to change your bobber to a fly fishing lure.").withBuy(500).withLayer("bobber_feather", -1)));
    Items::COIN_SPINNER = registerItem("coin_spinner", new BobberItem(ItemSettings().withName("Coin Spinner").withDescription("Equip this to change your bobber to a coin-shaped one.").withBuy(750).withLayer("bobber_coin", -1)));
    Items::TRINKET_SPINNER = registerItem("trinket_spinner", new BobberItem(ItemSettings().withName("Trinket Spinner").withDescription("Equip this to change your bobber to a trinket-shaped one.").withBuy(750).withLayer("bobber_trinket", -1)));
    Items::VIRIDIAN_SPINNER = registerItem("viridian_spinner", new BobberItem(ItemSettings().withName("Viridian Spinner").withDescription("Equip this to change your bobber to one shaped like you.").withBuy(750).withLayer("bobber_viridian", -1)));
    Items::GIANT_BOBBER = registerItem("giant_bobber", new BobberItem(ItemSettings().withName("Giant Bobber").withDescription("Equip this to change your bobber to a REALLY big one.").withBuy(1000).withLayer("bobber_giant", -1)));

    // junk
    Items::TIRE = registerItem("tire", new Item(ItemSettings().withName("Tire").withSell(5).withDescription("This tire could be from the 3099 Space Derby Grand Prix.").withRarity(Rarity_JUNK).withLayer("tire", 19)));
    Items::NAVAL_MINE = registerItem("naval_mine", new Item(ItemSettings().withName("Naval Mine").withSell(25).withDescription("Uh oh.").withRarity(Rarity_JUNK).withLayer("naval_mine_layer_1", 284).withLayer("naval_mine_layer_2", 285)));
    Items::OLD_BOOT = registerItem("old_boot", new Item(ItemSettings().withName("Old Boot").withSell(5).withDescription("This boot is soaked ALL the way through.").withRarity(Rarity_JUNK).withLayer("boot", -1)));
    Items::SODA_CAN = registerItem("soda_can", new Item(ItemSettings().withName("Soda Can").withSell(5).withDescription("Probably not the best idea to drink the water in this...").withRarity(Rarity_JUNK).withLayer("soda_can", -1)));
    Items::TIN_CAN = registerItem("tin_can", new Item(ItemSettings().withName("Tin Can").withSell(5).withDescription("In surprisingly decent shape. It says, \"MINMO CAT FOODS\".").withRarity(Rarity_JUNK).withLayer("tin_can", -1)));
    Items::CAR_BATTERY = registerItem("car_battery", new Item(ItemSettings().withName("Car Battery").withSell(15).withDescription("This may have been tossed in the ocean in a misguided attempt to \"recharge eels\".").withRarity(Rarity_JUNK).withLayer("car_battery", -1)));
    Items::GAME_CARTRIDGE = registerItem("game_cartridge", new Item(ItemSettings().withName("Game Cartridge").withSell(5).withDescription("A cartridge for a retro game console. No way this still works.").withRarity(Rarity_JUNK).withLayer("cartridge", -1)));
    Items::DRIFTWOOD = registerItem("driftwood", new Item(ItemSettings().withName("Driftwood").withSell(10).withDescription("This branch has been bleached by months at sea.").withRarity(Rarity_JUNK).withLayer("driftwood", -1)));
    Items::SEA_GLASS = registerItem("sea_glass", new Item(ItemSettings().withName("Sea Glass").withSell(10).withDescription("These pieces of glass have been smoothed by years in the sea.").withRarity(Rarity_JUNK).withLayer("sea_glass", -1)));

    toPool("freshwater_small", Items::LARGEMOUTH_BASS, 37);
    toPool("freshwater_small", Items::SMALLMOUTH_BASS, 31);
    toPool("freshwater_small", Items::BULLHEAD, 43);
    toPool("freshwater_small", Items::CHUB, 31);
    toPool("freshwater_small", Items::CARP, 52);
    toPool("freshwater_small", Items::PERCH, 34);
    toPool("freshwater_small", Items::MINNOW, 31);
    toPool("freshwater_small", Items::SPIKE_FISH, 31);
    toPool("freshwater_small", Items::TIRE, 15);
    toPool("freshwater_small", Items::GOLDFISH, 16);
    toPool("freshwater_small", Items::KOI_A, 4);
    toPool("freshwater_small", Items::KOI_B, 4);
    toPool("freshwater_small", Items::KOI_C, 4);
    toPool("freshwater_small", Items::OLD_BOOT, 15);
    toPool("freshwater_small", Items::TIN_CAN, 15);
    toPool("freshwater_small", Items::SODA_CAN, 15);
    toPool("freshwater_small", Items::GAME_CARTRIDGE, 5);

    toPool("freshwater_large", Items::LARGEMOUTH_BASS, 18);
    toPool("freshwater_large", Items::SMALLMOUTH_BASS, 15);
    toPool("freshwater_large", Items::BULLHEAD, 18);
    toPool("freshwater_large", Items::CHUB, 15);
    toPool("freshwater_large", Items::PIKE, 12);
    toPool("freshwater_large", Items::WALLEYE, 11);
    toPool("freshwater_large", Items::CARP, 18);
    toPool("freshwater_large", Items::PERCH, 16);
    toPool("freshwater_large", Items::CATFISH, 12);
    toPool("freshwater_large", Items::MINNOW, 15);
    toPool("freshwater_large", Items::KOI_A, 4);
    toPool("freshwater_large", Items::KOI_B, 4);
    toPool("freshwater_large", Items::KOI_C, 4);
    toPool("freshwater_large", Items::RAINBOW_TROUT, 12);
    toPool("freshwater_large", Items::GOLDEN_TROUT, 10);
    toPool("freshwater_large", Items::GOLDFISH, 12);
    toPool("freshwater_large", Items::GOLD_GOLDFISH, 9);
    toPool("freshwater_large", Items::PRISMATIC_TROUT, 9);
    toPool("freshwater_large", Items::SPIKE_FISH, 15);
    toPool("freshwater_large", Items::TRINKETFIN, 9);
    toPool("freshwater_large", Items::YESFIN, 10);
    toPool("freshwater_large", Items::VIRIDIFIN, 9);
    toPool("freshwater_large", Items::TIRE, 4);
    toPool("freshwater_large", Items::EDGEFISH, 1);
    toPool("freshwater_large", Items::TERMINNOW, 1);
    toPool("freshwater_large", Items::OLD_BOOT,4);
    toPool("freshwater_large", Items::TIN_CAN,4);
    toPool("freshwater_large", Items::SODA_CAN,4);
    toPool("freshwater_large", Items::GAME_CARTRIDGE,2);

    toPool("junk", Items::CARP, 10);
    toPool("junk", Items::GOLDFISH, 1);
    toPool("junk", Items::TIRE, 25);
    toPool("junk", Items::OLD_BOOT, 25);
    toPool("junk", Items::TIN_CAN, 25);
    toPool("junk", Items::SODA_CAN, 25);
    toPool("junk", Items::GAME_CARTRIDGE, 15);

    toPool("saltwater_small", Items::RED_SNAPPER, 19);
    toPool("saltwater_small", Items::AMBERJACK, 19);
    toPool("saltwater_small", Items::COALFISH, 19);
    toPool("saltwater_small", Items::ANCHOVY, 24);
    toPool("saltwater_small", Items::SARDINE, 24);
    toPool("saltwater_small", Items::SPIKE_FISH, 17);
    toPool("saltwater_small", Items::TIRE, 5);
    toPool("saltwater_small", Items::NAVAL_MINE, 1);
    toPool("saltwater_small", Items::TRINKETFIN, 5);
    toPool("saltwater_small", Items::OLD_BOOT, 5);
    toPool("saltwater_small", Items::TIN_CAN, 5);
    toPool("saltwater_small", Items::SODA_CAN, 5);
    toPool("saltwater_small", Items::CAR_BATTERY, 1);
    toPool("saltwater_small", Items::DRIFTWOOD, 5);
    toPool("saltwater_small", Items::SEA_GLASS, 5);

    toPool("saltwater_large", Items::RED_SNAPPER, 15);
    toPool("saltwater_large", Items::TUNA, 11);
    toPool("saltwater_large", Items::BLUE_MARLIN, 9);
    toPool("saltwater_large", Items::AMBERJACK, 15);
    toPool("saltwater_large", Items::COALFISH, 15);
    toPool("saltwater_large", Items::WAHOO, 11);
    toPool("saltwater_large", Items::ANCHOVY, 17);
    toPool("saltwater_large", Items::SARDINE, 17);
    toPool("saltwater_large", Items::POMPANO, 13);
    toPool("saltwater_large", Items::PUFFERFISH, 7);
    toPool("saltwater_large", Items::MAHIMAHI, 8);
    toPool("saltwater_large", Items::MORAY_EEL, 10);
    toPool("saltwater_large", Items::RIBBON_EEL, 9);
    toPool("saltwater_large", Items::SQUID, 8);
    toPool("saltwater_large", Items::OCTOPUS, 7);
    toPool("saltwater_large", Items::STAR_FISH, 13);
    toPool("saltwater_large", Items::TRINKETFIN, 6);
    toPool("saltwater_large", Items::STAR_FISH, 7);
    toPool("saltwater_large", Items::FISHTRONAUT, 6);
    toPool("saltwater_large", Items::NAVAL_MINE, 1);
    toPool("saltwater_large", Items::TIRE, 2);
    toPool("saltwater_large", Items::THIRTY_SEVEN_FISH, 1);
    toPool("saltwater_large", Items::SQUISHFISH, 1);
    toPool("saltwater_large", Items::OLD_BOOT, 2);
    toPool("saltwater_large", Items::TIN_CAN, 2);
    toPool("saltwater_large", Items::SODA_CAN, 2);
    toPool("saltwater_large", Items::CAR_BATTERY, 1);
    toPool("saltwater_large", Items::DRIFTWOOD, 2);
    toPool("saltwater_large", Items::SEA_GLASS, 2);


    BESTIARY_ITEMS.push_back(Items::LARGEMOUTH_BASS);
    BESTIARY_ITEMS.push_back(Items::SMALLMOUTH_BASS);
    BESTIARY_ITEMS.push_back(Items::BULLHEAD);
    BESTIARY_ITEMS.push_back(Items::CHUB);
    BESTIARY_ITEMS.push_back(Items::PIKE);
    BESTIARY_ITEMS.push_back(Items::WALLEYE);
    BESTIARY_ITEMS.push_back(Items::CARP);
    BESTIARY_ITEMS.push_back(Items::PERCH);
    BESTIARY_ITEMS.push_back(Items::CATFISH);
    BESTIARY_ITEMS.push_back(Items::MINNOW);
    BESTIARY_ITEMS.push_back(Items::KOI_A); // hardcoded: this shows all 3 variants
    BESTIARY_ITEMS.push_back(Items::RAINBOW_TROUT);
    BESTIARY_ITEMS.push_back(Items::GOLDEN_TROUT);
    BESTIARY_ITEMS.push_back(Items::GOLDFISH);
    BESTIARY_ITEMS.push_back(Items::GOLD_GOLDFISH);
    BESTIARY_ITEMS.push_back(Items::PRISMATIC_TROUT);
    BESTIARY_ITEMS.push_back(Items::SPIKE_FISH);
    BESTIARY_ITEMS.push_back(Items::TRINKETFIN);
    BESTIARY_ITEMS.push_back(Items::YESFIN);
    BESTIARY_ITEMS.push_back(Items::VIRIDIFIN);
    BESTIARY_ITEMS.push_back(Items::EDGEFISH);
    BESTIARY_ITEMS.push_back(Items::TERMINNOW);

    BESTIARY_ITEMS.push_back(Items::RED_SNAPPER);
    BESTIARY_ITEMS.push_back(Items::TUNA);
    BESTIARY_ITEMS.push_back(Items::BLUE_MARLIN);
    BESTIARY_ITEMS.push_back(Items::AMBERJACK);
    BESTIARY_ITEMS.push_back(Items::COALFISH);
    BESTIARY_ITEMS.push_back(Items::WAHOO);
    BESTIARY_ITEMS.push_back(Items::ANCHOVY);
    BESTIARY_ITEMS.push_back(Items::SARDINE);
    BESTIARY_ITEMS.push_back(Items::POMPANO);
    BESTIARY_ITEMS.push_back(Items::PUFFERFISH);
    BESTIARY_ITEMS.push_back(Items::MAHIMAHI);
    BESTIARY_ITEMS.push_back(Items::MORAY_EEL);
    BESTIARY_ITEMS.push_back(Items::RIBBON_EEL);
    BESTIARY_ITEMS.push_back(Items::SQUID);
    BESTIARY_ITEMS.push_back(Items::OCTOPUS);
    BESTIARY_ITEMS.push_back(Items::STAR_FISH);
    BESTIARY_ITEMS.push_back(Items::FISHTRONAUT);
    BESTIARY_ITEMS.push_back(Items::THIRTY_SEVEN_FISH);
    BESTIARY_ITEMS.push_back(Items::SQUISHFISH);
}

std::vector<Item*> getBestiaryItems(void)
{
    return BESTIARY_ITEMS;
}

bool hasDiscovered(Item* item)
{
    for (auto& info : game.fish_catch_info)
    {
        if (info.item == item)
        {
            return true;
        }
    }

    return false;
}

FishCatchInfo getFishCatchInfo(Item* item)
{
    for (auto& info : game.fish_catch_info)
    {
        if (info.item == item)
        {
            return info;
        }
    }

    FishCatchInfo info;
    info.item = item;
    info.smallest = 0;
    info.largest = 0;
    info.amount = 0;
    return info;
}

int getTrinketFinCount(void)
{
    int amount = 0;
    for (auto& stack : game.inventory)
    {
        if (stack.item == Items::TRINKETFIN)
        {
            amount += stack.count;
        }
    }
    return amount;
}

int getCaughtFishAmount(void)
{
    int amount = 0;
    for (auto& info : game.fish_catch_info)
    {
        amount += info.amount;
    }
    return amount;
}

ItemStack* getEquippedRod(void)
{
    for (auto& stack : game.inventory)
    {
        if (stack.item == Items::FISHING_ROD && stack.isEquipped())
        {
            return &stack;
        }
    }
    return NULL;
}

SDL_Color getWaterColorsForPool(std::string pool)
{
    if (pool == "freshwater_small") return graphics.getRGB(127, 192, 255);
    if (pool == "freshwater_large") return graphics.getRGB(127, 192, 255);
    if (pool == "saltwater_small") return graphics.getRGB(64, 160, 255);
    if (pool == "saltwater_large") return graphics.getRGB(64, 160, 255);
    if (pool == "junk") return graphics.getRGB(106, 127, 150);
    if (pool == "special_green")  return graphics.getRGB(SDL_clamp(80  + help.glow / 4, 0, 255), SDL_clamp(230 + help.glow / 4, 0, 255), SDL_clamp(180 + help.glow / 4, 0, 255));
    if (pool == "special_blue")   return graphics.getRGB(SDL_clamp(0   + help.glow / 4, 0, 255), SDL_clamp(64  + help.glow / 4, 0, 255), SDL_clamp(255 + help.glow / 4, 0, 255));
    if (pool == "special_purple") return graphics.getRGB(SDL_clamp(180 + help.glow / 4, 0, 255), SDL_clamp(16  + help.glow / 4, 0, 255), SDL_clamp(164 + help.glow / 4, 0, 255));
    return graphics.getRGB(127, 192, 255);
}

ItemStack getItemForPool(std::string pool, int flag)
{
    if (pool == "")
    {
        pool = "freshwater_small";
    }

    if (pool == "special_green") pool = "junk";
    if (pool == "special_blue") pool = "junk";
    if (pool == "special_purple") pool = "junk";

    if (POOLS.count(pool))
    {
        int bait_tier = getBaitTier();

        std::vector<ItemStack> chooseFrom;
        chooseFrom.clear();
        for (auto& data : POOLS[pool])
        {
            if (flag >= 50 && flag <= 54 && obj.flags[flag] && data.item == Items::TRINKETFIN) continue; // if trinketfin flag is set, don't add trinketfin to the pool
            if ((flag < 50 || flag > 54) && data.item == Items::TRINKETFIN) continue; // if trinketfin flag is NOT set, don't add trinketfin to the pool

            ItemStack stack = ItemStack(data.item, 1);
            int weight = data.weight * 10;

            switch (stack.getRarity())
            {
                case Rarity_RARE: weight += (5 * bait_tier); break;
                case Rarity_ELUSIVE: weight += (3 * bait_tier); break;
                case Rarity_LEGENDARY: weight += (1 * bait_tier); break;
            }

            for (int i = 0; i < SDL_max(weight, 1); i++)
            {
                chooseFrom.push_back(stack);
            }
        }
        return chooseFrom[SDL_clamp(fRandom() * chooseFrom.size(), 0, chooseFrom.size() - 1)];
    }

    return ItemStack(Items::CARP, 1);
}

void decrementItem(ItemStack stack, int amount)
{
    // ok, we only have a stack. i guess let's compare it?
    for (auto& inv_stack : game.inventory)
    {
        if (stack.item == inv_stack.item)
        {
            // same item at least. so check components
            bool components_match = true;
            if (stack.fish_size != inv_stack.fish_size) components_match = false;
            if (components_match)
            {
                inv_stack.decrement(amount);
                return;
            }
        }
    }
}

std::vector<ItemStack> getShopItems(void)
{
    std::vector<ItemStack> items;
    if (game.shopmode == ShopMode_SELL)
    {
        for (auto& stack : game.inventory)
        {
            if (stack.canSell())
            {
                items.push_back(stack);
            }
        }
    }
    else if (game.shopmode == ShopMode_BUY)
    {
        // loop through the whole registry ?!
        for (auto& item : ITEM_REGISTRY)
        {
            ItemStack stack = ItemStack(item.second, 1);
            stack.item->getDefaultComponents(&stack);

            if (!stack.canBuy()) continue;

            if (item.second == Items::GREEN_BOBBER && (hasItem(Items::GREEN_BOBBER))) continue;
            if (item.second == Items::BLUE_BOBBER && (hasItem(Items::BLUE_BOBBER))) continue;
            if (item.second == Items::PURPLE_BOBBER && (hasItem(Items::PURPLE_BOBBER))) continue;
            if (item.second == Items::BIG_BOBBER && (hasItem(Items::BIG_BOBBER))) continue;
            if (item.second == Items::FISH_SPINNER && (hasItem(Items::FISH_SPINNER))) continue;
            if (item.second == Items::FEATHER_SPINNER && (hasItem(Items::FEATHER_SPINNER))) continue;
            if (item.second == Items::COIN_SPINNER && (hasItem(Items::COIN_SPINNER))) continue;
            if (item.second == Items::TRINKET_SPINNER && (hasItem(Items::TRINKET_SPINNER))) continue;
            if (item.second == Items::VIRIDIAN_SPINNER && (hasItem(Items::VIRIDIAN_SPINNER))) continue;
            if (item.second == Items::GIANT_BOBBER && (hasItem(Items::GIANT_BOBBER))) continue;

            if (item.second == Items::BLUE_KEY && (hasItem(Items::BLUE_KEY))) continue;
            if (item.second == Items::PURPLE_KEY && (hasItem(Items::PURPLE_KEY))) continue;
            if (item.second == Items::RED_KEY && (hasItem(Items::RED_KEY))) continue;
            if (item.second == Items::BLUE_KEY && !obj.flags[6]) continue;
            if (item.second == Items::PURPLE_KEY && !obj.flags[3]) continue;
            if (item.second == Items::RED_KEY && !hasItem(Items::PURPLE_KEY)) continue;

            items.push_back(stack);
        }
        return items;
    }
    return items;
}

SDL_Texture* getBobberTexture(void)
{
    for (auto& inv_stack : game.inventory)
    {
        if (inv_stack.isEquipped())
        {
            if (inv_stack.item == Items::GREEN_BOBBER) return graphics.grphx.im_bobber_green;
            if (inv_stack.item == Items::BLUE_BOBBER) return graphics.grphx.im_bobber_blue;
            if (inv_stack.item == Items::PURPLE_BOBBER) return graphics.grphx.im_bobber_purple;
            if (inv_stack.item == Items::BIG_BOBBER) return graphics.grphx.im_bobber_big;
            if (inv_stack.item == Items::FISH_SPINNER) return graphics.grphx.im_bobber_fish;
            if (inv_stack.item == Items::FEATHER_SPINNER) return graphics.grphx.im_bobber_feather;
            if (inv_stack.item == Items::COIN_SPINNER) return graphics.grphx.im_bobber_coin;
            if (inv_stack.item == Items::TRINKET_SPINNER) return graphics.grphx.im_bobber_trinket;
            if (inv_stack.item == Items::VIRIDIAN_SPINNER) return graphics.grphx.im_bobber_viridian;
            if (inv_stack.item == Items::GIANT_BOBBER) return graphics.grphx.im_bobber_giant;
        }
    }
    return graphics.grphx.im_bobber;
}


float easeOutCubic(float x)
{
    return 1 - SDL_pow(1 - x, 3);
}

float easeInCubic(float x)
{
    return x * x * x;
}

float easeOutCirc(float x)
{
    return SDL_sqrtf(1 - SDL_pow(x - 1, 2));
}

int lerpReal(int a, int b, float t)
{
    return a + (b - a) * t;
}
