#pragma once

#include "hidapi.h"
#include <QObject>
#include <QTimer>
#include <span>

#define MAX_STR 64

using Data = std::vector<uint8_t>;

class UsbDevice : public QObject {
    Q_OBJECT

    static constexpr uint32_t DEV_PID = 0xA210;
    static constexpr uint32_t DEV_VID = 0x0483;
    static constexpr uint32_t USBReadTimeOut = 5000;
    static constexpr uint32_t USBWriteTimeOut = 5000;

public:
    explicit UsbDevice(QObject* parent = 0);
    ~UsbDevice();

    bool isConnected() const;

    void PollUSB();

    bool Open();
    void Close();
    int Read(std::span<uint8_t> data);
    int Write(std::span<uint8_t> data);

private:
    bool isConnected_ {};

    hid_device_* device {};
    QTimer* timer {};
    unsigned char buf[MAX_STR];
};
