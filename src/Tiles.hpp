#pragma once

#include <cstdint>
#include <cstdlib>
#include <unordered_map>

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

enum TileId : int16_t{
    None = -1,
    Dirt,
    Platform,
    Spike,
    Checkpoint,
};

static constexpr size_t TILE_COUNT = static_cast<size_t>(TileId::Checkpoint) + 1;

using Tiles = std::unordered_map<TileId, Tile>;

Tiles inline make_tiles(){
    Tiles tiles;

    tiles[TileId::None] = {};
    tiles[TileId::Dirt].colision = TileColision::Solid;
    tiles[TileId::Platform].colision = TileColision::Platform;
    tiles[TileId::Spike].colision = TileColision::Hazard;
    tiles[TileId::Checkpoint].behaviour = TileBehaviour::Checkpoint;

    return tiles;
}