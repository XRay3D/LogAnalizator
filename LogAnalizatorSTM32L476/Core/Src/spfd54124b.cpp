#include "spfd54124b.h"
#include "main.h"
#include "spi.h"

#include <span>

#define RES_Pin GPIOB
#define SET_RES GPIO_BSRR_BS4
#define RES_RES GPIO_BSRR_BR4

#define CS_Pin GPIOB
#define SET_CS GPIO_BSRR_BS12
#define RES_CS GPIO_BSRR_BR12

#define CLK_Pin GPIOB
#define SET_CLK GPIO_BSRR_BS10
#define RES_CLK GPIO_BSRR_BR10

#define SDA_Pin GPIOC
#define SET_SDA GPIO_BSRR_BS3
#define RES_SDA GPIO_BSRR_BR3

namespace CMD {
enum CMD_ : uint8_t { //SPFD54124B
    // clang-format off
    //6.2. SYSTEM COMMAND DESCRIPTION (Sheet 20)
    NOP =       0x00, // (Sheet 20)
    SWRESET =   0x01, // Software Reset(Sheet 20)
    RDDID =     0x04, // Read Display ID(Sheet 22)
    RDDST =     0x09, // Read Display Status (Sheet 23)
    RDDPM =     0x0A, // Read Display Power Mode(Sheet 25)
    RDDMADCTR = 0x0B, // Read Display MADCTR (Sheet 26)
    RDDCOLMOD = 0x0C, // Read Display Pixel Format (Sheet 27)
    RDDIM =     0x0D, // Read Display Image Mode (Sheet 28)
    RDDSM =     0x0E, // Read Display Signal Mode(Sheet 29)
    RDDSDR =    0x0F, // Read Display Self-Diagnostic Result (Sheet 30)
    SLPIN =     0x10, // Sleep In (Sheet 31)
    SLPOUT =    0x11, // Sleep Out(Sheet 33)
    PTLON =     0x12, // Partial Display Mode On (Sheet 35)
    NORON =     0x13, // Normal Display Mode On (Sheet 36)
    INVOFF =    0x20, // Display Inversion Off(Sheet 37)
    INVON =     0x21, // Display Inversion On (Sheet 38)
    GAMSET =    0x26, // Gamma Set(Sheet 39)
    DISPOFF =   0x28, // Display Off(Sheet 40)
    DISPON =    0x29, // Display On(Sheet 42)
    CASET =     0x2A, // Column Address Set(Sheet 44)
    RASET =     0x2B, // Row Address Set (Sheet 46)
    RAMWR =     0x2C, // Memory Write (Sheet 48)
    RGBSET =    0x2D, // Colour Setting for 4K, 65K and 262K (Sheet 50)
    RAMHD =     0x2E, // Memory Read(Sheet 51)
    PTLAR =     0x30, // Partial Area(Sheet 52)
    SCRLAR =    0x33, // Scroll Area(Sheet 54)
    TEOFF =     0x34, // Tearing Effect Line OFF (Sheet 58)
    TEON =      0x35, // Tearing Effect Line ON(Sheet 59)
    MADCTR =    0x36, // Memory Data Access Control (Sheet 60)
    VSCSAD =    0x37, // Vertical Scroll Start Address of RAM(Sheet 62)
    IDMOFF =    0x38, // Idle Mode Off(Sheet 64)
    IDMON =     0x39, // Idle Mode On (Sheet 65)
    COLMOD =    0x3A, // Interface Pixel Format(Sheet 67)
    RDID1 =     0xDA, // Read ID1 Value (Sheet 68)
    RDID2 =     0xDB, // Read ID2 Value (Sheet 69)
    RDID3 =     0xDC, // Read ID3 Value(Sheet 70)

    //6.3. PANEL COMMAND DESCRIPTION (Sheet 71)
    RGBCTR =    0xB0, // RGB signal control (Sheet 71)
    FRMCTR1 =   0xB1, // Frame Rate Control (In normal mode/ Full colors)(Sheet 73)
    FRMCTR2 =   0xB2, // Frame Rate Control (In Idle mode/ 8-colors)(Sheet 75)
    FRMCTR3 =   0xB3, // Frame Rate Control (In Partial mode/ full colors)(Sheet 77)
    INVCTR =    0xB4, // Display Inversion Control (Sheet 79)
    RGBBPCTR =  0xB5, // RGB Interface Blanking Porch setting(Sheet 80)
    DISSET5 =   0xB6, // Display Function set 5 (Sheet 81)
    PWCTR1 =    0xC0, // Power Control 1 (Sheet 83)
    PWCTR2 =    0xC1, // Power Control 2 (Sheet 85)
    PWCTR3 =    0xC2, // Power Control 3 (in Normal mode/ Full colors) (Sheet 86)
    PWCTR4 =    0xC3, // Power Control 4 (in Idle mode/ 8-colors) (Sheet 88)
    PWCTR5 =    0xC4, // Power Control 5 (in Partial mode/ full-colors) (Sheet 90)
    VMCTR1 =    0xC5, // VCOM Control 1 (Sheet 92)
    VMCTR2 =    0xC6, // VCOM Control 2 (Sheet 94)
    RDVMH =     0xC8, // Read the VCOMH Value NV memory (Sheet 96)
    WRID1 =     0xD0, // Write ID1 Value(Sheet 97)
    WRID2 =     0xD1, // Write ID2 Value(Sheet 98)
    WRID3 =     0xD2, // Write ID3 Value(Sheet 99)
    RDID4 =     0xD3, // Read the ID4 value(Sheet 100)
    NVFCTR1 =   0xD9, // NV Memory Function Controller 1 (Sheet 101)
    NVFCTR2 =   0xDE, // NV Memory Function Controller 2(Sheet 103)
    NVFCTR3 =   0xDF, // NV Memory Function Controller 3 (Sheet 104)
    GMCTRP1 =   0xE0, // Gamma Correction Characteristics Setting (Sheet 105)
    GMCTRP2 =   0xE1, // Gamma Correction Characteristics Setting (Sheet 107) ?? name is GMCTRP1
    // clang-format on
};
}

