#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <thread>


#pragma comment(lib, "ws2_32.lib")
enum CMD {
	CMD_LOGIN, CMD_LOGOUT, CMD_ERROR, CMD_LOGIN_RESULT, CMD_LOGOUT_RESULT, CMD_NEW_USER_JOIN
};
struct DataHeader {
	short dataLength;//数据长度
	short cmd;//命令
};

//登陆结构体
struct LoginData : public DataHeader { //通过继承将头放入 组成消息体
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

//新用户加入
struct NewUserJoin : public DataHeader {
	NewUserJoin() {
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};
int process(SOCKET _csock) {
	DataHeader header = {};
	int nLen = recv(_csock, (char*)&header, sizeof(header), 0);
	if (nLen <= 0) {
		printf("与服务器的链接断开\n");
		return -1;
	}
	else {
		switch (header.cmd) {
		case CMD_LOGIN_RESULT:
		{
			LoginResult login_res = {};
			recv(_csock, (char*)&login_res + sizeof(DataHeader), sizeof(LoginResult) - sizeof(DataHeader), 0);
			printf("收到服务端消息：LoginResult 数据长度：%d 最终结果：%d\n", login_res.dataLength, login_res.result);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			LogOutResult logout_res = {};
			recv(_csock, (char*)&logout_res + sizeof(DataHeader), sizeof(LogoutData) - sizeof(DataHeader), 0);
			printf("收到服务端消息：CMD_LOGOUT_RESULT 结果：%d\n", logout_res.result);
		}
		break;
		case CMD_NEW_USER_JOIN: {
			NewUserJoin newJoin = {};
			recv(_csock, (char*)&newJoin + sizeof(DataHeader), sizeof(NewUserJoin) - sizeof(DataHeader), 0);
			printf("收到服务端消息：CMD_NEW_USER_JOIN 结果：新客户端加入 socket:%d\n", newJoin.sock);
		}
		break;
		defualt:
			break;
		}
		return 0;
	}
}

void cmdThread(void * arg) {
	while (true) {
		SOCKET _socket = *(SOCKET*)arg;
		//输入请求命令
		char cmdBuf[128] = {};
		scanf_s("%s", cmdBuf, sizeof(cmdBuf));
		if (0 == strcmp(cmdBuf, "exit")) {
			printf("退出\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			//发送请求命令
			LoginData login;
			strcpy_s(login.userName, "amston");
			strcpy_s(login.Passward, "amston");
			send(_socket, (char*)&login, sizeof(login), 0); //请求数据
			//接收服务器返回的数据
			LoginResult loginres = {};
			recv(_socket, (char*)&loginres, sizeof(loginres), 0);
			printf("login result: %d \n", loginres.result);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			//发送请求命令
			LogoutData logout;
			strcpy_s(logout.userName, "amston");
			send(_socket, (char*)&logout, sizeof(logout), 0); //请求数据
			//接收服务器返回的数据
			LogOutResult logoutres = {};
			recv(_socket, (char*)&logoutres, sizeof(logoutres), 0);
			printf("logout result:  %d  \n", logoutres.result);
		}
		else {
			//发送指令
			printf("不支持的命令\n");
		}
	}
}

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
	//多线程输入指令
	std::thread subthread(cmdThread, &_socket);
	subthread.detach();
	while (true) {
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(_socket, &fdset);
		int res = select(_socket, &fdset, NULL, NULL, NULL);//第一个参数指示最大文件描述符，表示要扫描的范围
		if (res < 0) {
			printf("select结束\n");
			break;
		}
		if (FD_ISSET(_socket, &fdset)) {
			FD_CLR(_socket, &fdset);
			if (-1 == process(_socket)) {
				printf("任务结束\n");
				break;
			}
		}
	}
	//关闭 socket closesocket
	closesocket(_socket);
	WSACleanup();
	printf("已退出，任务结束\n");
	getchar();
	return 0;
}