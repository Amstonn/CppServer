#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")
enum CMD {
	CMD_LOGIN, CMD_LOGINOUT, CMD_ERROR
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
		else if (0 == strcmp(cmdBuf, "login")) {
			//发送请求命令
			LoginData login = {"amston", "amston"};
			DataHeader header = {sizeof(login),CMD_LOGIN};
			send(_socket,(char*)&header, sizeof(header), 0 );//请求头
			send(_socket,(char *)&login, sizeof(login),0); //请求数据
			//接收服务器返回的数据
			DataHeader retheader = {};
			LoginResult loginres = {};
			recv(_socket,(char *)&retheader, sizeof(retheader), 0);
			recv(_socket, (char*)&loginres, sizeof(loginres), 0);
			printf("login result: %d \n", loginres.result);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			//发送请求命令
			LogoutData logout = {"amston"};
			DataHeader header = { sizeof(logout),CMD_LOGINOUT };
			send(_socket, (char*)&header, sizeof(header), 0);//请求头
			send(_socket, (char*)&logout, sizeof(logout), 0); //请求数据
			//接收服务器返回的数据
			DataHeader retheader = {};
			LogOutResult logoutres = {};
			recv(_socket, (char*)&retheader, sizeof(retheader), 0);
			recv(_socket, (char*)&logoutres, sizeof(logoutres), 0);
			printf("logout result:  %d  \n", logoutres.result);
		}
		else {
			//发送指令
			printf("不支持的命令");
		}
		//char recvBuf[128] = {};
		////接收服务器信息 recv
		//int nlens = recv(_socket, recvBuf, 128, 0);
		//if (0!=strcmp(cmdBuf, "getInfo") && nlens > 0) {
		//	printf("收到数据：%s\n", recvBuf);
		//}
		//else {
		//	DataPackage* data = (DataPackage*)recvBuf;
		//	printf("接收到的数据： 年龄 %d  姓名  %s \n", data->age, data->name);
		//}
	}
	//关闭 socket closesocket
	closesocket(_socket);
	WSACleanup();
	printf("已退出，任务结束\n");
	getchar();
	return 0;
}