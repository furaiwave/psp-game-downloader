#pragma once

#include <Windows.h>
#include <winusb.h>
#include <SetupAPI.h>
#include <string>
#include <vector>
#include <memeory>

namespace psp {


	struct USBDeviceInfo {
		std::string devicePath;
		std::string deviceInstanceId;
		USHORT vendorID;
		USHORT productID;
		std::string manufacturer;
		std::string product;
		std::string serialNumber;
		UCHAR interfaceNumber;
	};

	struct USBConfig {
		UCHAR constgurationValue;
		UCHAR interfaceNumber;
		UCHAR altenateSetting;
		UCHAR inEndpoint;
		UCHAR outEndpoinbt;
		ULONG maxPacketSize;
		ULONG timout;
	};

	class WinUSBConnection {
	public:
		WinUSBConnection();
		~WinUSBConnection();

		bool connect(const std::string& devicePath);
		bool connect(USHORT vendorID, USHORT productID);
		void disconnect();
		bool isConnected() const;

		static std::vector<USBDeviceInfo> findDevices(USHORT vendorID, USHORT productID);
		static USBDeviceInfo findFirstDevice(USHORT vendoeID, USHORT productID);

		bool write(const void* buffer, ULONG size, ULONG* bytesWritten);
		bool read(void* buffer, ULONG size, ULONG* bytesRead);

		bool bulkWrite(UCHAR endpoint, const void* buffer, ULONG size, ULONG* bytesWritten);
		bool bulkRead(UCHAR endpoint, void* buffer, ULONG size, ULONG* bytesRead);

		bool controlTransfer(
			UCHAR requestType,
			UCHAR request,
			USHORT value,
			USHORT index,
			void* buffer,
			ULONG size,
			ULONG* bytesTransferred
		);

		bool interrptRead(UCHAR endpoint, void* buffer, ULONG size, ULONG* bytesRead);
		bool interrputWrite(UCHAR endpoint, const void* buffer, ULONG size, ULONG* bytesWritten);

		bool setConfiguration(const USBConfig& config);
		bool setPipePolicy(UCHAR endpoint, ULONG policyType, ULONG value);
		bool setInterfaceAlternateSetting(UCHAR alternateSetting);
		bool resetPipe(UCHAR endpoint);
		bool abortPipe(UCHAR endpoint);
		bool flushPipe(UCHAR endpoint);

		USBDeviceInfo getDeviceInfo() const;
		bool getDeviceDescriptor(USB_DEVICE_DESCRIPTOR* descriptor);
		bool getConfigDescriptor(USB_CONFIGURATION_DESCRIPTOR* descriptor);
		bool getStringDescriptor(UCHAR index, std::string& str);
		UCHAR getInterfaceNumber() const;

		void setTimeout(ULONG timeout);
		ULONG getTimeout() const;

		DWORD getLastError() const;
		std::string getLastErrorString() const;

		bool checkDeviceStatus();

	private:
		HANDLE deviceHandle_;
		WINUSB_INTERFACE_HANDLE winusbHandle_;
		USBDeviceInfo deviceInfo_;
		USBConfig config_;
		ULONG timeout_;
		DWORD lastError_;
		bool connetcted_;

		bool openDevice(const std::string& devicePath);
		bool initializeWinUSB();
		bool queryDeviceInformation();
		bool setupEndpoints();
		void cleanup();

		static bool getDeviceProperty(
			HDEVINFO deviceInfoSet,
			PSP_DEVINFO_DATA deviceInfoData,
			DWORD property,
			std::string& value
		);

		static std::string getDevicePath(
			HDEVINFO deviceInfoSet,
			PSP_DEVINFO_DATA deviceInfoData,
			const GUID& interfaceGuid
		);

		static std::string errorToString(DWORD error);
	};
}