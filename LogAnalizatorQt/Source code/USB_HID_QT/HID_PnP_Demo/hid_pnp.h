#ifndef HID_PNP_H
#define HID_PNP_H

#include <QObject>
#include <QTimer>
#include "../HIDAPI/hidapi.h"

#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STR 65

class HID_PnP : public QObject
{
    Q_OBJECT
public:
    explicit HID_PnP(QObject *parent = 0);
    ~HID_PnP();

signals:
    void hid_comm_update(bool isConnected, int potentiometerValue);

public slots:
    void PollUSB();
    void asd(int i, bool r, bool g);
    void qwe(char i, bool g, bool r);

private:
    bool isConnected;
    int potentiometerValue;

    hid_device *device;
    QTimer *timer;

    unsigned char buf[MAX_STR];

    void CloseDevice();
};

#endif // HID_PNP_H
