#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

enum CMD {
	CMD_LOGIN,CMD_LOGINOUT,CMD_ERROR
};
struct DataHeader {
	short dataLength;//数据长度
	short cmd;//命令
};

//登陆结构体
struct LoginData {
	char userName[32];
	char Passward[32];
};

//登陆返回结构体
struct LoginResult {
	int result;
};

//登出结构体
struct LogoutData {
	char userName[32];
};

//登出返回结构体
struct LogOutResult {
	int result;
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
		DataHeader header = {};
		int nLen = recv(_csock, (char *)&header, sizeof(header), 0);
		if (nLen <= 0) {
			printf("客户端没有发送有效命令");
			break;
		}
		else {
			printf("接收到消息：命令 %d  数据长度  %d \n", header.cmd, header.dataLength);
			switch (header.cmd) {
			case CMD_LOGIN:
			{
				LoginData login_data = {};
				recv(_csock, (char*)&login_data, sizeof(LoginData), 0);
				LoginResult login_res = {};
				login_res.result = {1};
				send(_csock, (char*)&header, sizeof(DataHeader), 0);
				send(_csock, (char*)&login_res, sizeof(login_res), 0);
			}
				break;
			case CMD_LOGINOUT:
			{
				LogoutData logout = {};
				recv(_csock, (char*)&logout, sizeof(LogoutData), 0);
				LogOutResult logout_res = {};
				logout_res.result = {1};
				send(_csock, (char*)&header, sizeof(DataHeader), 0);
				send(_csock, (char*)&logout_res, sizeof(LogOutResult), 0);
			}
				break;
			defualt:
				{
					header.cmd = CMD_ERROR;
					send(_csock, (char*)&header, sizeof(DataHeader), 0);
				}
				break;
			}
		}
	}

	//关闭socket
	closesocket(_socket);
	WSACleanup();
	printf("任务结束，关闭服务器。\n");
	getchar();
	return 0;
}