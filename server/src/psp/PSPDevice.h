#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace psp {
	class WinUSBConnection;
}

struct DriveInfo {
	std::string name;
	std::string path;
	std::string type;
	size_t totalSpace;
	size_t freeSpace;
	size_t usedSpace;
};

struct FileEntry {
	std::string name;
	std::string path;
	bool isDirectory;
	size_t size;
	long long modifiedTime;
};

enum class PSPModel {
	Uknown,
	PSP1000,
	PSP2000,
	PSP3000,
	PSPGo,
	PSPStreet
};

class PSPDevice {
public:
	PSPDevice();
	~PSPDevice();

	bool connect();
	bool connect(const std::string& devicePath);
	void disconnect();
	bool isConnected() const;
	bool reconnect();

	std::string getDeviceName() const;
	std::string getModelName() const;
	PSPModel getModel() const;
	std::string getFirmwareVersion() const;
	std::string getSerialNumber() const;
	uint16_t getVendoId() const;
	uint16_t getProductId() const;
	std::string getUsbVersion() const;

	int getBatteryLevel() const;
	bool isBatterCharging() const;
	int gertBatteryTimeRemaining() const;
	std::string getBatteryStatus() const;

	std::vector<DriveInfo> getDrivesList();
	size_t getTotalSpace(const std::string& drive = "ms0:/");
	size_t getFreeSpace(const std::string& drive = "ms0:/");
	size_t getUsedSpace(const std::string& drive = "ms0:/");
	double getUsagePercent(const std::string& drive = "ms0:/");

	std::vector<std::string> listFiles(const std::string& path);
	std::vector<FileEntry> listFolders(const std::string& path);
	std::vector<FileEntry> listDirectory(const std::string& path, bool recursive = false);

	bool fileExists(const std::string& path);
	bool directoryExists(const std::string& path);

	size_t getFileSize(const std::string& path);
	long long getFileModificationTime(const std::string& path);

	bool createDirectory(const std::string& path);
	bool createDirectories(const std::string& path);

	bool deleteFile(const std::string& path);
	bool deleteDirectory(const std::string& path, bool recursive = false);

	bool renameFile(const std::string& oldPath, const std::string& newPath);
	bool moveFile(const std::string& sourcePath, const std::string& destPath);
	bool copyFile(const std::string& sourcePath, const std::string& destPath);

	bool readFile(const std::string& path, std::vector<uint8_t>& data);
	bool readFile(const std::string& path, std::string& data);
	bool writeFile(const std::string& path, const std::vector<uint8_t>& data);
	bool writeFile(const std::string& path, const std::string& data);

	bool openFileForReading(const std::string& path);
	bool openFileForWriting(const std::string& path);
	bool readChunk(void* buffer, size_t size, size_t* bytesRead);
	bool writeChunk(const void* buffer, size_t size, size_t* bytesWritten);
	bool seekFile(long long offset, int origin);
	long long tellFile();
	void closeFile();

	bool isInUsbMode() const;
	bool enterUSbMode();
	bool exitUsbMode();

	bool takeScreenshot(const std::string& outputPath);
	bool getSystemInfo(std::string& info);

	bool ping();
	std::string getDeviceStatus();

	using StatusCallback = std::is_function<void(const std::string& status)>;
	void setStatusCallback(StatusCallback callback);

private:
	std::shared_ptr <psp::WinUSBConnection> connection;
	bool connected_;

	std::string devicenName_;
	PSPModel model_;
	std::string firmwareVersion_;
	std::string serialNumber_;

	struct OpenFile {
		std::string path;
		bool isOpen();
		bool isWriting();
		long long position;
		size_t fileSize;
	};

	OpenFile currentFile_;
	StatusCallback statusCallback_;

	static constexpr uint16_t PSP_VENDOR_ID = 0x054C; // Sony Corporation
	static constexpr uint16_t PSP_PRODUCT_ID_USB_MODE = 0x0268; // PSP in USB mode


	enum class PSPCommand : uint8_t {
		GetDeviceInfo			= 0x01,
		GetDriveInfo			= 0x02,
		ListDirectory			= 0x03,
		ReadFile				= 0x04,
		WriteFile				= 0x05,
		DeleteFile				= 0x06, 
		CreateDirectory			= 0x07,
		DeleteDirectory			= 0x08,
		RenameFile				= 0x09,
		GetFileInfo				= 0x0A,
		GetBatteryStatus		= 0x0B,
		Ping					= 0x0C,
		EnterUSBMode			= 0x0D,
		ExitUSBMode				= 0x0E,
	};

	bool sendCommand(PSPCommand cmd, const void* data, size_t dataSize);
	bool receiveResponse(void* buffer, size_t bufferSize, size_t* bytesReceived);
	bool executeCommand(
		PSPCommand cmd,
		const void* sendData,
		size_t sendSize,
		void* recvData,
		size_t recvSize,
		size_t* bytesReceived
	);

	bool queryDeviceInfo();
	bool queryBatteryInfo();

	PSPModel detectModel(uint16_t productId);
	std::string modelToString(PSPModel model) const;
	void notifyStatus(const std::string& status);
};