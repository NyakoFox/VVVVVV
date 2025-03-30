#include "ItemHelpers.h"

#include "FishItem.h"
#include "FishingRodItem.h"
#include "Game.h"
#include "ItemStack.h"
#include "Item.h"
#include "Music.h"

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

    Item* TEST;
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

void registerItems(void)
{
    Items::FISHING_ROD = registerItem("fishing_rod", new FishingRodItem(ItemSettings().withName("Fishing Rod").withDescription("A basic fishing rod.").withLayer("fishing_rod_basic", 200).withLayer("fishing_line", 201)));
    Items::WORMS = registerItem("worms", new Item(ItemSettings().withName("Worms").withDescription("A perfect bait for beginners. Notably not a fish.").withLayer("worm_layer_1", 204).withLayer("worm_layer_2", 205)));
    Items::LARGEMOUTH_BASS = registerItem("largemouth_bass", new FishItem(ItemSettings().withName("Largemouth Bass").withDescription("A carnivorous, freshwater member of the sunfish family.").withLayer("largemouth_bass_layer_1", 202).withLayer("largemouth_bass_layer_2", 203), 30, 40, 75));
    Items::SMALLMOUTH_BASS = registerItem("smallmouth_bass", new FishItem(ItemSettings().withName("Smallmouth Bass").withDescription("An adaptive freshwater fish known to be feistier than other bass.").withRarity(Rarity_UNCOMMON).withLayer("smallmouth_bass_layer_1", 206).withLayer("smallmouth_bass_layer_2", 207), 22, 27, 65));
    Items::BULLHEAD = registerItem("bullhead", new FishItem(ItemSettings().withName("Bullhead").withDescription("This bottom dwelling scavenger will eat almost anything.").withLayer("bullhead_layer_1", 208).withLayer("bullhead_layer_2", 209), 18, 23, 60));
    Items::CHUB = registerItem("chub", new FishItem(ItemSettings().withName("Chub").withDescription("A voracious eater, it will even eat fruit off trees above the water.").withRarity(Rarity_UNCOMMON).withLayer("chub_layer_1", 210).withLayer("chub_layer_2", 211), 15, 30, 60));
    Items::PIKE = registerItem("pike", new FishItem(ItemSettings().withName("Pike").withDescription("This large species is aggressive and quick when chasing prey.").withRarity(Rarity_RARE).withLayer("pike_layer_1", 212).withLayer("pike_layer_2", 213), 40, 55, 150));
    Items::WALLEYE = registerItem("walleye", new FishItem(ItemSettings().withName("Walleye").withDescription("Its unique pearlescent eyes give this fish powerful night vision.").withRarity(Rarity_RARE).withLayer("walleye_layer_1", 214).withLayer("walleye_layer_2", 215), 36, 54, 107));
    Items::CARP = registerItem("carp", new FishItem(ItemSettings().withName("Carp").withDescription("This common freshwater fish is highly tolerant of turbid waters.").withLayer("carp_layer_1", 216).withLayer("carp_layer_2", 217), 25, 31, 120));
    Items::PERCH = registerItem("perch", new FishItem(ItemSettings().withName("Perch").withDescription("This yellow fish is a critical food source for bass and walleye.").withRarity(Rarity_UNCOMMON).withLayer("perch_layer_1", 218).withLayer("perch_layer_2", 219), 10, 19, 50));
    Items::CATFISH = registerItem("catfish", new FishItem(ItemSettings().withName("Catfish").withDescription("These whiskered fish use pheremones to communicate. They have a social hierarchy.").withRarity(Rarity_RARE).withLayer("catfish_layer_1", 220).withLayer("catfish_layer_2", 221), 43, 57, 132));
    Items::MINNOW = registerItem("minnow", new FishItem(ItemSettings().withName("Minnow").withDescription("These little fish travel in large shoals to avoid predators.").withRarity(Rarity_UNCOMMON).withLayer("minnow_layer_1", 222).withLayer("minnow_layer_2", 223), 5, 7, 14));
    Items::KOI_A = registerItem("koi_a", new FishItem(ItemSettings().withName("Koi").withDescription("A domesticated variety of carp with beautiful colored scales.").withRarity(Rarity_RARE).withLayer("koi_1_layer_1", 224).withLayer("koi_1_layer_2", 225), 25, 31, 60));
    Items::KOI_B = registerItem("koi_b", new FishItem(ItemSettings().withName("Koi").withDescription("A domesticated variety of carp with beautiful colored scales.").withRarity(Rarity_RARE).withLayer("koi_2_layer_1", 224).withLayer("koi_2_layer_2", 225), 25, 31, 60));
    Items::KOI_C = registerItem("koi_c", new FishItem(ItemSettings().withName("Koi").withDescription("A domesticated variety of carp with beautiful colored scales.").withRarity(Rarity_RARE).withLayer("koi_3_layer_1", 224).withLayer("koi_3_layer_2", 225), 25, 31, 60));
    Items::RAINBOW_TROUT = registerItem("rainbow_trout", new FishItem(ItemSettings().withName("Rainbow Trout").withDescription("This multicolored fish is known for leaping out of the water.").withRarity(Rarity_RARE).withLayer("rainbow_trout_layer_1", 226).withLayer("rainbow_trout_layer_2", 227).withLayer("rainbow_trout_layer_3", 228).withLayer("rainbow_trout_layer_4", 229), 30, 60, 122));
    Items::GOLDEN_TROUT = registerItem("golden_trout", new FishItem(ItemSettings().withName("Golden Trout").withDescription("This beautiful golden trout is out-competed by other species.").withRarity(Rarity_ELUSIVE).withLayer("golden_trout_layer_1", 230).withLayer("golden_trout_layer_2", 231).withLayer("golden_trout_layer_3", 232), 15, 30, 71));
    Items::GOLDFISH = registerItem("goldfish", new FishItem(ItemSettings().withName("Goldfish").withDescription("Commonly kept as a pet. It gets bored easily without accessories.").withRarity(Rarity_RARE).withLayer("goldfish_layer_1", 233).withLayer("goldfish_layer_2", 234), 11, 15, 41));
    Items::GOLD_GOLDFISH = registerItem("gold_goldfish", new FishItem(ItemSettings().withName("Gold Goldfish").withDescription("Its scales are made of gold leaf. It is sensitive to temperature.").withRarity(Rarity_LEGENDARY).withLayer("gold_goldfish_layer_1", 235).withLayer("gold_goldfish_layer_2", 236).withLayer("gold_goldfish_layer_3", 237).withLayer("gold_goldfish_layer_4", 238).withLayer("gold_goldfish_layer_5", 239).withLayer("gold_goldfish_layer_6", 240), 11, 15, 41));
    Items::PRISMATIC_TROUT = registerItem("prismatic_trout", new FishItem(ItemSettings().withName("Prismatic Trout").withDescription("The mesmerizing scales of this fish can match any colour.").withRarity(Rarity_LEGENDARY).withNameColor(241).withLayer("prismatic_trout", 241), 30, 60, 100));
    Items::YESFIN = registerItem("yesfin", new FishItem(ItemSettings().withName("Yesfin").withDescription("With a natural briefcase, this fish is perfect for the workplace.").withRarity(Rarity_ELUSIVE).withLayer("yesfin", 9), 20, 30, 70));
    Items::VIRIDIFIN = registerItem("viridifin", new FishItem(ItemSettings().withName("Viridifin").withDescription("It has an uncanny resemblance to you, to say the least...").withRarity(Rarity_LEGENDARY).withLayer("viridifin", 0), 40, 60, 100));
    Items::EDGEFISH = registerItem("edgefish", new FishItem(ItemSettings().withName("Edgefish").withDescription("This fish subsists on pellets. No trademark infringement intended.").withRarity(Rarity_LEGENDARY).withLayer("edgefish", 8), 15, 30, 60));
    Items::TERMINNOW = registerItem("terminnow", new FishItem(ItemSettings().withName("Terminnow").withDescription("      -= PERSONAL LOG =-      \nI can't believe this fish even has Wi-Fi.").withRarity(Rarity_LEGENDARY).withLayer("terminnow", 4), 10, 15, 20));
    //Items::A = registerItem("", new FishItem(ItemSettings().withName("").withDescription("").withLayer("", 208).withLayer("", 209), 18, 23, 60));

    Items::TEST = registerItem("test", new Item(ItemSettings().withName("Test").withDescription("These whiskered fish use pheremones to communicate. They have a social hierarchy.")));
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
