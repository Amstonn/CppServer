
#ifdef _WIN32 | _WIN64
	#define _CRT_SECURE_NO_WARNINGS 
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#include <windows.h>
	#include <WinSock2.h>
#else
	#include<unistd.h>
	#include<arpa/inet.h>
	#include<string.h>
	#define SOCKET int
	#define INVALID_SOCKET (SOCKET)(~0)
	#define SOCKET_ERROR (-1)
#endif

#include <iostream>
#include <thread>


#pragma comment(lib, "ws2_32.lib")
enum CMD {
	CMD_LOGIN, CMD_LOGOUT, CMD_ERROR, CMD_LOGIN_RESULT, CMD_LOGOUT_RESULT, CMD_NEW_USER_JOIN
};
struct DataHeader {
	short dataLength;//���ݳ���
	short cmd;//����
};

//��½�ṹ��
struct LoginData : public DataHeader { //ͨ���̳н�ͷ���� �����Ϣ��
	LoginData() {
		dataLength = sizeof(LoginData);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char Passward[32];
};

//��½���ؽṹ��
struct LoginResult : public DataHeader {
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
	}
	int result;
};

//�ǳ��ṹ��
struct LogoutData : public DataHeader {
	LogoutData() {
		dataLength = sizeof(LogoutData);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

//�ǳ����ؽṹ��
struct LogOutResult : public DataHeader {
	LogOutResult() {
		dataLength = sizeof(LogOutResult);
		cmd = CMD_LOGOUT_RESULT;
	}
	int result;
};

//���û�����
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
		printf("������������ӶϿ�\n");
		return -1;
	}
	else {
		switch (header.cmd) {
		case CMD_LOGIN_RESULT:
		{
			LoginResult login_res = {};
			recv(_csock, (char*)&login_res + sizeof(DataHeader), sizeof(LoginResult) - sizeof(DataHeader), 0);
			printf("�յ��������Ϣ��LoginResult ���ݳ��ȣ�%d ���ս����%d\n", login_res.dataLength, login_res.result);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			LogOutResult logout_res = {};
			recv(_csock, (char*)&logout_res + sizeof(DataHeader), sizeof(LogoutData) - sizeof(DataHeader), 0);
			printf("�յ��������Ϣ��CMD_LOGOUT_RESULT �����%d\n", logout_res.result);
		}
		break;
		case CMD_NEW_USER_JOIN: {
			NewUserJoin newJoin = {};
			recv(_csock, (char*)&newJoin + sizeof(DataHeader), sizeof(NewUserJoin) - sizeof(DataHeader), 0);
			printf("�յ��������Ϣ��CMD_NEW_USER_JOIN ������¿ͻ��˼��� socket:%d\n", newJoin.sock);
		}
			break;
		defualt:
			break;
		}
		return 0;
	}
}

void cmdThread(void* arg) {
	while (true) {
		SOCKET _socket = *(SOCKET*)arg;
		//������������
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit")) {
			printf("�˳�\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			//������������
			LoginData login;
			strcpy(login.userName, "amston");
			strcpy(login.Passward, "amston");
			send(_socket, (char*)&login, sizeof(login), 0); //��������
			//���շ��������ص�����
			LoginResult loginres = {};
			recv(_socket, (char*)&loginres, sizeof(loginres), 0);
			printf("login result: %d \n", loginres.result);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			//������������
			LogoutData logout;
			strcpy(logout.userName, "amston");
			send(_socket, (char*)&logout, sizeof(logout), 0); //��������
			//���շ��������ص�����
			LogOutResult logoutres = {};
			recv(_socket, (char*)&logoutres, sizeof(logoutres), 0);
			printf("logout result:  %d  \n", logoutres.result);
		}
		else {
			//����ָ��
			printf("��֧�ֵ�����\n");
		}
	}
}

/**
* ����һ������TCP�ͻ���
* 1.����һ��socket
* 2.���ӷ����� connect
* 3.���շ�������Ϣ recv
* 4.�ر� socket closesocket
**/
int main() {
#ifdef _WIN32 | _WIN64
	WORD w = MAKEWORD(2, 2);
	WSAData dt;
	WSAStartup(w, &dt);
#endif
	//����socket
	SOCKET _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET) {
		printf("Socket build failed!\n");
	}
	else {
		printf("Socket build successfally!\n");
	}
	//���ӷ�����
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
#ifdef _WIN32 | _WIN64
	_sin.sin_addr.S_un.S_addr = inet_addr("192.168.0.105");
#else
	_sin.sin_addr.s_addr = inet_addr("192.168.0.105");
#endif
	
	int ret = connect(_socket, (sockaddr*)&_sin, sizeof(_sin));
	if (ret == SOCKET_ERROR) {
		printf("Socket connect failed!\n");
	}
	else {
		printf("Socket connect successfally!\n");
	}
	//���߳�����ָ��
	std::thread subthread(cmdThread, &_socket);
	subthread.detach();
	while (true) {
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(_socket, &fdset);
		int res = select(_socket, &fdset, NULL, NULL, NULL);//��һ������ָʾ����ļ�����������ʾҪɨ��ķ�Χ
		if (res < 0) {
			printf("select����\n");
			break;
		}
		if (FD_ISSET(_socket, &fdset)) {
			FD_CLR(_socket, &fdset);
			if (-1 == process(_socket)) {
				printf("�������\n");
				break;
			}
		}
	}
	//�ر� socket closesocket
#ifdef _WIN32 | _WIN64
	closesocket(_socket);
	WSACleanup();
#else
	close(_socket);
#endif

	printf("���˳����������\n");
	getchar();
	return 0;
}