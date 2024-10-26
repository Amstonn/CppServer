#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

//结构化返回数据
struct DataPackage {
	int age;
	char name[32];
};
int main() {
	WORD w = MAKEWORD(2, 2);
	WSAData dt;
	//初始化winsock环境
	WSAStartup(w, &dt);
	//建立socket
	SOCKET _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET) {
		printf("Socket建立失败!\n");
	}
	else {
		printf("Socket建立成功!\n");
	}
	//绑定用于接收客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (SOCKET_ERROR == bind(_socket, (sockaddr*)&_sin, sizeof(_sin))) {
		printf("端口绑定失败\n");
	}
	else {
		printf("端口绑定成功\n");
	}
	//listen 监听网络端口
	if (SOCKET_ERROR == listen(_socket, 5)) {
		printf("端口监听失败\n");
	}
	else {
		printf("端口监听成功\n");
	}
	//accept 等待客户端连接
	sockaddr_in _csockaddr = {};
	int clens = sizeof(_csockaddr);
	SOCKET _csock = accept(_socket, (sockaddr*)&_csockaddr, &clens);
	if (INVALID_SOCKET == _csock) {
		printf("客户端Socket无效\n");
	}
	else {
		printf("新客户端加入IP = %s\n", inet_ntoa(_csockaddr.sin_addr));
	}
	//接收客户端请求 处理并发送数据
	while (true) {
		char _recvBuff[128] = {};
		int nLen = recv(_csock, _recvBuff, 128, 0);
		if (nLen <= 0) {
			printf("客户端没有发送有效命令");
			break;
		}
		else {
			printf("接收到消息：%s\n", _recvBuff);
			char returnBuff[128] = {};
			if (0 == strcmp(_recvBuff, "getName")) {
				strcpy_s(returnBuff, "我是服务器\n");
			}
			else if (0 == strcmp(_recvBuff, "getAge")) {
				strcpy_s(returnBuff, "我0岁了\n");
			}
			else if (0 == strcmp(_recvBuff, "getInfo")) {
				DataPackage data = {41,"小王"};
				send(_csock, (const char*) &data, sizeof(data), 0);
			}
			else {
				strcpy_s(returnBuff, "我可以帮你做什么？\n");
			}
			send(_csock, returnBuff, sizeof(returnBuff), 0);
		}
	}

	//关闭socket
	closesocket(_socket);
	WSACleanup();
	printf("任务结束，关闭服务器。\n");
	getchar();
	return 0;
}