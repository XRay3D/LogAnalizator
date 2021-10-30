#pragma once

#include <array>
#include <stdint.h>

struct Color {
    uint8_t R, G, B;
    Color* Set(uint8_t r, uint8_t g, uint8_t b) {
        R = r;
        G = g;
        B = b;
        return this;
    }

    auto getData() const {
        struct {
            uint16_t data[3];
        } data { {
            uint16_t(0x0100 | (R << 4)),
            uint16_t(0x0100 | (G << 2 & 0xF0) | (R >> 4)),
            uint16_t(0x0100 | (B & 0xFC) | (G >> 6)),
        } };
        return data;
    }
};

// Основные цвета и фон
inline Color Background { 0, 0, 0 };
inline Color White { 255, 255, 255 };
inline Color Black { 0, 0, 0 };
inline Color Red { 255, 0, 0 };
inline Color Green { 0, 255, 0 };
inline Color Blue { 0, 0, 255 };

struct Size {
    uint8_t w {}, h {};
};

struct Point {
    uint8_t x {}, y {};

    constexpr Point operator+(const Point& l) const { return { uint8_t(x + l.x), uint8_t(y + l.y) }; }
    constexpr Point operator+(const Size& l) const { return { uint8_t(x + l.w), uint8_t(y + l.h) }; }
};

/*
     0.0
    {x,y}--------+
      |          |
      |          |
      +--------{w,h}
              +xw,+yh
*/

struct Rect {
    Point p;
    Size size;
    Point p1() const { return p; }
    Point p2() const { return p + size; }

    uint8_t left() const { return p.x; }
    uint8_t right() const { return p.x + size.w; }

    uint8_t top() const { return p.y; }
    uint8_t bottom() const { return p.y + size.h; }
};

class Buffer {
    enum { MaxSize = 256 };
    uint16_t data_[MaxSize];
    uint16_t size_ {};

public:
    void append(uint16_t val) {
        if (size_ < MaxSize)
            data_[size_++] = val;
    }
    void clear() { size_ = 0; }

    auto size() const { return size_; }
    auto data() const { return data_; }

    auto operator[](uint16_t i) { return (data_[i]); }
    auto operator[](uint16_t i) const { return (data_[i]); }

    auto begin() const { return data_; }
    auto begin() { return data_; }
    auto end() const { return data_ + size_; }
    auto end() { return data_ + size_; }
};

class SPFD54124B {
protected:
    SPFD54124B();
    void init();
    template <size_t N>
    void sendDma(const uint16_t (&data)[N]) const { sendDma(data, N); }
    void sendDma(const uint16_t* data, uint32_t size) const;

    void waitDma() const;
    void waitSpi() const;

    void setWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h) { setWindow({ { x, y }, { w, h } }); }
    void setWindow(Point point, Size size) { setWindow({ point, size }); }
    void setWindow(const Rect& rect);

    void sendPix(const Color& color); // вывод пикселя в установленную область
    void sendDat(uint8_t data); // отправить данные
    void sendCmd(uint8_t data); // отправить комманду

    void sendDatSft(uint8_t data); // отправить данные программный SPI
    void sendCmdSft(uint8_t data); // отправить комманду программный SPI

public:
    enum {
        Width = 128,
        Height = 160,
    };

    void clear();
};
