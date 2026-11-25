#pragma once

#include "Screens.hpp"
#include "Assets.hpp"
#include "Tiles.hpp"
#include "Input.hpp"
#include "Items.hpp"

inline struct Global{
    Assets assets;
    
    Screens screens;
    ScreenId active_screen_id = ScreenId::Game;

    Tiles tiles;
    Items items;

    Input input;

    Rectangle render_texture_destination_rectangle{};
}global;

void inline init_global(){
    global.assets = make_assets();
    global.screens = make_screens();
    global.tiles = make_tiles();
    global.items = make_items();
}