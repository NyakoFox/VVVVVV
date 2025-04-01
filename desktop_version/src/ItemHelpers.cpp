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

    Item* TEST;
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
    game.play_item_get = true;

    ItemGetDisplay display;
    display.stack = stack;
    display.timer = 0;
    game.item_get_displays.push_back(display);
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
    for (int i = 0; i < game.fish_catch_info.size(); i++)
    {
        FishCatchInfo& info = game.fish_catch_info[i];
        if (info.item == game.fishing_item.item)
        {
            info.smallest = SDL_min(info.smallest, game.fishing_item.fish_size);
            info.largest = SDL_max(info.largest, game.fishing_item.fish_size);
            info.amount += game.fishing_item.count;
            return;
        }
    }

    FishCatchInfo info;
    info.item = game.fishing_item.item;
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
    Items::LARGEMOUTH_BASS = registerItem("largemouth_bass", new FishItem(ItemSettings().withName("Largemouth Bass").withSell(22).withDescription("A carnivorous, freshwater member of the sunfish family.").withLayer("largemouth_bass_layer_1", 202).withLayer("largemouth_bass_layer_2", 203), 30, 40, 75));
    Items::SMALLMOUTH_BASS = registerItem("smallmouth_bass", new FishItem(ItemSettings().withName("Smallmouth Bass").withSell(30).withDescription("An adaptive freshwater fish known to be feistier than other bass.").withRarity(Rarity_UNCOMMON).withLayer("smallmouth_bass_layer_1", 206).withLayer("smallmouth_bass_layer_2", 207), 22, 27, 65));
    Items::BULLHEAD = registerItem("bullhead", new FishItem(ItemSettings().withName("Bullhead").withSell(20).withDescription("This bottom dwelling scavenger will eat almost anything.").withLayer("bullhead_layer_1", 208).withLayer("bullhead_layer_2", 209), 18, 23, 60));
    Items::CHUB = registerItem("chub", new FishItem(ItemSettings().withName("Chub").withSell(32).withDescription("A voracious eater, it will even eat fruit off trees above the water.").withRarity(Rarity_UNCOMMON).withLayer("chub_layer_1", 210).withLayer("chub_layer_2", 211), 15, 30, 60));
    Items::PIKE = registerItem("pike", new FishItem(ItemSettings().withName("Pike").withSell(70).withDescription("This large species is aggressive and quick when chasing prey.").withRarity(Rarity_RARE).withLayer("pike_layer_1", 212).withLayer("pike_layer_2", 213), 40, 55, 150));
    Items::WALLEYE = registerItem("walleye", new FishItem(ItemSettings().withName("Walleye").withSell(65).withDescription("Its unique pearlescent eyes give this fish powerful night vision.").withRarity(Rarity_RARE).withLayer("walleye_layer_1", 214).withLayer("walleye_layer_2", 215), 36, 54, 107));
    Items::CARP = registerItem("carp", new FishItem(ItemSettings().withName("Carp").withSell(18).withDescription("This common freshwater fish is highly tolerant of turbid waters.").withLayer("carp_layer_1", 216).withLayer("carp_layer_2", 217), 25, 31, 120));
    Items::PERCH = registerItem("perch", new FishItem(ItemSettings().withName("Perch").withSell(35).withDescription("This yellow fish is a critical food source for bass and walleye.").withRarity(Rarity_UNCOMMON).withLayer("perch_layer_1", 218).withLayer("perch_layer_2", 219), 10, 19, 50));
    Items::CATFISH = registerItem("catfish", new FishItem(ItemSettings().withName("Catfish").withSell(65).withDescription("These whiskered fish use pheremones to communicate. They have a social hierarchy.").withRarity(Rarity_RARE).withLayer("catfish_layer_1", 220).withLayer("catfish_layer_2", 221), 43, 57, 132));
    Items::MINNOW = registerItem("minnow", new FishItem(ItemSettings().withName("Minnow").withSell(30).withDescription("These little fish travel in large shoals to avoid predators.").withRarity(Rarity_UNCOMMON).withLayer("minnow_layer_1", 222).withLayer("minnow_layer_2", 223), 5, 7, 14));
    Items::KOI_A = registerItem("koi_a", new FishItem(ItemSettings().withName("Koi").withSell(60).withDescription("A domesticated variety of carp with beautiful colored scales.").withRarity(Rarity_RARE).withLayer("koi_1_layer_1", 224).withLayer("koi_1_layer_2", 225), 25, 31, 60));
    Items::KOI_B = registerItem("koi_b", new FishItem(ItemSettings().withName("Koi").withSell(60).withDescription("A domesticated variety of carp with beautiful colored scales.").withRarity(Rarity_RARE).withLayer("koi_2_layer_1", 224).withLayer("koi_2_layer_2", 225), 25, 31, 60));
    Items::KOI_C = registerItem("koi_c", new FishItem(ItemSettings().withName("Koi").withSell(60).withDescription("A domesticated variety of carp with beautiful colored scales.").withRarity(Rarity_RARE).withLayer("koi_3_layer_1", 224).withLayer("koi_3_layer_2", 225), 25, 31, 60));
    Items::RAINBOW_TROUT = registerItem("rainbow_trout", new FishItem(ItemSettings().withName("Rainbow Trout").withSell(75).withDescription("This multicolored fish is known for leaping out of the water.").withRarity(Rarity_RARE).withLayer("rainbow_trout_layer_1", 226).withLayer("rainbow_trout_layer_2", 227).withLayer("rainbow_trout_layer_3", 228).withLayer("rainbow_trout_layer_4", 229), 30, 60, 122));
    Items::GOLDEN_TROUT = registerItem("golden_trout", new FishItem(ItemSettings().withName("Golden Trout").withSell(100).withDescription("This beautiful golden trout is out-competed by other species.").withRarity(Rarity_ELUSIVE).withLayer("golden_trout_layer_1", 230).withLayer("golden_trout_layer_2", 231).withLayer("golden_trout_layer_3", 232), 15, 30, 71));
    Items::GOLDFISH = registerItem("goldfish", new FishItem(ItemSettings().withName("Goldfish").withSell(50).withDescription("Commonly kept as a pet. It gets bored easily without accessories.").withRarity(Rarity_RARE).withLayer("goldfish_layer_1", 233).withLayer("goldfish_layer_2", 234), 11, 15, 41));
    Items::GOLD_GOLDFISH = registerItem("gold_goldfish", new FishItem(ItemSettings().withName("Gold Goldfish").withSell(200).withDescription("Its scales are made of gold leaf. It is sensitive to temperature.").withRarity(Rarity_LEGENDARY).withLayer("gold_goldfish_layer_1", 235).withLayer("gold_goldfish_layer_2", 236).withLayer("gold_goldfish_layer_3", 237).withLayer("gold_goldfish_layer_4", 238).withLayer("gold_goldfish_layer_5", 239).withLayer("gold_goldfish_layer_6", 240), 11, 15, 41));
    Items::PRISMATIC_TROUT = registerItem("prismatic_trout", new FishItem(ItemSettings().withName("Prismatic Trout").withSell(200).withDescription("The mesmerizing scales of this fish can match any colour.").withRarity(Rarity_LEGENDARY).withNameColor(241).withLayer("prismatic_trout", 241), 30, 60, 100));
    Items::YESFIN = registerItem("yesfin", new FishItem(ItemSettings().withName("Yesfin").withSell(100).withDescription("With a natural briefcase, this fish is perfect for the workplace.").withRarity(Rarity_ELUSIVE).withLayer("yesfin", 9), 20, 30, 70));
    Items::VIRIDIFIN = registerItem("viridifin", new FishItem(ItemSettings().withName("Viridifin").withSell(222).withDescription("It has an uncanny resemblance to you, to say the least...").withRarity(Rarity_LEGENDARY).withLayer("viridifin", 0), 40, 60, 100));
    Items::EDGEFISH = registerItem("edgefish", new FishItem(ItemSettings().withName("Edgefish").withSell(190).withDescription("This fish subsists on pellets. No trademark infringement intended.").withRarity(Rarity_LEGENDARY).withLayer("edgefish", 8), 15, 30, 60));
    Items::TERMINNOW = registerItem("terminnow", new FishItem(ItemSettings().withName("Terminnow").withSell(200).withDescription("      -= PERSONAL LOG =-      \nI can't believe this fish even has Wi-Fi.").withRarity(Rarity_LEGENDARY).withLayer("terminnow", 4), 10, 15, 20));
    Items::TIRE = registerItem("tire", new Item(ItemSettings().withName("Tire").withSell(0).withDescription("This tire could be from the 3099 Space Derby Grand Prix.").withRarity(Rarity_JUNK).withLayer("tire", 19)));
    Items::SPIKE_FISH = registerItem("spike_fish", new FishItem(ItemSettings().withName("Spike Fish").withSell(15).withDescription("They leave behind spiky corpses when they die. An invasive species found everywhere.").withLayer("spike_fish", -1), 40, 40, 40));
    Items::TRINKETFIN = registerItem("trinketfin", new Item(ItemSettings().withName("Trinketfin").withDescription("Congratulations! You have found a shiny trinketfin!").withRarity(Rarity_RARE).withLayer("trinketfin", 3)));
    //Items::A = registerItem("", new FishItem(ItemSettings().withName("").withDescription("").withLayer("", 208).withLayer("", 209), 18, 23, 60));

    Items::YELLOW_KEY_FAKE = registerItem("yellow_key_fake", new Item(ItemSettings().withName("Metallic Object").withDescription("A mysterious metal object. It has a tag which says \"key\" on it.").withLayer("key", 26)));
    Items::YELLOW_KEY = registerItem("yellow_key", new Item(ItemSettings().withName("Yellow Key").withDescription("A \"key\" which opens yellow gates.").withLayer("key", 26)));
    Items::BLUE_KEY = registerItem("blue_key", new Item(ItemSettings().withName("Blue Key").withBuy(1000).withDescription("A \"key\" which opens blue gates.").withLayer("key", 300)));
    Items::PURPLE_KEY = registerItem("purple_key", new Item(ItemSettings().withName("Purple Key").withBuy(1500).withDescription("A \"key\" which opens purple gates.").withLayer("key", 301)));
    Items::RED_KEY = registerItem("red_key", new Item(ItemSettings().withName("Red Key").withBuy(1000).withDescription("A \"key\" which opens red gates.").withLayer("key", 302)));

    Items::BUCKET = registerItem("bucket", new BucketItem(ItemSettings().withName("Bucket (Empty)").withDescription("It's a bucket. You cannot wear it on your head.").withLayer("bucket", 303)));
    Items::FRESHWATER_BUCKET = registerItem("freshwater_bucket", new FilledBucketItem(ItemSettings().withName("Bucket (Filled)").withDescription("It's a bucket filled with fresh water.").withLayer("bucket", 303).withLayer("bucket_fill",304)));
    Items::SALTWATER_BUCKET = registerItem("saltwater_bucket", new FilledBucketItem(ItemSettings().withName("Bucket (Filled)").withDescription("It's a bucket filled with salt water.").withLayer("bucket", 303).withLayer("bucket_fill",305)));
    Items::JUNKWATER_BUCKET = registerItem("junkwater_bucket", new FilledBucketItem(ItemSettings().withName("Bucket (Filled)").withDescription("It's a bucket filled with... gross water.").withLayer("bucket", 303).withLayer("bucket_fill",306)));
    Items::GREENWATER_BUCKET = registerItem("greenwater_bucket", new FilledBucketItem(ItemSettings().withName("Bucket (Filled)").withDescription("It's a bucket filled with green chemicals.").withLayer("bucket", 303).withLayer("bucket_fill",307)));
    Items::BLUEWATER_BUCKET = registerItem("bluewater_bucket", new FilledBucketItem(ItemSettings().withName("Bucket (Filled)").withDescription("It's a bucket filled with blue chemicals.").withLayer("bucket", 303).withLayer("bucket_fill",308)));
    Items::PURPLEWATER_BUCKET = registerItem("purplewater_bucket", new FilledBucketItem(ItemSettings().withName("Bucket (Filled)").withDescription("It's a bucket filled with purple chemicals.").withLayer("bucket", 303).withLayer("bucket_fill",309)));

    Items::ENHANCED_BAIT = registerItem("enhanced_bait", new Item(ItemSettings().withName("Enhanced Bait").withDescription("Upgraded by Vitellary using the power of science. Still not a fish.").withLayer("worm_layer_1", 310).withLayer("worm_layer_2", 311)));
    Items::DELUXE_BAIT = registerItem("deluxe_bait", new Item(ItemSettings().withName("Deluxe Bait").withDescription("Upgraded even further. Has science gone too far? Not a fish.").withLayer("worm_layer_1", 312).withLayer("worm_layer_2", 313)));
    Items::ULTRA_BAIT = registerItem("ultra_bait", new Item(ItemSettings().withName("Ultra Bait").withDescription("It's a good thing this wasn't called something else.").withLayer("worm_layer_1", 314).withLayer("worm_layer_2", 315)));

    Items::TEST = registerItem("test", new Item(ItemSettings().withName("Test").withDescription("These whiskered fish use pheremones to communicate. They have a social hierarchy.")));

    toPool("freshwater_small", Items::LARGEMOUTH_BASS, 27);
    toPool("freshwater_small", Items::SMALLMOUTH_BASS, 21);
    toPool("freshwater_small", Items::BULLHEAD, 33);
    toPool("freshwater_small", Items::CHUB, 21);
    toPool("freshwater_small", Items::CARP, 42);
    toPool("freshwater_small", Items::PERCH, 24);
    toPool("freshwater_small", Items::MINNOW, 21);
    toPool("freshwater_small", Items::SPIKE_FISH, 21);
    toPool("freshwater_small", Items::TIRE, 15);
    toPool("freshwater_small", Items::GOLDFISH, 6);
    toPool("freshwater_small", Items::KOI_A, 1);
    toPool("freshwater_small", Items::KOI_B, 1);
    toPool("freshwater_small", Items::KOI_C, 1);

    toPool("freshwater_large", Items::LARGEMOUTH_BASS, 10);
    toPool("freshwater_large", Items::SMALLMOUTH_BASS, 7);
    toPool("freshwater_large", Items::BULLHEAD, 10);
    toPool("freshwater_large", Items::CHUB, 7);
    toPool("freshwater_large", Items::PIKE, 4);
    toPool("freshwater_large", Items::WALLEYE, 3);
    toPool("freshwater_large", Items::CARP, 10);
    toPool("freshwater_large", Items::PERCH, 8);
    toPool("freshwater_large", Items::CATFISH, 4);
    toPool("freshwater_large", Items::MINNOW, 7);
    toPool("freshwater_large", Items::KOI_A, 1);
    toPool("freshwater_large", Items::KOI_B, 1);
    toPool("freshwater_large", Items::KOI_C, 1);
    toPool("freshwater_large", Items::RAINBOW_TROUT, 4);
    toPool("freshwater_large", Items::GOLDEN_TROUT, 2);
    toPool("freshwater_large", Items::GOLDFISH, 4);
    toPool("freshwater_large", Items::GOLD_GOLDFISH, 1);
    toPool("freshwater_large", Items::PRISMATIC_TROUT, 1);
    toPool("freshwater_large", Items::SPIKE_FISH, 7);
    toPool("freshwater_large", Items::TRINKETFIN, 1);
    toPool("freshwater_large", Items::YESFIN, 2);
    toPool("freshwater_large", Items::VIRIDIFIN, 1);
    toPool("freshwater_large", Items::TIRE, 4);
    toPool("freshwater_large", Items::EDGEFISH, 1);
    toPool("freshwater_large", Items::TERMINNOW, 1);

    toPool("junk", Items::CARP, 10);
    toPool("junk", Items::TIRE, 25);
    toPool("junk", Items::GOLDFISH, 1);
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

ItemStack getItemForPool(std::string pool)
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
