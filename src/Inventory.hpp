#pragma once

#include <array>

#include "Constants.hpp"
#include "Items.hpp"

struct Stack{
    ItemId item_id{};
    int32_t count{};
};

struct Inventory{
    std::array<std::array<Stack, INVENTORY_ITEM_COUNT_ON_X_AXIS>, INVENTORY_ITEM_COUNT_ON_Y_AXIS> items;
};

Inventory inline make_new_inventory(){
    Inventory inv;

    for(auto& row : inv.items){
        row.fill({});
    };

    return inv;
}