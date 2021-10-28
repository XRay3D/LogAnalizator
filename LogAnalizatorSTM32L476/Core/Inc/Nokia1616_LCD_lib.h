#pragma once

#include "font5x8.h"
#include "spfd54124b.h"
#include <array>
#include <charconv>
#include <concepts>
#include <cstring>
#include <span>
#include <stdint.h>
#include <string_view>

// --------------------
class Nokia1616_LCD : public SPFD54124B {
public:
    Nokia1616_LCD();

    void print(const char c, Point p, const Color& color);
    void print(const std::string_view str, Point p, const Color& color);

    template <std::integral T>
    void print(T data, Point p, const Color& color, const uint8_t base = 10)
    {
        constexpr uint8_t size = sizeof(T) * 8; //16
        std::array<char, size + 1> array {};
        std::memset(array.data(), ' ', size);
        if (auto [ptr, ec] = std::to_chars(array.begin(), array.end(), data, base); ec == std::errc {}) {
            print({ array.data(), ptr }, p, color);
        }
    }

    template <std::floating_point T>
    void print(T data, Point p, const Color& color)
    {
        constexpr uint8_t size = sizeof(T) * 8; //16
        std::array<char, size + 1> array {};
        std::memset(array.data(), ' ', size);
        if (auto [ptr, ec] = std::to_chars(array.begin(), array.end(), data); ec == std::errc {}) {
            print({ array.data(), ptr }, p, color);
        }
    }
};