enum : uint16_t { DATA = 0x0100 };

SPFD54124B::SPFD54124B() { init(); }

void SPFD54124B::init() {
    // reset
    RES_Pin->BSRR = RES_RES;
    LL_mDelay(1);
    RES_Pin->BSRR = SET_RES;

    LL_DMA_DisableChannel(DMA_SPI2_TX_CH5);

    LL_SPI_EnableDMAReq_TX(SPI2);
    LL_SPI_Enable(SPI2);

    LL_DMA_ClearFlag_TC5(DMA1);
    LL_DMA_SetPeriphAddress(DMA_SPI2_TX_CH5, LL_SPI_DMA_GetRegAddr(SPI2));

    const uint16_t cfg[] {
        CMD::RGBBPCTR, // Порядок отсылки данных
        0x0107,
        0x0115,
        // Контраст 3F
        //        0x0025,
        //        0x013F,
        CMD::SLPOUT, // Выход из режима sleep
        CMD::VSCSAD,
        DATA,
        CMD::COLMOD,
        0x0106, //  pixel format 4=12,5=16,6=18
        CMD::DISPON,
        CMD::INVOFF,
        CMD::NORON,
    };
    sendDma(cfg);

    clear();
}

void SPFD54124B::sendDat(uint8_t data) {
    LL_SPI_TransmitData16(SPI2, DATA | data);
    waitSpi();
}

void SPFD54124B::sendCmd(uint8_t data) {
    LL_SPI_TransmitData16(SPI2, data);
    waitSpi();
}

void SPFD54124B::sendDma(const uint16_t* data, uint32_t size) const {
    if (!size)
        return;

    LL_DMA_SetDataLength(DMA_SPI2_TX_CH5, size);
    LL_DMA_SetMemoryAddress(DMA_SPI2_TX_CH5, uint32_t(data));
    LL_DMA_EnableChannel(DMA_SPI2_TX_CH5);
    waitDma();
}

void SPFD54124B::waitDma() const {
    while (!LL_DMA_IsActiveFlag_TC5(DMA1)) { }
    LL_DMA_ClearFlag_TC5(DMA1);
    LL_DMA_DisableChannel(DMA_SPI2_TX_CH5);
}

void SPFD54124B::waitSpi() const {
    while (LL_SPI_GetTxFIFOLevel(SPI2) == LL_SPI_TX_FIFO_FULL)
        continue;
}

void SPFD54124B::sendDatSft(uint8_t data) {
    // Включаем контроллер дисплея (низкий уровень активный)
    CS_Pin->BSRR = RES_CS;
    // Первый бит высокий уровень - данные
    SDA_Pin->BSRR = SET_SDA;
    // Подаем такт на CLK
    CLK_Pin->BSRR = SET_CLK;
    CLK_Pin->BSRR = RES_CLK;
    for (char i = 0; i < 8; i++) {
        if (data & (Width >> i))
            SDA_Pin->BSRR = SET_SDA;
        else
            SDA_Pin->BSRR = RES_SDA;
        // Подаем такт на CLK
        CLK_Pin->BSRR = SET_CLK;
        CLK_Pin->BSRR = RES_CLK;
    }
    // Отключаем контроллер дисплея
    CS_Pin->BSRR = SET_CS;
}

void SPFD54124B::sendCmdSft(uint8_t data) {
    // Включаем контроллер дисплея (низкий уровень активный)
    CS_Pin->BSRR = RES_CS;
    // Первый бит низкий уровень - команда
    SDA_Pin->BSRR = RES_SDA;
    // Подаем такт на CLK
    CLK_Pin->BSRR = SET_CLK;
    CLK_Pin->BSRR = RES_CLK;
    for (char i = 0; i < 8; i++) {
        if (data & (Width >> i))
            SDA_Pin->BSRR = SET_SDA;
        else
            SDA_Pin->BSRR = RES_SDA;
        // Подаем такт на CLK
        CLK_Pin->BSRR = SET_CLK;
        CLK_Pin->BSRR = RES_CLK;
    }
    // Отключаем контроллер дисплея
    CS_Pin->BSRR = SET_CS;
}

void SPFD54124B::setWindow(const Rect& rect) {
    uint16_t setup[] {
        CMD::CASET,
        DATA,
        uint16_t(DATA | uint8_t(2 + rect.left())),
        DATA,
        uint16_t(DATA | uint8_t(2 + rect.right() - 1)),

        CMD::RASET,
        DATA,
        uint16_t(DATA | uint8_t(1 + rect.top())),
        DATA,
        uint16_t(DATA | uint8_t(1 + rect.bottom())),

        CMD::RAMWR,
    };
    sendDma(setup, std::size(setup));
}

void SPFD54124B::sendPix(const Color& color) { sendDma(color.getData().data); }

void SPFD54124B::clear() // заливка чёрным
{
    setWindow({}, { Width, Height });
    LL_DMA_SetMemoryIncMode(DMA_SPI2_TX_CH5, LL_DMA_MEMORY_NOINCREMENT);
    constexpr uint16_t black = DATA;
    sendDma(&black, Width * Height * 3);
    LL_DMA_SetMemoryIncMode(DMA_SPI2_TX_CH5, LL_DMA_MEMORY_INCREMENT);
}
