#pragma once

#include <array>

#include "Screen.hpp"

inline struct Global{
    std::array<Screen, SCREEN_COUNT> screens;
}g;