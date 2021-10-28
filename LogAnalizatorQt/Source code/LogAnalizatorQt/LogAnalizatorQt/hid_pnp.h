#ifndef HID_PNP_H
#define HID_PNP_H

#include "HIDAPI/hidapi.h"
#include <QObject>
#include <QTimer>

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define MAX_STR 64

class HID_PnP : public QObject {
    Q_OBJECT
public:
    explicit HID_PnP(QObject* parent = 0);
    ~HID_PnP();

signals:

public slots:
    void PollUSB();

private:
    bool isConnected {};

    hid_device* device {};
    QTimer* timer {};

    unsigned char buf[MAX_STR];

    void CloseDevice();
};

#endif // HID_PNP_H
