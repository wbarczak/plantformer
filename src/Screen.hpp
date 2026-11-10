#pragma once

struct Screen{
    void (*on_enter) = nullptr;
    void (*on_exit) = nullptr;
    void (*update) = nullptr;
    void (*draw) = nullptr;
};

enum ScreenId{
    SCREEN_GAME,

    SCREEN_COUNT,
}