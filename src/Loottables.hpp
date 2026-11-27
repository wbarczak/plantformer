#pragma once

#include "EnumArray.hpp"
#include "Tiles.hpp"
#include "Items.hpp"

using Lotttables = EnumArray<TileId, ItemId, TILE_COUNT>;

Lotttables inline make_loottables(){
    Lotttables ltb;

    ltb[TileId::Air] = ItemId::Air;
    ltb[TileId::Dirt] = ItemId::TileDirt;
    ltb[TileId::Platform] = ItemId::TilePlatform;
    ltb[TileId::Spike] = ItemId::TileSpike;
    ltb[TileId::Checkpoint] = ItemId::TileCheckpoint;

    return ltb;
}