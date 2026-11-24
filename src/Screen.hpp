#pragma once

#include <cstdlib>

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