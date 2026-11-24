#pragma once

#include <cstdint>
#include <cstdlib>

enum class TileColision : uint8_t{
    AIR,
    SOLID,
    HAZARD,
    PLATFORM,
};

enum class TileBehaviour : uint8_t{
    NONE,
    CHECKPOINT,
};

struct TileDef{
    TileColision colision{};
    TileBehaviour behaviour{};
};

enum TileId : int16_t{
    NONE = -1,
    DIRT,
    PLATFORM,
    SPIKE,
    CHECKPOINT,
};

static constexpr size_t TILE_COUNT = static_cast<size_t>(TileId::CHECKPOINT) + 1;