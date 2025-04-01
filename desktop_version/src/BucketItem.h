#ifndef BUCKETITEM_H
#define BUCKETITEM_H

#include "Item.h"

#include <string>
#include <map>
#include <vector>

#include "SDL.h"

class BucketItem : public Item
{
public:
    BucketItem(ItemSettings settings);
    BucketItem(void);
    virtual bool canUse(ItemStack* stack) override;
    virtual void use(ItemStack* stack) override;
};

#endif /* BUCKETITEM_H */
