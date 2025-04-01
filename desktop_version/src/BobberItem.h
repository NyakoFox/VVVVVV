#ifndef BOBBERITEM_H
#define BOBBERITEM_H

#include "Item.h"
#include "EquippableItem.h"

#include <string>
#include <map>
#include <vector>

#include "SDL.h"

class BobberItem : public EquippableItem
{
public:
    BobberItem(ItemSettings settings);
    BobberItem(void);
    virtual void use(ItemStack* stack) override;
};

#endif /* BOBBERITEM_H */
