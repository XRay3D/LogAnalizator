#include "usbdevice.h"

#include <QDebug>

UsbDevice::UsbDevice(QObject* parent)
    : QObject(parent)
    , timer { new QTimer(this) } {

    device = NULL;
    buf[0] = 0x00;
    buf[1] = 0x37;
    memset((void*)&buf[2], 0x00, sizeof(buf) - 2);
    connect(timer, &QTimer::timeout, this, &UsbDevice::PollUSB);
    timer->start(1000);
}

UsbDevice::~UsbDevice() { Close(); }

bool UsbDevice::isConnected() const { return isConnected_; }

void UsbDevice::PollUSB() {
    if (!isConnected_) {
        device = hid_open(DEV_VID, DEV_PID, nullptr);
        if (device) {
            //            hid_set_nonblocking(device, true);
            isConnected_ = true;
            timer->stop();
        }
    } else {
    }
    qDebug() << device;
}

bool UsbDevice::Open() { return {}; }

void UsbDevice::Close() {
    hid_close(device);
    device = {};
    isConnected_ = {};
    timer->start(250);
}

int UsbDevice::Read(std::span<uint8_t> data) {
    auto ret = hid_read(device, data.data(), data.size());
    qDebug() << "Read ErrorMessage: " << ret << QString::fromWCharArray(hid_error(device));
    return ret; //> 0; // ret != -1;
}

int UsbDevice::Write(std::span<uint8_t> data) {
    auto ret = hid_write(device, data.data(), data.size());
    qDebug() << "Write ErrorMessage: " << ret << QString::fromWCharArray(hid_error(device));
    return ret; //> 0; // ret != -1;
}
