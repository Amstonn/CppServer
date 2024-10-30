#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include<vector>
using namespace std;
#pragma comment(lib, "ws2_32.lib")


enum CMD {
	CMD_LOGIN,CMD_LOGOUT,CMD_ERROR,CMD_LOGIN_RESULT,CMD_LOGOUT_RESULT
};
struct DataHeader {
	short dataLength;//数据长度
	short cmd;//命令
};

//登陆结构体
struct LoginData : public DataHeader{ //通过继承将头放入 组成消息体
	LoginData() {
		dataLength = sizeof(LoginData);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char Passward[32];
};

//登陆返回结构体
struct LoginResult : public DataHeader {
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
	}
	int result;
};

//登出结构体
struct LogoutData : public DataHeader {
	LogoutData() {
		dataLength = sizeof(LogoutData);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

//登出返回结构体
struct LogOutResult : public DataHeader {
	LogOutResult() {
		dataLength = sizeof(LogOutResult);
		cmd = CMD_LOGOUT_RESULT;
	}
	int result;
};

int process(SOCKET _csock) {
	DataHeader header = {};
	int nLen = recv(_csock, (char*)&header, sizeof(header), 0);
	if (nLen <= 0) {
		printf("客户端没有发送有效命令");
		return -1;
	}
	else {
		switch (header.cmd) {
		case CMD_LOGIN:
		{
			LoginData login_data = {};
			//先前接收了一个DataHeader 所以这里需要做偏移  以获取剩余的数据
			recv(_csock, (char*)&login_data + sizeof(DataHeader), sizeof(LoginData) - sizeof(DataHeader), 0);
			printf("收到命令：CMD_LGOIN 数据长度：%d 用户名：%s\n", login_data.dataLength, login_data.userName);
			LoginResult login_res;
			login_res.result = 1;
			send(_csock, (char*)&login_res, sizeof(login_res), 0);
		}
		break;
		case CMD_LOGOUT:
		{
			LogoutData logout = {};
			recv(_csock, (char*)&logout + sizeof(DataHeader), sizeof(LogoutData) - sizeof(DataHeader), 0);
			printf("收到命令：CMD_LOGOUT 用户名：%s\n", logout.userName);
			LogOutResult logout_res;
			logout_res.result = 1;
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
	return 0;
}

vector<SOCKET> fd;

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
	
	//接收客户端请求 处理并发送数据
	while (true) {
		fd_set readfds;
		FD_ZERO(&readfds); //初始化为空集合
		FD_SET(_socket, &readfds);
		for (auto filedis : fd) {
			FD_SET(filedis, &readfds); //套接字加入到set集合
		}
		int ret = select(_socket + 1, &readfds, NULL,NULL,NULL);
		if (ret < 0) {  //出错则跳出循环
			perror("select error!");
			break;
		}
		if (FD_ISSET(_socket, &readfds)) {
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
			fd.push_back(_csock); //将新连接的客户端socket放到文件描述符中
		}
		for (int i = 0; i < fd.size(); i++) {
			if (FD_ISSET(fd[i], &readfds)) {
				if (-1 == process(fd[i])) {
					auto iter = find(fd.begin(), fd.end(),fd[i]);
					if (iter != fd.end()) {
						fd.erase(iter);
					}
				}
			}
		}
		
	}

	//关闭socket
	for (auto item : fd) {
		closesocket(item);
	}
	closesocket(_socket);
	WSACleanup();
	printf("任务结束，关闭服务器。\n");
	getchar();
	return 0;
}