#pragma once

#include <cstdint>
#include <cstdlib>

#include "EnumArray.hpp"

enum class TileColision : uint8_t{
    Air,
    Solid,
    Hazard,
    Platform,
};

enum class TileBehaviour : uint8_t{
    None,
    Checkpoint,
};

struct Tile{
    TileColision colision{};
    TileBehaviour behaviour{};
};

enum TileId : uint16_t{
    Air,
    Dirt,
    Platform,
    Spike,
    Checkpoint,
};

static constexpr size_t TILE_COUNT = static_cast<size_t>(TileId::Checkpoint) + 1;

using Tiles = EnumArray<TileId, Tile, TILE_COUNT>;

Tiles inline make_tiles(){
    Tiles tiles;

    tiles[TileId::Air] = {};
    tiles[TileId::Dirt].colision = TileColision::Solid;
    tiles[TileId::Platform].colision = TileColision::Platform;
    tiles[TileId::Spike].colision = TileColision::Hazard;
    tiles[TileId::Checkpoint].behaviour = TileBehaviour::Checkpoint;

    return tiles;
}