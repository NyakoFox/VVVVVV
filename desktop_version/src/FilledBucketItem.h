#ifndef FILLEDBUCKETITEM_H
#define FILLEDBUCKETITEM_H

#include "Item.h"

#include <string>
#include <map>
#include <vector>

#include "SDL.h"

class FilledBucketItem : public Item
{
public:
    FilledBucketItem(ItemSettings settings);
    FilledBucketItem(void);
    virtual bool canUse(ItemStack* stack) override;
    virtual void use(ItemStack* stack) override;
};

#endif /* FILLEDBUCKETITEM_H */
