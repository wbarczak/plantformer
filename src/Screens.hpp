#pragma once

#include <cstdlib>

#include "EnumArray.hpp"
#include "screens/game.hpp"

struct Screen{
    void (*on_enter)() = nullptr;
    void (*on_exit)() = nullptr;
    void (*update)() = nullptr;
    void (*draw)() = nullptr;
};

enum class ScreenId{
    Game,
};

static constexpr size_t SCREEN_COUNT = static_cast<size_t>(ScreenId::Game) + 1;

using Screens = EnumArray<ScreenId, Screen, SCREEN_COUNT>;

Screens inline make_screens(){
    Screens screens;

    screens[ScreenId::Game] = Screen{
        .on_enter = game_on_enter,
        .on_exit = game_on_exit,
        .update = game_update,
        .draw = game_draw,
    };

    return screens;
}