#pragma once

struct Screen{
    void (*on_enter) = nullptr;
    void (*on_exit) = nullptr;
    void (*update) = nullptr;
    void (*draw) = nullptr;
};

enum class ScreenId{
    GAME,

    COUNT,
};