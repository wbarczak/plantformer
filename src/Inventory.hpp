#pragma once

#include <array>

#include "Constants.hpp"
#include "Items.hpp"

struct ItemStack{
    ItemId item_id = ItemId::Air;
    int32_t count{};
};

struct Inventory{
    std::array<std::array<ItemStack, INVENTORY_ITEM_COUNT_ON_X_AXIS>, INVENTORY_ITEM_COUNT_ON_Y_AXIS> items;
};

Inventory inline make_new_inventory(){
    Inventory inv;

    for(auto& row : inv.items){
        row.fill({});
    };

    inv.items[0][0] = {ItemId::TileDirt};
    inv.items[0][1] = {ItemId::TilePlatform};
    inv.items[0][2] = {ItemId::TileSpike};
    inv.items[0][3] = {ItemId::TileCheckpoint};

    return inv;
}

bool inline inventory_pickup(Inventory& inv, ItemId item){
    return false;
}