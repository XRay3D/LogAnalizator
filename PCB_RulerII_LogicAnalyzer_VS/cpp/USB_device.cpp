#include "USB_device.h"

using namespace LibUsbDotNet;
using namespace LibUsbDotNet::Main;

namespace LogicAnalyzer
{

std::shared_ptr<UsbDevice> USB_device::USBDev;
std::shared_ptr<UsbEndpointWriter> USB_device::USBDevWriter;
std::shared_ptr<UsbEndpointReader> USB_device::USBDevReader;

	bool USB_device::Open()
	{
		std::shared_ptr<UsbDeviceFinder> DeviceFinder = std::make_shared<UsbDeviceFinder>(DEV_VID, DEV_PID);

		USBDev = UsbDevice::OpenUsbDevice(DeviceFinder);

		if (USBDev == nullptr)
		{
			return false;
		}

		std::shared_ptr<IUsbDevice> wholeUsbDevice = std::dynamic_pointer_cast<IUsbDevice>(USBDev);
		if (!ReferenceEquals(wholeUsbDevice, nullptr))
		{
			wholeUsbDevice->SetConfiguration(1);
			wholeUsbDevice->ClaimInterface(0);
		}

		USBDevReader = USBDev->OpenEndpointReader(ReadEndpointID::Ep01);
		USBDevWriter = USBDev->OpenEndpointWriter(WriteEndpointID::Ep01);

		return true;
	}

	void USB_device::Close()
	{
		if (USBDev != nullptr)
		{
			if (USBDev->IsOpen)
			{
				std::shared_ptr<IUsbDevice> wholeUsbDevice = std::dynamic_pointer_cast<IUsbDevice>(USBDev);
				if (!ReferenceEquals(wholeUsbDevice, nullptr))
				{
					wholeUsbDevice->ReleaseInterface(0);
				}

				USBDev->Close();
			}

			USBDev.reset();

			UsbDevice::Exit();
		}
	}

	bool USB_device::Write(std::vector<unsigned char> &Buffer)
	{
		ErrorCode USB_Error = ErrorCode::None;
		int BytesWritten = 0;

		if (USBDevWriter == nullptr)
		{
			return false;
		}

		USB_Error = USBDevWriter->Write(Buffer, USBWriteTimeOut, BytesWritten);

		if ((USB_Error != ErrorCode::None) || (BytesWritten != Buffer.size()))
		{
			return false;
		}

		return true;
	}

	bool USB_device::Read(std::vector<unsigned char> &Buffer)
	{
		ErrorCode USB_Error = ErrorCode::None;
		int BytesRead = 0;

		if (USBDevReader == nullptr)
		{
			return false;
		}

		USB_Error = USBDevReader->Read(Buffer, USBReadTimeOut, BytesRead);

		if ((USB_Error != ErrorCode::None) || (BytesRead != Buffer.size()))
		{
			return false;
		}

		return true;
	}
}
