#include "../include/ChatServer.h"
#include <iostream>
#include <signal.h>

// 处理信号函数
void handleSignal(int sig) {
	std::cout << "Received signal " << sig << ", shutting down..." << std::endl;
	exit(0);
}

// 主函数
int main(int argc, char* argv[]) {
	signal(SIGINT, handleSignal);
	signal(SIGTERM, handleSignal);
	
	int port = 8080;
	
	for (int i = 1; i < argc; ++i) {
		if (std::string(argv[i]) == "-p" && i + 1 < argc) {
			port = std::stoi(argv[i + 1]);
			break;
		}
	}
	
	try {
		ChatServer server(port, "ChatServer");
		server.setThreadNum(4);
		std::cout << "ChatServer started on port " << port << std::endl;
		server.start();
	} catch (const std::exception& e) {
		std::cerr << "Server failed to start: " << e.what() << std::endl;
		return 1;
	}
	
	return 0;
}
