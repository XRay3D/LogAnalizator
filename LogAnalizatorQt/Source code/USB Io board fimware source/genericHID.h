// genericHID.h

#ifndef GENERICHID_H
#define GENERICHID_H

unsigned char ReceivedDataBuffer[64] @ 0x47F;
unsigned char ToSendDataBuffer[64] @ 0x4BF;

USB_HANDLE USBOutHandle = 0;
USB_HANDLE USBInHandle = 0;
BOOL blinkStatusValid = TRUE;

#endif