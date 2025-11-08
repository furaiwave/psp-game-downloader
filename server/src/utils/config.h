#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <any>

namespace nlohmann {
	class json;
}

class Config {
public:
	static Config& getInstance();

	static bool load(const std::string& filename = "config.json");
	static bool save(const std::string& filename = "config.json");
	static bool reload();

	template<typename T>
	static T get(const std::string& key, const T& defaultValue = T());

	static std::string getString(const std::string& key, const std::string& defaultValue = "");
	static int getInt(const std::string& key, int defaultValue = 0);
	static double getDouble(const std::string& ley, double defaultValue = 0.0);
	static bool getBool(const std::string& key, bool defaultValue = false);
	static std::vector<std::string> getArray(const std::string& key);

	template<typename T>
	static void set(const std::string& key, const T& value);
	static void setInt(const std::string& key, int value);
	static void setString(const std::string& key, const std::string& value);
	static void setDouble(const std::string& key, double vlaue);
	static void setBool(const std::string& key, bool value);
	static void setArray(const std::string& key, const std::vector<std::string>& value);

	static bool has(const std::string& key);

	static void remove(const std::string& key);

	static void clear();

	static std::vector<std::string> getKeys();

	static bool hasSection(const std::string& section);
	static std::vector<std::string> getSectionKeys(const std::string& value);

	template <typename T>
	static T getSectionValue(
		const std::string& section,
		const std::string& key,
		const T& defaultValue = T()
	);

	template <typename T>
	static void setSectionValue(const std::string& section, const std::string& key, const T& value);
	static std::string toJSONString(bool pretty = true);
	static bool fromJSONString(const std::string& jsonString);

	static std::map<std::string, std::any> getAllValues();
	static void setAllValues(const std::map<std::string, std::any>& values);

	static bool validate();
	static std::vector<std::string> getValidationErrors();

	static void loadDefaults();
	static void resetToDefaults();

private:
	Config();
	~Config();

	Config(const Config&) = delete;
	Config& operator = (const Config&) = delete;

	bool loadFromFile(const std::string& filename);
	bool saveToFile(const std::string& filename);

	std::any getValue(const std::string& key, const std::any& defaultValue);
	void setValue(const std::string& key, const std::any& value);

	std::any getValue(const std::string& key, const std::any& defaultValue);
	void setValue(const std::string& key, const std::any& value);

	std::vector<std::string> splitPath(const std::string& path);
	bool hasPath(const std::vector<std::string>& path);
	std::any getPath(const std::vector<std::string>& path, const std::any& defaultValue);
	void setPath(const std::vector<std::string>& path, const std::any& value);

	template<typename T>
	T convertValue(const std::any& value, const T& defaultValue);

	void createDefaultConfig();

	bool validateInteranl();
	void addValidationError(const std::string& error);

	std::unique_ptr<nlohmann::json> configData_;
	std::string currentFilename_;
	mutable std::mutex mutex_;
	std::vector<std::string> validationErrors_;

	std::map<std::string, std::any> defauts_;
};

struct ServerConfig {
	static int getPort() { return Config::getInt("server.port", 8080); }
	static void setPort(int port) { Config::setInt("server.port", port); }
	
	static std::string getHost() { return Config::getString("server.host", "0.0.0.0"); }
	static void setHost(const std::string& host) { Config::setString("server.host", host); }

	static int getThreads() { return Config::getInt("server.threads", 4); }
	static void setThreads(int threads) { Config::setInt("server.threads", threads); }

	static bool getCorsEnabled() { return Config::getBool("server.cors.enabled", true); }
	static void setCorsEnabled(bool enabled) { Config::setBool("server.cors.enabled", enabled); }
};

struct PSPConfig {
	static int getVendorID() { return Config::getInt("psp.vendorID", 0x054C); }
	static void setVendorID(int id) { Config::setInt("psp.vendorID", id); }

	static int getProductID() { return Config::getInt("psp.productID", 0x01C9); }
	static void setProductID(int id) { Config::setInt("psp.productID", id); }

	static std::string getISOPath() { return Config::getString("psp.isoPath", "/ISO"); }
	static void setISOPath(const std::string& path) { Config::setString("psp.isoPath", path); }

	static int getConnectionTimeout() { return Config::getInt("psp.connectionTimeout", 5000); }
	static void setConnectionTimeout(int ms) { Config::setInt("psp.connectionTimeout", ms); }

	static bool getAutoReconnect() { return Config::getBool("psp.autoReconnect", true); }
	static void setAutoReconnect(bool enabled) { Config::setBool("psp.autoReconnect", enabled); }
};

struct TransferConfig {
	static int getChunkSize() { return Config::getInt("transfer.chunkSize", 65536); }
	static void setChunkSize(int size) { Config::setInt("trasnfer.chunkSize", size); }

	static int getRetryCount() { return Config::getInt("transfer.retryCount", 3); }
	static void setRetryCount(int count) { Config::setInt("transger", count); }

	static int getRetryDelay() { return Config::getInt("transfer.retryDelay", 1000); }
	static void setRetryDelay(int ms) { Config::setInt("transfer.retryDelay", ms); }

	static bool getVerifyTransfer() { return Config::getBool("transfer.verifyTransfer", false); }
	static void setVerifyTranser(bool verify) { Config::setBool("transfer.verifyTransfer", verify); }

	static std::string getTempDir() { return Config::getString("transfer.tempDir", "/temp_uploads"); }
	static void setTempDir(const std::string& dir) { Config::setString("transfer.tempDir", dir); }
};

struct LogConfig {
	static std::string getLevel() { return Config::getString("logging.level", "info"); }
	static void setLevel(const std::string& level) { Config::setString("logging.level", level); }

	static std::string getfile() { return Config::getString("logging.file", "psp_server.log"); }
	static void setFile(const std::string& file) { Config::setString("logging.file", file); }

	static bool getConsoleOutput() { return Config::getBool("logging.console", true); }
	static void setConsoleOutput(bool enabled) { Config::setBool("logging.console", enabled); }

	static bool getFileOutput() { return Config::getBool("logging.fileOutput", true); }
	static void setFileOutput(bool enabled) { Config::setBool("looging.fileOutput", enabled); }

	static bool getColoredOutput() { return Config::getBool("logging.colored", true); }
	static void setColoredOutput(bool enabled) { Config::setBool("logging.colored", enabled); }

	static int getMaxFileSize() { return Config::getInt("logging.maxFileSize", 10485760); }
	static void setMaxFileSize(int size) { Config::setInt("logging.maxFileSize", size); }

	static int getMaxBackupFiles() { return Config::getInt("logging.maxBackupFiles", 5); }
	static void setMaxBackupFiles(int count) { Config::setInt("logging.maxBackupFiles", count); }
};