#pragma once

#include <crow.h>
#include <memory>
#include <string>

class PSPController;
class UploadController;

namespace http {
	class Server {
	public:
		Server(int port = 8080);
		~Server();

		void run();
		void stop();
		void setupCORS();
		void setupRoutes();
		void setupStatic(const std::string& path);

	private:
		crow::SimpleApp app_;
		int port_;

		std::shared_ptr<PSPController> pspController_;
		std::shared_ptr<UploadController> uploadController_;

		void setupLooging();
		void setupErrorHadnling();
		void handleOptions(const crow::request& req, crow::response& res);
	};
}