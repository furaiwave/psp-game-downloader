#pragma once

#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>

class PSPDevice;

class FileTransfer {
public:
	FileTransfer(std::shared_ptr<PSPDevice> device);
	~FileTransfer();

	using ProgressCallback = std::function<void(size_t, size_t, double)>;
	using CompletionCallback = std::function<void(bool)>;

	bool transferFiles(
		const std::string& sourcePath,
		const std::string& destinationPath,
		ProgressCallback progressCallback = nullptr
	);

	struct FileTransferTask {
		std::string soutcePath;
		std::string destinationPath;
		size_t fileSize;
	};

	bool transferFiles(
		const std::string& sourcePath,
		const std::string& destinationPath,
		ProgressCallback progressCallback = nullptr,
		CompletionCallback completionCallback = nullptr
	);

	void cancelTransfer();
	void pauseTransfer();
	void resumeTransfer();
	bool isPaused() const;
	bool isCancelled() const;
	bool isTransferring() const;

	struct TransferStats {
		size_t bytesTransferred;
		size_t totalBytes;
		double progreess;
		double currentSpeed;
		double averageSpeed;
		long long elapsedTime;
		long long estimateTimeLeft;
		size_t chunksTransferred;
		size_t totalChunks;
	};

	TransferStats getStats() const;
	void resetStats();

	void setChunkSize(size_t size);
	size_t getChunkSize() const;

	void setVerifyTransfer(bool verify);
	bool getVerifyTransfer() const;

	void setRetryCount(int count);
	int getRetryCount() const;

	void setRetryDelay(int milliseconds);
	int getRetryDelay() const;

	bool verifyFile(const std::string& sourcePathj, const std::string& destPath);
	std::string calculateChecksum(const std::string& filePath);

	bool resumeTransfer(
		const std::string& sourcePath,
		const std::string& destinationPath,
		size_t startOffset,
		ProgressCallback progressCallback = nullptr
	);

	std::string getLastError() const;

private:
	std::shared_ptr<PSPDevice> device_;
	size_t chunkSize_;
	bool verifyTransfer_;
	int retryCount_;
	int retryDelay_;

	std::atomic<bool> transferring_;
	std::atomic<bool> paused_;
	std::atomic<bool> cancelled_;

	mutable std::mutex statsMutex_;
	TransferStats stats_;
	long long trasnferStartTime_;

	struct AssyncTransferTask {
		std::string sourcePath;
		std::string destinationPath;
		ProgressCallback progressCallback;
		CompletionCallback completionCallback;
	};

	std::queue<AssyncTransferTask> asyncQueue_;
	std::mutex asyncMutex_;
	std::thread asyncThread_;
	std::condition_variable queueCV_;
	std::atomic<bool> asyncThreadRunning_;

	mutable std::mutex errorMutex_;
	std::string lastError_;

	bool TransferFileInternal(
		const std::string& sourcePath,
		const std::string& destinationPath,
		ProgressCallback progressCallback,
		size_t startOffset = 0
	);

	bool openSourceFile(const std::string& path, size_t& fileSize);
	bool openDestFile(const std::string& path);
	bool closeSourceFile();
	bool closeDestFile();

	bool transferChunk(const void* buffer, size_t size);
	bool transferChunkWithretry(const void* buffer, size_t size);

	void updateStats(size_t bytesTransferred, size_t totalBytes);
	double calculateSpeed(size_t bytes, long long timeMs) const;
	long long estimateTimeRemaining(size_t bytesremaining, double speed) const;

	void waitIfPaused();
	void asyncTransferThread();
	void pressAsyncQueue();

	bool verifyChecksum(const std::string& sourcePath, const std::string& destPath);
	std::string calculateMD5(const std::string& filePath);
	std::string calculateCRC32(const std::string& filePath);
	void setLastError(const std::string& error);
	long long getCurrentTime() const;
	bool fileExists(const std::string& path) const;
	size_t getFileSize(const std::string& path) const;
};