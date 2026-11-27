#pragma once

#include "Screens.hpp"
#include "Assets.hpp"
#include "Tiles.hpp"
#include "Input.hpp"
#include "Items.hpp"
#include "Loottables.hpp"

inline struct Global{
    Assets assets;
    
    Screens screens;
    ScreenId active_screen_id = ScreenId::Game;
    float elapsed{};

    Tiles tiles;
    Items items;

    Lotttables loottables;

    Input input;

    Rectangle render_texture_destination_rectangle{};
}global;

void inline init_global(){
    global.assets = make_assets();
    global.screens = make_screens();
    global.tiles = make_tiles();
    global.items = make_items();
    global.loottables = make_loottables();
}