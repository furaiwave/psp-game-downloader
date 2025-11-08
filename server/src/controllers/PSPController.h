#pragma once

#include <crow.h>
#include <memory>
#include <string>
#include <vector>

class PSPDevice;

namespace services {
	class TransferService;
}

class PSPController {
public:
	PSPController();
	~PSPController();

	bool initialize();

	crow::json::wvalue getStatus();
	crow::json::wvalue getDeviceInfo();
	crow::json::wvalue getDrives();
	crow::json::wvalue getStorageInfo();
	crow::json::wvalue getGameList();
	crow::json::wvalue getGameInfo(const std::string& gameId);
	crow::json::wvalue deleteGame(const std::string& gameId);
	crow::json::wvalue getFolders(const std::string& path = "/");
	crow::json::wvalue createFolder(const std::string& path);
	crow::json::wvalue renameGame(const std::string& gameId, const std::string& newGame);

private:
	std::shared_ptr<PSPDevice> device_;
	std::shared_ptr<services::TransferService> transferService_;

	struct GameInfo {
		std::string id;
		std::string title;
		std::string filename;
		std::string path;
		std::string discId;
		size_t size;
		std::string format;
		long long addedDate;
	};

	std::vector<GameInfo> scanGamesFolder();
	GameInfo parseGamefile(const std::string& filePath);
	std::string extractGameTitle(const std::string& isoPath);
	std::string extractDiscId(const std::string& isoPath);
	std::string formatSize(size_t bytes);
	std::string formatDate(long long timestamp);

	bool checkDeviceConnection();
	crow::json::wvalue createErrorResponce(const std::string& message);
	crow::json::wvalue createSuccessResponce(const std::string& message);
};