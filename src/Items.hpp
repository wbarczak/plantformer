#pragma once

#include <cstdlib>
#include <cstdint>
#include <unordered_map>

#include "Tiles.hpp"

enum class ItemType : uint8_t{
    Tile,
};

struct Item{
    ItemType type;
    TileId tile;
};

enum class ItemId : uint16_t{
    TileDirt,
    TilePlatform,
    TileSpike,
    TileCheckpoint,
};

static constexpr size_t ITEM_COUNT = static_cast<size_t>(ItemId::TileCheckpoint) + 1;

using Items = std::unordered_map<ItemId, Item>;

Items inline make_items(){
    Items items;

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