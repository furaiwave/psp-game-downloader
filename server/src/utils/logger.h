#pragma once

#define LOG_TRACE(msg)    Logger::log(LogLevel::Trace, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_DEBUG(msg)    Logger::log(LogLevel::Debug, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_INFO(msg)     Logger::log(LogLevel::Info, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_WARNING(msg)  Logger::log(LogLevel::Warning, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_ERROR(msg)    Logger::log(LogLevel::Error, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_CRITICAL(msg) Logger::log(LogLevel::Critical, msg, __FILE__, __LINE__, __FUNCTION__)

#define LOG_TRACE_FMT(fmt, ...)    { char buf[1024]; snprintf(buf, sizeof(buf), fmt, __VA_ARGS__); LOG_TRACE(buf); }
#define LOG_DEBUG_FMT(fmt, ...)    { char buf[1024]; snprintf(buf, sizeof(buf), fmt, __VA_ARGS__); LOG_DEBUG(buf); }
#define LOG_INFO_FMT(fmt, ...)     { char buf[1024]; snprintf(buf, sizeof(buf), fmt, __VA_ARGS__); LOG_INFO(buf); }
#define LOG_WARNING_FMT(fmt, ...)  { char buf[1024]; snprintf(buf, sizeof(buf), fmt, __VA_ARGS__); LOG_WARNING(buf); }
#define LOG_ERROR_FMT(fmt, ...)    { char buf[1024]; snprintf(buf, sizeof(buf), fmt, __VA_ARGS__); LOG_ERROR(buf); }
#define LOG_CRITICAL_FMT(fmt, ...) { char buf[1024]; snprintf(buf, sizeof(buf), fmt, __VA_ARGS__); LOG_CRITICAL(buf); }


#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <sstream>
#include <vector>
#include <functional>
#include <Windows.h>

enum class LogLevel {
	Trace,
	Debug,
	Info,
	Warning,
	Error,
	Critical
};

enum class ConsoleColor {
	DefaultСolor,
	Black,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan,
	White,
	BrightRed,
	BrightGreen,
	BrightYellow,
	BrightBlue,
	BrightMagenta,
	BrightCyan,
	BrightWhite,
};

struct LogEntry {
	LogLevel level;
	std::string  message;
	std::string timestamp;
	std::string threadId;
	std::string file;
	int line;
	std::string function;
};

class Logger {
public:
	static Logger& getInstance();
	static void initialize(const std::string& logFile = "psp_server.log");
	static void shutdown();

	static void trace(const std::string& message);
	static void debug(const std::string& message);
	static void info(const std::string& message);
	static void warning(const std::string& message);
	static void error(const std::string& message);
	static void critical(const std::string& message);

	static void log(
		LogLevel level,
		const std::string& message,
		const char* file = nullptr,
		int line = 0,
		const char* function = nullptr
	);

	void setLogLevel(LogLevel level);
	LogLevel getLogLevel() const;

	void setConsoleOutput(bool enabled);
	bool getConsoleOutput() const;

	void setFileOutput(bool enabled);
	bool getColoredOutput() const;

	void setColoredOutput(bool enabled);
	bool getColoredOutput() const;

	void setLogFile(const std::string& filename);
	std::string getLogFile() const;

	void setMaxFileSize(size_t sizeInBytes);
	size_t geMaxFileSize() const;

	void setMaxBackupFiles(int count);
	int getMaxBackupFiles() const;

	void setTimestampFormat(const std::string& format);
	std::string gettimestampFormat() const;

	void setAsync(bool async);
	bool getAsync() const;

	using LogFilter = std::function<bool(const LogEntry&)>;
	void addFilter(LogFilter filter);
	void cleearHandlers();

	using LogHandler = std::function<void(const LogEntry&)>;
	void addHandler(LogHandler handler);
	void clearHandlers();

	void flush();
	void clear();
	void rotateLogFile();

	size_t getLogCount() const;
	size_t getLogCount(LogLevel level) const;

	std::vector<LogEntry> getRecentLogs(size_t count) const;
	bool exportLogs(const std::string& filename, LogLevel minLevel = LogLevel::Trace) const;

private:
	Logger();
	~Logger();

	Logger(const Logger&) = delete;
	Logger& operator = (const Logger&) = delete;

	void writeLog(const LogEntry& entry);
	void writeToConsole(const LogEntry& entry);
	void writeToFile(const LogEntry& entry);
	void writeAsync(const LogEntry& entry);

	bool shouldLog(LogLevel level) const;
	bool passFilters(const LogEntry& entry) const;

	std::string formatMessage(const LogEntry& entry) const;
	std::string getCurrentTimestamp() const;
	std::string getThreadID() const;
	std::string levelToString(LogLevel level) const;
	ConsoleColor levelToColor(LogLevel level) const;

	void setConsoleColor(ConsoleColor color);
	void resetConsoleColor();

	void checkandRotatefile();
	void backupLogFile();

	void asyncTread();
	void processASyncQueue();

	std::ofstream logFile_;
	std::string logFilename_;
	LogLevel currentLevel_;
	bool consoleOutput_;
	bool fileOutput_;
	bool coloredOutput_;
	bool asyncMode_;

	size_t maxFileSize_;
	int maxBackupFiles_;
	std::string timestampFormat_;

	mutable std::mutex mutex_;
	std::vector<LogFilter> filter_;
	std::vector<LogHandler> handlers_;

	std::vector<size_t> logCounts_;
	static constexpr size_t MAX_RECENT_LOGS = 1000;
	std::vector<LogEntry> recentLogs_;
	
	struct AsyncQueue;
	std::unique_ptr<AsyncQueue> asyncQueue;
};