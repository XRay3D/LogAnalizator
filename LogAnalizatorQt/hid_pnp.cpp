#include "hid_pnp.h"

#include <QDebug>

UsbDevice::UsbDevice(QObject* parent)
    : QObject(parent)
    , timer { new QTimer(this) } {

    device = NULL;
    buf[0] = 0x00;
    buf[1] = 0x37;
    memset((void*)&buf[2], 0x00, sizeof(buf) - 2);
    connect(timer, SIGNAL(timeout()), this, SLOT(PollUSB()));
    timer->start(1000);
}

UsbDevice::~UsbDevice() {
}

bool UsbDevice::isConnected() const { return isConnected_; }

void UsbDevice::PollUSB() {
    if (!isConnected_) {
        device = hid_open(DEV_VID, DEV_PID, nullptr);
        if (device) {
            isConnected_ = true;
            hid_set_nonblocking(device, true);
        }
        timer->stop();
    } else {
        //        if (hid_write(device, buf, sizeof(buf)) == -1) {
        //            qDebug() << "error write";
        //            CloseDevice();
        //            return;
        //        }
        //        if (hid_read(device, buf, sizeof(buf)) == -1) {
        //            qDebug() << "error read";
        //            CloseDevice();
        //            return;
        //        }
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

bool UsbDevice::Read(std::span<uint8_t> data) {
    auto ret = hid_read(device, data.data(), data.size());
    qDebug("Read %d\n", ret);
    return ret != -1;
}

bool UsbDevice::Write(const std::span<uint8_t> data) {
    auto ret = hid_write(device, data.data(), data.size());
    qDebug("Write %d\n", ret);
    return ret != -1;
}
