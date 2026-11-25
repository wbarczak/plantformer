#pragma once

#include <array>

#include <raylib.h>

#include "Constants.hpp"
#include "Tiles.hpp"

struct Position{
    int32_t x{}, y{};
};

struct Entity{
    Vector2 pos{50, 40};
    Vector2 vel{};
    Rectangle hurtbox{-.25, -.75, .5, .75};
    Position spawnpoint;
    bool grounded = false;
    bool facing_left = false;
    bool respawning = false;
};

// To access a tile at (x, y) do tiles[y][x] cuz that's more intuitive for me
using Tilemap = std::array<std::array<TileId, WORLD_WIDTH>, WORLD_HEIGHT>;

struct World{
    Tilemap map;

    Entity player;
};

World inline make_new_world(){
    World world;
    
    static constexpr size_t HALF_WORLD_HEIGHT = WORLD_HEIGHT / 2;
    for(size_t i{}; i < HALF_WORLD_HEIGHT; i++){
        world.map[i].fill(TileId::None);
    }
    for(size_t i = HALF_WORLD_HEIGHT; i < WORLD_HEIGHT; i++){
        world.map[i].fill(TileId::Dirt);
    }

    return world;
}