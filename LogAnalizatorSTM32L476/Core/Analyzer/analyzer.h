#pragma once

#include <stdint.h>

#pragma pack(push, 1)

union CaptureT {
    struct {
        uint8_t USB_REPORT_ID_POS;
        uint8_t USB_CMD_POS;
        uint8_t SYNC;
        uint16_t TIM_PSC;
        uint32_t TIM_ARR;
        uint16_t SAMPLE;
        uint8_t TRIGGER_ENABLE;
        uint8_t TRIGGER_MODE;
        uint8_t TRIGGER_SET;
        uint8_t TRIGGER_MASK;
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

enum {
    USB_MESSAGE_LENGTH = 64, //because first is REPORT_ID //64,
    CAPTURE_BUFFER_SIZE = 16384 + 1
};

enum class Status : uint8_t {
    IDLE,
    CAPTURE,
    COMPLETE,
    ERROR,
};

class AnalyzerUSB {

public:
    void pool(void* lcd_);
    void Init();
    uint8_t* GetPacket();
    bool IsReadyToSend();
    int8_t TransmitData(uint8_t* data);
    void ReceiveData(uint8_t* data);

    Status status() const;
    void setStatus(volatile Status status);

    uint16_t EdgeTrigChnls;
    uint16_t LevelTrigValue;
    uint16_t LevelTrigChnls;

private:
    volatile Status status_ { Status::IDLE };
    uint8_t usbData[USB_MESSAGE_LENGTH] {};
    uint8_t CaptureBuff[CAPTURE_BUFFER_SIZE + 1] {};
};

inline AnalyzerUSB Analyzer;
