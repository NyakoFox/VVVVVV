#ifndef ITEMDEFINITIONS_H
#define ITEMDEFINITIONS_H

enum FishingState {
    FishingState_IDLE,
    FishingState_CHOOSING,
    FishingState_POSTCHOOSING,
    FishingState_CASTING,
    FishingState_WAITING,
    FishingState_HOOKED,
    FishingState_REELING,
    FishingState_REELING_NOFISH
};

struct ItemGetDisplay {
    ItemStack stack;
    int timer;
};

struct FishCatchInfo
{
    Item* item;
    int smallest;
    int largest;
    int amount;
};

#endif /* ITEMDEFINITIONS_H */
