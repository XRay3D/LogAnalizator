#include "hid_pnp.h"

#include <QDebug>

HID_PnP::HID_PnP(QObject* parent)
    : QObject(parent)
    , timer { new QTimer(this) }
{

    device = NULL;
    buf[0] = 0x00;
    buf[1] = 0x37;
    memset((void*)&buf[2], 0x00, sizeof(buf) - 2);
    connect(timer, SIGNAL(timeout()), this, SLOT(PollUSB()));
    timer->start(1000);
}

HID_PnP::~HID_PnP()
{
    disconnect(timer, SIGNAL(timeout()), this, SLOT(PollUSB()));
}

void HID_PnP::PollUSB()
{
    if (isConnected == false) {
        device = hid_open(0x0483, 0xA210, NULL);
        if (device) {
            isConnected = true;
            hid_set_nonblocking(device, true);
        }
    } else {
        if (hid_write(device, buf, sizeof(buf)) == -1) {
            qDebug() << "error write";
            CloseDevice();
            return;
        }
        if (hid_read(device, buf, sizeof(buf)) == -1) {
            qDebug() << "error read";
            CloseDevice();
            return;
        }
    }
    qDebug() << device;
}

//void HID_PnP::asd(int i, bool r, bool g)
//{

//    buf[0] = 0x00;
//    buf[1] = 0x80;
//    memset((void*)&buf[2], 0x00, sizeof(buf) - 2);
//    buf[2] = i++;
//    potentiometerValue = i;
//    r ? buf[3] |= 0x80 : buf[3];
//    g ? buf[3] |= 0x40 : buf[3];

//    if (isConnected == false) {
//        device = hid_open(0x04D8, 0x0042, NULL);

//        if (device) {
//            isConnected = true;
//            hid_set_nonblocking(device, true);
//            timer->stop(); //->start(1);
//        }
//    } else {
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
//    }
//}

//void HID_PnP::qwe(char i, bool g, bool r)
//{
//    buf[0] = 0x00;
//    buf[1] = 0x80;
//    memset((void*)&buf[2], 0x00, sizeof(buf) - 2);
//    buf[2] = i++;
//    potentiometerValue = i;
//    r ? buf[3] |= 0x80 : buf[3];
//    g ? buf[3] |= 0x40 : buf[3];

//    if (isConnected == false) {
//        device = hid_open(0x04D8, 0x0042, NULL);
//        if (device) {
//            isConnected = true;
//            hid_set_nonblocking(device, true);
//        }
//    } else {
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
//    }
//}

void HID_PnP::CloseDevice()
{
    hid_close(device);
    device = {};
    isConnected = {};
    timer->start(250);
}
