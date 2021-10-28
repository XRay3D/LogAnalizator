/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2017
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/

#ifndef __ANALYZER_USB_H
#define __ANALYZER_USB_H

#include "analyzer.h"

#define USB_MESSAGE_LENGTH 0x40 //длина пакета USB, максимум 64 байта

//#define USB_REPORT_ID_POS 0
//#define USB_CMD_POS 1 //индекс команды в посылке

//#define USB_CMD_ID 1

//команды
#define USB_CMD_START_CAPTURE 1

//#define CAPTURE_SYNC_OFFSET (USB_CMD_POS + 1)
//#define CAPTURE_TIM_PSC_OFFSET (CAPTURE_SYNC_OFFSET + 1)
//#define CAPTURE_TIM_ARR_OFFSET (CAPTURE_TIM_PSC_OFFSET + 1)
//#define CAPTURE_SAMPLE_OFFSET (CAPTURE_TIM_ARR_OFFSET + 1)
//#define CAPTURE_TRIG_ENABLE_OFFSET (CAPTURE_SAMPLE_OFFSET + 2)
//#define CAPTURE_TRIG_MODE_OFFSET (CAPTURE_TRIG_ENABLE_OFFSET + 1)
//#define CAPTURE_TRIG_SET_OFFSET (CAPTURE_TRIG_MODE_OFFSET + 1)

#pragma pack(push, 1)
union CaptureT {
    struct {
        uint8_t USB_REPORT_ID_POS;
        uint8_t USB_CMD_POS;
        uint8_t CAPTURE_SYNC_OFFSET;
        uint8_t CAPTURE_TIM_PSC_OFFSET;
        uint8_t CAPTURE_TIM_ARR_OFFSET;
        uint16_t CAPTURE_SAMPLE_OFFSET;
        uint8_t CAPTURE_TRIGGER_ENABLE_OFFSET;
        uint8_t CAPTURE_TRIGGER_MODE_OFFSET;
        uint8_t CAPTURE_TRIGGER_SET_OFFSET;
        uint8_t CAPTURE_TRIGGER_MASK;

    };
    uint8_t array[64] {};
};
#pragma pack(pop)

enum CAPTURE : uint8_t {
    SYNC_INTERNAL = 0,
    SYNC_EXTERNAL = 1,
    TRIG_BYTES_COUNT = 4,
    TRIG_DISABLE = 0,
    TRIG_ENABLE = 1,
    TRIG_MODE_CHANNELS = 0,
    TRIG_MODE_EXT_LINES = 1,
    TRIG_EXT_BYTES_COUNT = 1,
};

enum TRIGGER : uint8_t {
    NONE = 0,
    LOW_LEVEL = 1,
    HIGH_LEVEL = 2,
    RISING_EDGE = 3,
    FALLING_EDGE = 4,
    ANY_EDGE = 5,
};

//Status
enum USB_Status : uint8_t {
    IDLE = 0,
    START_CAPTURE = (1 << 0)
};

struct Analyzer_USB {
    uint8_t* GetPacket();
    USB_Status status();
    uint8_t* IsReadyToSend();
    void clearStatus(USB_Status StatusFlag);
    void RecPacket(uint8_t* Packet);
    void SendData(uint8_t* Data);

private:
    uint8_t PrevXferDone { 1 };
    uint8_t USBStatus { USB_Status::IDLE };
    uint8_t USBDataBuf[USB_MESSAGE_LENGTH] {};
};

inline Analyzer_USB Analyzer;

#endif //__ANALYZER_USB_H
