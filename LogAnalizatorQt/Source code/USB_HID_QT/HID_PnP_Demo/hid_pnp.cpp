#include "hid_pnp.h"

#include <QDebug>

HID_PnP::HID_PnP(QObject *parent) : QObject(parent) {
    isConnected = false;
    potentiometerValue = 0;

    device = NULL;
    buf[0] = 0x00;
    buf[1] = 0x37;
    memset((void*)&buf[2], 0x00, sizeof(buf) - 2);
    
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(PollUSB()));

    //timer->start(25000000);
}

HID_PnP::~HID_PnP() {
    disconnect(timer, SIGNAL(timeout()), this, SLOT(PollUSB()));
}

void HID_PnP::PollUSB()
{
    //static long ii;
    static char i;

    buf[0] = 0x00/*0x00*/;
    buf[1] = 0x80;

    memset((void*)&buf[2], 0x00, sizeof(buf) - 2);
    buf[2] = i++;potentiometerValue=i;
    buf[3] = 0xFF;

    if (isConnected == false) {
        device = hid_open(0x04D8, 0x0042, NULL);

        if (device) {
            isConnected = true;
            hid_set_nonblocking(device, true);
            //timer->start(1000000000000000);
        }
    }
    else
    {
        if (hid_write(device, buf, sizeof(buf)) == -1)
        {
            qDebug()<<"error write";
            CloseDevice();
            return;
        }
        if(hid_read(device, buf, sizeof(buf)) == -1)
        {
            qDebug()<<"error read";
            CloseDevice();
            return;
        }

        //qDebug()<<ii++<<buf[0]<<buf[1]<<buf[2]<<buf[3]<<buf[4]<<buf[5];//<<buf[6]<<buf[7]<<buf[8]<<buf[9]<<buf[10]<<buf[11]<<buf[12]<<buf[13]<<buf[14]<<buf[15]<<buf[16]<<buf[17]<<buf[18]<<buf[19]<<buf[20]<<buf[21]<<buf[22]<<buf[23]<<buf[24]<<buf[25]<<buf[26]<<buf[27]<<buf[28]<<buf[29]<<buf[30]<<buf[31]<<buf[32]<<buf[33]<<buf[34]<<buf[35]<<buf[36]<<buf[37]<<buf[38]<<buf[39]<<buf[40]<<buf[41]<<buf[42]<<buf[43]<<buf[44]<<buf[45]<<buf[46]<<buf[47]<<buf[48]<<buf[49]<<buf[50]<<buf[51]<<buf[52]<<buf[53]<<buf[54]<<buf[55]<<buf[56]<<buf[57]<<buf[58]<<buf[59]<<buf[60]<<buf[61]<<buf[62]<<buf[63	];

        if(buf[0] == 0x84 /*0x37*/) {
            potentiometerValue = buf[3]<<8 + buf[4];
            // qDebug()<<potentiometerValue;
        }
    }

    hid_comm_update(isConnected, potentiometerValue);
}

void HID_PnP::asd(int i,bool r,bool g)
{

    buf[0] = 0x00;
    buf[1] = 0x80;
    memset((void*)&buf[2], 0x00, sizeof(buf) - 2);
    buf[2] = i++;potentiometerValue=i;
    r ? buf[3] |= 0x80 : buf[3] ;
    g ? buf[3] |= 0x40 : buf[3] ;

    if (isConnected == false) {
        device = hid_open(0x04D8, 0x0042, NULL);

        if (device) {
            isConnected = true;
            hid_set_nonblocking(device, true);
            timer->stop();//->start(1);
        }
    }
    else
    {
        if (hid_write(device, buf, sizeof(buf)) == -1)
        {
            qDebug()<<"error write";
            CloseDevice();
            return;
        }
        if(hid_read(device, buf, sizeof(buf)) == -1)
        {
            qDebug()<<"error read";
            CloseDevice();
            return;
        }
    }
}

void HID_PnP::qwe(char i,bool g,bool r)
{
    buf[0] = 0x00;
    buf[1] = 0x80;
    memset((void*)&buf[2], 0x00, sizeof(buf) - 2);
    buf[2] = i++;potentiometerValue=i;
    r ? buf[3] |= 0x80 : buf[3] ;
    g ? buf[3] |= 0x40 : buf[3] ;

    if (isConnected == false) {
        device = hid_open(0x04D8, 0x0042, NULL);
        if (device) {
            isConnected = true;
            hid_set_nonblocking(device, true);
        }
    }
    else
    {
        if (hid_write(device, buf, sizeof(buf)) == -1)
        {
            qDebug()<<"error write";
            CloseDevice();
            return;
        }
        if(hid_read(device, buf, sizeof(buf)) == -1)
        {
            qDebug()<<"error read";
            CloseDevice();
            return;
        }
    }
}


void HID_PnP::CloseDevice()
{
    hid_close(device);
    device = NULL;
    isConnected = false;
    potentiometerValue = 0;
    hid_comm_update(isConnected,  potentiometerValue);
    timer->start(250);
}
