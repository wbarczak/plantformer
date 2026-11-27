#pragma once

#include <cstdlib>
#include <cstdint>

#include "EnumArray.hpp"
#include "Tiles.hpp"

enum class ItemType : uint8_t{
    Dummy,
    Tile,
};

struct Item{
    ItemType type;
    TileId tile;
};

enum class ItemId : uint16_t{
    Air,
    TileDirt,
    TilePlatform,
    TileSpike,
    TileCheckpoint,
};

static constexpr size_t ITEM_COUNT = static_cast<size_t>(ItemId::TileCheckpoint) + 1;

using Items = EnumArray<ItemId, Item, ITEM_COUNT>;

Items inline make_items(){
    Items items;

    items[ItemId::Air] = {};
    items[ItemId::TileDirt] = Item{
        .type = ItemType::Tile,
        .tile = TileId::Dirt,
    };
    items[ItemId::TilePlatform] = Item{
        .type = ItemType::Tile,
        .tile = TileId::Platform,
    };
    items[ItemId::TileSpike] = Item{
        .type = ItemType::Tile,
        .tile = TileId::Spike,
    };
    items[ItemId::TileCheckpoint] = Item{
        .type = ItemType::Tile,
        .tile = TileId::Checkpoint,
    };

    return items;
}