#pragma once

#include <unordered_map>

#include "Tiles.hpp"
#include "Items.hpp"

using Lotttables = std::unordered_map<TileId, ItemId>;

Lotttables inline make_loottables(){
    Lotttables ltb;

    ltb[TileId::None] = ItemId::Air;
    ltb[TileId::Dirt] = ItemId::TileDirt;
    ltb[TileId::Platform] = ItemId::TilePlatform;
    ltb[TileId::Spike] = ItemId::TileSpike;
    ltb[TileId::Checkpoint] = ItemId::TileCheckpoint;

    return ltb;
}