#pragma once

#include <vector>
#include <memory>

//C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace LibUsbDotNet { class UsbDevice; }
namespace LibUsbDotNet { class UsbEndpointWriter; }
namespace LibUsbDotNet { class UsbEndpointReader; }

/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2017, 2018
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


using namespace LibUsbDotNet;
using namespace LibUsbDotNet::Main;

namespace LogicAnalyzer
{
	class USB_device : public std::enable_shared_from_this<USB_device>
	{
	private:
		static const int DEV_VID = 0x0483;
		static const int DEV_PID = 0xA210;
		static const int USBWriteTimeOut = 5000;
		static const int USBReadTimeOut = 5000;

		static std::shared_ptr<UsbDevice> USBDev;
		static std::shared_ptr<UsbEndpointWriter> USBDevWriter;
		static std::shared_ptr<UsbEndpointReader> USBDevReader;


		//--------------------------------------------------------------------------------------------------------
	public:
		static bool Open();
		//--------------------------------------------------------------------------------------------------------
		static void Close();
		//--------------------------------------------------------------------------------------------------------
		static bool Write(std::vector<unsigned char> &Buffer);
		//--------------------------------------------------------------------------------------------------------
		static bool Read(std::vector<unsigned char> &Buffer);
	};
}
