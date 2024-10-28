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
	short dataLength;//���ݳ���
	short cmd;//����
};

//��½�ṹ��
struct LoginData {
	char userName[32];
	char Passward[32];
};

//��½���ؽṹ��
struct LoginResult {
	int result;
};

//�ǳ��ṹ��
struct LogoutData {
	char userName[32];
};

//�ǳ����ؽṹ��
struct LogOutResult {
	int result;
};

/**
* ����һ������TCP�ͻ���
* 1.����һ��socket
* 2.���ӷ����� connect
* 3.���շ�������Ϣ recv
* 4.�ر� socket closesocket
**/
int main() {
	WORD w = MAKEWORD(2, 2);
	WSAData dt;
	//��ʼ��winsock����
	WSAStartup(w, &dt);
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
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_socket, (sockaddr*)&_sin, sizeof(_sin));
	if (ret == SOCKET_ERROR) {
		printf("Socket connect failed!\n");
	}
	else {
		printf("Socket connect successfally!\n");
	}

	while (true) {
		//������������
		char cmdBuf[128] = {};
		scanf_s("%s", cmdBuf, sizeof(cmdBuf));
		if (0 == strcmp(cmdBuf, "exit")) {
			printf("�˳�\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			//������������
			LoginData login = {"amston", "amston"};
			DataHeader header = {sizeof(login),CMD_LOGIN};
			send(_socket,(char*)&header, sizeof(header), 0 );//����ͷ
			send(_socket,(char *)&login, sizeof(login),0); //��������
			//���շ��������ص�����
			DataHeader retheader = {};
			LoginResult loginres = {};
			recv(_socket,(char *)&retheader, sizeof(retheader), 0);
			recv(_socket, (char*)&loginres, sizeof(loginres), 0);
			printf("login result: %d \n", loginres.result);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			//������������
			LogoutData logout = {"amston"};
			DataHeader header = { sizeof(logout),CMD_LOGINOUT };
			send(_socket, (char*)&header, sizeof(header), 0);//����ͷ
			send(_socket, (char*)&logout, sizeof(logout), 0); //��������
			//���շ��������ص�����
			DataHeader retheader = {};
			LogOutResult logoutres = {};
			recv(_socket, (char*)&retheader, sizeof(retheader), 0);
			recv(_socket, (char*)&logoutres, sizeof(logoutres), 0);
			printf("logout result:  %d  \n", logoutres.result);
		}
		else {
			//����ָ��
			printf("��֧�ֵ�����");
		}
		//char recvBuf[128] = {};
		////���շ�������Ϣ recv
		//int nlens = recv(_socket, recvBuf, 128, 0);
		//if (0!=strcmp(cmdBuf, "getInfo") && nlens > 0) {
		//	printf("�յ����ݣ�%s\n", recvBuf);
		//}
		//else {
		//	DataPackage* data = (DataPackage*)recvBuf;
		//	printf("���յ������ݣ� ���� %d  ����  %s \n", data->age, data->name);
		//}
	}
	//�ر� socket closesocket
	closesocket(_socket);
	WSACleanup();
	printf("���˳����������\n");
	getchar();
	return 0;
}