#include "Analyzer_USB.h"
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_custom_hid_if.h"
#include "usbd_customhid.h"

extern "C" void ReceiveData(uint8_t* data)
{
    Analyzer.ReceiveData(data);
}

extern USBD_HandleTypeDef hUsbDeviceFS;

uint8_t* Analyzer_USB::GetPacket() { return USBDataBuf; }

USB_Status Analyzer_USB::status() { return USB_Status(USBStatus); }

bool Analyzer_USB::IsReadyToSend()
{
    return ((USBD_CUSTOM_HID_HandleTypeDef*)hUsbDeviceFS.pClassData)->state == CUSTOM_HID_IDLE;
}

void Analyzer_USB::clearStatus(USB_Status StatusFlag) { USBStatus &= ~StatusFlag; }

void Analyzer_USB::ReceiveData(uint8_t* data)
{
    memcpy(USBDataBuf, data, USB_MESSAGE_LENGTH);
    switch (USBDataBuf[1]) {
    case USB_CMD_START_CAPTURE:
        USBStatus |= USB_Status::START_CAPTURE;
        break;
    default:
        break;
    }
}

int8_t Analyzer_USB::TransmitData(uint8_t* data)
{
    int8_t ret = { -1 };
    if (IsReadyToSend() && hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
        data[0] = 2;
        ret = USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, data, USB_MESSAGE_LENGTH);
    }
    return ret;
}
