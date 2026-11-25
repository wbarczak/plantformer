#pragma once

#include <cstdint>
#include <cstdlib>

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

struct TileDef{
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