#include "Nokia1616_LCD_lib.h"

Nokia1616_LCD::Nokia1616_LCD() { }

void Nokia1616_LCD::print(const char c, Point p, const Color& color)
{
    setWindow({ p, { 6, 8 } }); // ширина char'а 5 + 1 межбуквенное расстояние, высота 8 (кол-во бит)
    uint8_t u8c = c - (c > Width ? 64 : 0) - 32;
    for (uint8_t j = 0; j < 8; j++) // вывод по строкам
    {
        sendPix(Background); // | "разделитель" символов
        const uint8_t mask = 1 << j;
        const uint8_t* arr = font5x8[u8c];
        for (uint8_t i = 0; i < 5; i++)
            sendPix(arr[i] & mask ? color : Background);
    }
}

void Nokia1616_LCD::print(const std::string_view str, Point p, const Color& color)
{
    //    constexpr bool isUtf8 { sizeof("я") > 2 };

    for (auto&& chr : str) {
        uint16_t i = chr;
        //        //        if constexpr (isUtf8)
        //        if (i & 0x80) { // if UTF8
        //            i &= 0b00011111;
        //            i <<= 6;
        //            i |= *str++ & 0b00111111;
        //            i -= 1040 - 192;
        //        }

        print(char(i < 32 ? 32 : i), p, color);
        p.x += 6;
    }
    //    while (*str != 0) {
    //        uint16_t i = *str++;
    //        //        if constexpr (isUtf8)
    //        if (i & 0x80) { // if UTF8
    //            i &= 0b00011111;
    //            i <<= 6;
    //            i |= *str++ & 0b00111111;
    //            i -= 1040 - 192;
    //        }
    //        print(char(i < 32 ? 32 : i), x, y, color);
    //        x += 6;
    //    }
}
