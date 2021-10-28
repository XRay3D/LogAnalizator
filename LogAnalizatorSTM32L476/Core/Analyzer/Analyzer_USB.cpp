#include "Analyzer_USB.h"
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

uint8_t* Analyzer_USB::GetPacket() { return USBDataBuf; }

USB_Status Analyzer_USB::status() { return USB_Status(USBStatus); }

uint8_t* Analyzer_USB::IsReadyToSend() { return &PrevXferDone; }

void Analyzer_USB::clearStatus(USB_Status StatusFlag) { USBStatus &= ~StatusFlag; }

void Analyzer_USB::RecPacket(uint8_t* Packet) {
    memcpy(USBDataBuf, Packet, USB_MESSAGE_LENGTH);
    switch (USBDataBuf[1]) {
    case USB_CMD_START_CAPTURE:
        USBStatus |= USB_Status::START_CAPTURE;
        break;
    default:
        break;
    }
}

void Analyzer_USB::SendData(uint8_t* Data) {
    if ((PrevXferDone) && (hUsbDeviceFS.dev_config_status == USBD_STATE_CONFIGURED)) {
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, Data, USB_MESSAGE_LENGTH);
        PrevXferDone = 0;
    }
}
