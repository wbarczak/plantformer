#pragma once

#include <cassert>
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

    inv.items[0][0] = {ItemId::TileDirt, 200};
    inv.items[0][1] = {ItemId::TilePlatform, 200};
    inv.items[0][2] = {ItemId::TileSpike, 200};
    inv.items[0][3] = {ItemId::TileCheckpoint, 200};

    return inv;
}

bool inline inventory_pickup(Inventory& inv, ItemId item){
    assert(item != ItemId::Air);
    bool any_empty{};
    size_t empty_x{}, empty_y{};

    for(size_t y{}; y < INVENTORY_ITEM_COUNT_ON_Y_AXIS; y++){
        for(size_t x{}; x < INVENTORY_ITEM_COUNT_ON_X_AXIS; x++){
            auto& slot = inv.items[y][x];
            if(!any_empty && slot.item_id == ItemId::Air){
                any_empty = true;
                empty_x = x;
                empty_y = y;
            }
            if(slot.count < MAX_STACK_SIZE){
                slot.count += 1;
                return true;
            }
        }
    }

    if(any_empty){
        inv.items[empty_y][empty_x] = ItemStack{.item_id = item, .count = 1};
        return true;
    }

    return false;
}

void inline inventory_decrease_stack(ItemStack& stack){
    stack.count -= 1;
    if(stack.count == 0){
        stack.item_id = ItemId::Air;
    }
}