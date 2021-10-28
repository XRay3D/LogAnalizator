#pragma once
#include <stdint.h>
// description General purpose I/O
// groupName GPIO
inline volatile struct GPIOA_ {
    union alignas(4) CRL_{
        struct{
            unsigned MODE0 : 2; // 0 Port n.0 mode bits
            unsigned CNF0 : 2; // 2 Port n.0 configuration bits
            unsigned MODE1 : 2; // 4 Port n.1 mode bits
            unsigned CNF1 : 2; // 6 Port n.1 configuration bits
            unsigned MODE2 : 2; // 8 Port n.2 mode bits
            unsigned CNF2 : 2; // 10 Port n.2 configuration bits
            unsigned MODE3 : 2; // 12 Port n.3 mode bits
            unsigned CNF3 : 2; // 14 Port n.3 configuration bits
            unsigned MODE4 : 2; // 16 Port n.4 mode bits
            unsigned CNF4 : 2; // 18 Port n.4 configuration bits
            unsigned MODE5 : 2; // 20 Port n.5 mode bits
            unsigned CNF5 : 2; // 22 Port n.5 configuration bits
            unsigned MODE6 : 2; // 24 Port n.6 mode bits
            unsigned CNF6 : 2; // 26 Port n.6 configuration bits
            unsigned MODE7 : 2; // 28 Port n.7 mode bits
            unsigned CNF7 : 2; // 30 Port n.7 configuration bits
        };
        uint32_t U32;
    };
    union alignas(4) CRH_{
        struct{
            unsigned MODE8 : 2; // 0 Port n.8 mode bits
            unsigned CNF8 : 2; // 2 Port n.8 configuration bits
            unsigned MODE9 : 2; // 4 Port n.9 mode bits
            unsigned CNF9 : 2; // 6 Port n.9 configuration bits
            unsigned MODE10 : 2; // 8 Port n.10 mode bits
            unsigned CNF10 : 2; // 10 Port n.10 configuration bits
            unsigned MODE11 : 2; // 12 Port n.11 mode bits
            unsigned CNF11 : 2; // 14 Port n.11 configuration bits
            unsigned MODE12 : 2; // 16 Port n.12 mode bits
            unsigned CNF12 : 2; // 18 Port n.12 configuration bits
            unsigned MODE13 : 2; // 20 Port n.13 mode bits
            unsigned CNF13 : 2; // 22 Port n.13 configuration bits
            unsigned MODE14 : 2; // 24 Port n.14 mode bits
            unsigned CNF14 : 2; // 26 Port n.14 configuration bits
            unsigned MODE15 : 2; // 28 Port n.15 mode bits
            unsigned CNF15 : 2; // 30 Port n.15 configuration bits
        };
        uint32_t U32;
    };
    union alignas(4) IDR_{
        const struct{
            unsigned IDR0 : 1; // 0 Port input data
            unsigned IDR1 : 1; // 1 Port input data
            unsigned IDR2 : 1; // 2 Port input data
            unsigned IDR3 : 1; // 3 Port input data
            unsigned IDR4 : 1; // 4 Port input data
            unsigned IDR5 : 1; // 5 Port input data
            unsigned IDR6 : 1; // 6 Port input data
            unsigned IDR7 : 1; // 7 Port input data
            unsigned IDR8 : 1; // 8 Port input data
            unsigned IDR9 : 1; // 9 Port input data
            unsigned IDR10 : 1; // 10 Port input data
            unsigned IDR11 : 1; // 11 Port input data
            unsigned IDR12 : 1; // 12 Port input data
            unsigned IDR13 : 1; // 13 Port input data
            unsigned IDR14 : 1; // 14 Port input data
            unsigned IDR15 : 1; // 15 Port input data
        };
        uint32_t U32;
    };
    union alignas(4) ODR_{
        struct{
            unsigned ODR0 : 1; // 0 Port output data
            unsigned ODR1 : 1; // 1 Port output data
            unsigned ODR2 : 1; // 2 Port output data
            unsigned ODR3 : 1; // 3 Port output data
            unsigned ODR4 : 1; // 4 Port output data
            unsigned ODR5 : 1; // 5 Port output data
            unsigned ODR6 : 1; // 6 Port output data
            unsigned ODR7 : 1; // 7 Port output data
            unsigned ODR8 : 1; // 8 Port output data
            unsigned ODR9 : 1; // 9 Port output data
            unsigned ODR10 : 1; // 10 Port output data
            unsigned ODR11 : 1; // 11 Port output data
            unsigned ODR12 : 1; // 12 Port output data
            unsigned ODR13 : 1; // 13 Port output data
            unsigned ODR14 : 1; // 14 Port output data
            unsigned ODR15 : 1; // 15 Port output data
        };
        uint32_t U32;
    };
    union alignas(4) BSRR_{
        struct{
            unsigned BS0 : 1; // 0 Set bit 0
            unsigned BS1 : 1; // 1 Set bit 1
            unsigned BS2 : 1; // 2 Set bit 1
            unsigned BS3 : 1; // 3 Set bit 3
            unsigned BS4 : 1; // 4 Set bit 4
            unsigned BS5 : 1; // 5 Set bit 5
            unsigned BS6 : 1; // 6 Set bit 6
            unsigned BS7 : 1; // 7 Set bit 7
            unsigned BS8 : 1; // 8 Set bit 8
            unsigned BS9 : 1; // 9 Set bit 9
            unsigned BS10 : 1; // 10 Set bit 10
            unsigned BS11 : 1; // 11 Set bit 11
            unsigned BS12 : 1; // 12 Set bit 12
            unsigned BS13 : 1; // 13 Set bit 13
            unsigned BS14 : 1; // 14 Set bit 14
            unsigned BS15 : 1; // 15 Set bit 15
            unsigned BR0 : 1; // 16 Reset bit 0
            unsigned BR1 : 1; // 17 Reset bit 1
            unsigned BR2 : 1; // 18 Reset bit 2
            unsigned BR3 : 1; // 19 Reset bit 3
            unsigned BR4 : 1; // 20 Reset bit 4
            unsigned BR5 : 1; // 21 Reset bit 5
            unsigned BR6 : 1; // 22 Reset bit 6
            unsigned BR7 : 1; // 23 Reset bit 7
            unsigned BR8 : 1; // 24 Reset bit 8
            unsigned BR9 : 1; // 25 Reset bit 9
            unsigned BR10 : 1; // 26 Reset bit 10
            unsigned BR11 : 1; // 27 Reset bit 11
            unsigned BR12 : 1; // 28 Reset bit 12
            unsigned BR13 : 1; // 29 Reset bit 13
            unsigned BR14 : 1; // 30 Reset bit 14
            unsigned BR15 : 1; // 31 Reset bit 15
        };
        uint32_t U32;
    };
    union alignas(4) BRR_{
        struct{
            unsigned BR0 : 1; // 0 Reset bit 0
            unsigned BR1 : 1; // 1 Reset bit 1
            unsigned BR2 : 1; // 2 Reset bit 1
            unsigned BR3 : 1; // 3 Reset bit 3
            unsigned BR4 : 1; // 4 Reset bit 4
            unsigned BR5 : 1; // 5 Reset bit 5
            unsigned BR6 : 1; // 6 Reset bit 6
            unsigned BR7 : 1; // 7 Reset bit 7
            unsigned BR8 : 1; // 8 Reset bit 8
            unsigned BR9 : 1; // 9 Reset bit 9
            unsigned BR10 : 1; // 10 Reset bit 10
            unsigned BR11 : 1; // 11 Reset bit 11
            unsigned BR12 : 1; // 12 Reset bit 12
            unsigned BR13 : 1; // 13 Reset bit 13
            unsigned BR14 : 1; // 14 Reset bit 14
            unsigned BR15 : 1; // 15 Reset bit 15
        };
        uint32_t U32;
    };
    union alignas(4) LCKR_{
        struct{
            unsigned LCK0 : 1; // 0 Port A Lock bit 0
            unsigned LCK1 : 1; // 1 Port A Lock bit 1
            unsigned LCK2 : 1; // 2 Port A Lock bit 2
            unsigned LCK3 : 1; // 3 Port A Lock bit 3
            unsigned LCK4 : 1; // 4 Port A Lock bit 4
            unsigned LCK5 : 1; // 5 Port A Lock bit 5
            unsigned LCK6 : 1; // 6 Port A Lock bit 6
            unsigned LCK7 : 1; // 7 Port A Lock bit 7
            unsigned LCK8 : 1; // 8 Port A Lock bit 8
            unsigned LCK9 : 1; // 9 Port A Lock bit 9
            unsigned LCK10 : 1; // 10 Port A Lock bit 10
            unsigned LCK11 : 1; // 11 Port A Lock bit 11
            unsigned LCK12 : 1; // 12 Port A Lock bit 12
            unsigned LCK13 : 1; // 13 Port A Lock bit 13
            unsigned LCK14 : 1; // 14 Port A Lock bit 14
            unsigned LCK15 : 1; // 15 Port A Lock bit 15
            unsigned LCKK : 1; // 16 Lock key
        };
        uint32_t U32;
    };

    CRL_ CRL;// Port configuration register low (GPIOn_CRL)
    CRH_ CRH;// Port configuration register high (GPIOn_CRL)
    IDR_ IDR;// Port input data register (GPIOn_IDR)
    ODR_ ODR;// Port output data register (GPIOn_ODR)
    BSRR_ BSRR;// Port bit set/reset register (GPIOn_BSRR)
    BRR_ BRR;// Port bit reset register (GPIOn_BRR)
    LCKR_ LCKR;// Port configuration lock register
}&_GPIOA=*(GPIOA_*)0x40010800;

// description 
// groupName 
inline volatile struct GPIOB_ {

}&_GPIOB=*(GPIOB_*)0x40010C00;

// description 
// groupName 
inline volatile struct GPIOC_ {

}&_GPIOC=*(GPIOC_*)0x40011000;

// description 
// groupName 
inline volatile struct GPIOD_ {

}&_GPIOD=*(GPIOD_*)0x40011400;

// description 
// groupName 
inline volatile struct GPIOE_ {

}&_GPIOE=*(GPIOE_*)0x40011800;

// description 
// groupName 
inline volatile struct GPIOF_ {

}&_GPIOF=*(GPIOF_*)0x40011C00;

// description 
// groupName 
inline volatile struct GPIOG_ {

}&_GPIOG=*(GPIOG_*)0x40012000;

