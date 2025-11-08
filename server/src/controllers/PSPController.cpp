#include "PSPController.h"
#include "../psp/PSPDevice.h"
#include "../services/TransferService.h" // <-- Ensure this include is present and not just a forward declaration
#include "../utils/logger.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>


using services::TransferService;

PSPController::PSPController() {
	Logger::info("PSPController initialized");
}

PSPController::~PSPController() {
	Logger::info("PSPController shutdown");
}

bool PSPController::initialize() {
	try {

		device_ = std::make_shared<PSPDevice>();

		if (!device_->connect()) {
			Logger::warning("PSP device not fount while initialize");
		}

		transferService_ = std::make_shared<services::TransferService>();
		transferService_->initialize(device_);

		Logger::info("PSPController success initalized with PSP");
		return true;
	}
	catch (const std::exception& e) {
		Logger::error("Error while initialize PSPController" + std::string(e.what()));
		return false;
	}
}

crow::json::wvalue PSPController::getStatus() {
	crow::json::wvalue response;

	try {
		bool connected = checkDeviceConnection();

		response["cennected"] = connected;
		response["timestamp"] = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count();

		if (connected) {
			response["status"] = "online";
			response["message"] = "PSPDevice is connected and ready";
		}
		else {
			response["status"] = "offline";
			response["message"] = "PSP device is not connected";
		}
	}
	catch (const std::exception& e) {
		Logger::error("Error while fetching status" + std::string(e.what()));
		return createErrorResponce(e.what());
	}

	return response;
}

crow::json::wvalue PSPController::getDeviceInfo() {
	crow::json::wvalue response;

	try {
		if (!checkDeviceConnection()) {
			return createErrorResponce("PSP Device is not connected");
		}

		response["success"] = true;
		response["device"]["name"] = device_->getDeviceName();
		response["device"]["model"] = device_->getModelName();
		response["device"]["firmware"] = device_->getFirmwareVersion();
		response["device"]["serialNumber"] = device_->getSerialNumber();
		response["device"]["batteryLevel"] = device_->getBatteryLevel();
		response["device"]["batteryCharging"] = device_->isBatterCharging();

		response["usb"]["vendorID"] = device_->getVendoId();
		response["usb"]["productID"] = device_->getProductId();
		response["usb"]["usbVersion"] = device_->getUsbVersion();
	}
	catch (const std::exception& e) {
		Logger::error("Error while fetching info about device" + std::string(e.what()));
		return createErrorResponce(e.what());
	}

	return response;
}

crow::json::wvalue PSPController::getDrives() {
	crow::json::wvalue response;

	try {
		if (!checkDeviceConnection()) {
			return createErrorResponce("PSP device is not connected");
		}

		auto drives = device_->getDrivesList();

		response["success"] = true;
		response["count"] = drives.size();

		crow::json::wvalue::list driverList;
		for (const auto& drive : drives) {
			crow::json::wvalue driveInfo;
			driveInfo["name"] = drive.name;
			driveInfo["path"] = drive.path;
			driveInfo["type"] = drive.path;
			driveInfo["totalSpace"] = drive.totalSpace;
			driveInfo["freeSpace"] = drive.freeSpace;
			driveInfo["usedSpace"] = drive.usedSpace;
			driveInfo["totalSpaceFormatted"] = formatSize(drive.totalSpace);
			driveInfo["freeSpaceFormatted"] = formatSize(drive.freeSpace);
			driveInfo["usedSpaceFormatted"] = formatSize(drive.usedSpace);
			driveInfo["usagePersent"] = (static_cast<double>(drive.usedSpace) / drive.totalSpace) * 100.0;

			driverList.push_back(std::move(driveInfo));
		}

		response["drives"] = std::move(driverList);
	}
	catch (const std::exception& e) {
		Logger::error("Error while fetching list disk" + std::string(e.what()));
		return createErrorResponce(e.what());
	}

	return response;
}

crow::json::wvalue PSPController::getStorageInfo() {
	crow::json::wvalue response;

	try {
		if (!checkDeviceConnection()) {
			return createErrorResponce("PSP device is not connected");
		}

		auto storage = transferService_->getStorageInfo();

		response["success"] = true;
		response["storage"]["totalSpace"] = storage.totalSpace;
		response["storage"]["freeSpace"] = storage.freeSpace;
		response["storage"]["usedSpace"] = storage.usedSpace;
		response["storage"]["usagePercent"] = storage.usagePercent;

		response["storage"]["totalSpaceFormatted"] = formatSize(storage.totalSpace);
		response["storage"]["freeSpaceFormatted"] = formatSize(storage.freeSpace);
		response["storage"]["usedSpaceFormatted"] = formatSize(storage.usedSpace);

		response["storage"]["conFit4GbGames"] = storage.freeSpace >= 4ULL * 1024 * 1024 * 1024;
		response["storage"]["estimatedGamesCapacity"] = storage.freeSpace / (1ULL * 1024 * 1024 * 1024);

	}
	catch (const std::exception& e) {
		Logger::error("Error while fetching info about storage" + std::string(e.what()));
		return createErrorResponce(e.what());
	}

	return response;
}

crow::json::wvalue PSPController::getGameList() {
	crow::json::wvalue response;

	try {
		if (!checkDeviceConnection()) {
			return createErrorResponce("PSP Device is not connetcted");
		}

		auto games = scanGamesFolder();

		response["success"] = true;
		response["count"] = games.size();

		crow::json::wvalue::list gamesList;
		for (const auto& game : games) {
			crow::json::wvalue gameInfo;
			gameInfo["id"] = game.id;
			gameInfo["title"] = game.title;
			gameInfo["filename"] = game.filename;
			gameInfo["path"] = game.path;
			gameInfo["discID"] = game.discId;
			gameInfo["size"] = game.size;
			gameInfo["sizeFormatted"] = formatSize(game.size);
			gameInfo["format"] = game.format;
			gameInfo["addedDate"] = game.addedDate;
			gameInfo["addedDateFormatted"] = formatDate(game.addedDate);

			gamesList.push_back(std::move(gameInfo));
		}

		response["games"] = std::move(gamesList);
	}
	catch (const std::exception& e) {
		Logger::error("Error while fetching game list" + std::string(e.what()));
		return createErrorResponce(e.what());
	}

	return response;
}

crow::json::wvalue PSPController::getGameInfo(const std::string& gameID) {
	crow::json::wvalue response;

	try {
		if (!checkDeviceConnection()) {
			return createErrorResponce("PSP devicew is not connected");
		}

		auto games = scanGamesFolder();

		auto it = std::find_if(games.begin(), games.end(),
			[&gameID](const GameInfo& game) {
				return game.id == gameID;
			});
		
		if (it == games.end()) {
			return createErrorResponce("Game not found: " + gameID);
		}

		response["success"] = true;
		response["game"]["id"] = it->id;
		response["game"]["title"] = it->title;
		response["game"]["path"] = it->path;
		response["game"]["discID"] = it->discId;
		response["game"]["size"] = it->size;
		response["game"]["sizeFormatted"] = formatSize(it->size);
		response["game"]["format"] = it->format;
		response["game"]["addedDate"] = it->addedDate;
		response["game"]["addedDateFormatted"] = formatDate(it->addedDate);
		
	}
	catch (const std::exception& e) {
		Logger::error("Error while get Info about game" + std::string(e.what()));
		return createErrorResponce(e.what());
	}

	return response;
}
