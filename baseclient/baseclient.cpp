#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")
/**
* 建立一个简易TCP客户端
* 1.建立一个socket
* 2.连接服务器 connect
* 3.接收服务器信息 recv
* 4.关闭 socket closesocket
**/
int main() {
	WORD w = MAKEWORD(2, 2);
	WSAData dt;
	//初始化winsock环境
	WSAStartup(w, &dt);
	//建立socket
	SOCKET _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET) {
		printf("Socket build failed!\n");
	}
	else {
		printf("Socket build successfally!\n");
	}
	//连接服务器
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_socket, (sockaddr*)&_sin, sizeof(_sin));
	if (ret == SOCKET_ERROR) {
		printf("Socket connect failed!\n");
	}
	else {
		printf("Socket connect successfally!\n");
	}

	while (true) {
		//输入请求命令
		char cmdBuf[128] = {};
		scanf_s("%s", cmdBuf, sizeof(cmdBuf));
		if (0 == strcmp(cmdBuf, "exit")) {
			printf("退出\n");
			break;
		}
		else {
			//发送指令
			send(_socket, cmdBuf, strlen(cmdBuf) + 1, 0);
		}
		char recvBuf[128] = {};
		//接收服务器信息 recv
		int nlens = recv(_socket, recvBuf, 128, 0);
		if (nlens > 0) {
			printf("收到数据：%s\n", recvBuf);
		}
	}
	//关闭 socket closesocket
	closesocket(_socket);
	WSACleanup();
	printf("已退出，任务结束\n");
	getchar();
	return 0;
}