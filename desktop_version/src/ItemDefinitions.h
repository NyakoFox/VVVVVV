#ifndef ITEMDEFINITIONS_H
#define ITEMDEFINITIONS_H

enum FishingState {
    FishingState_IDLE,
    FishingState_CHOOSING,
    FishingState_POSTCHOOSING,
    FishingState_CASTING,
    FishingState_WAITING,
    FishingState_HOOKED,
    FishingState_REELING
};

struct ItemGetDisplay {
    ItemStack stack;
    int timer;
};

#endif /* ITEMDEFINITIONS_H */
