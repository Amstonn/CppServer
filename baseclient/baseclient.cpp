#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")
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
		else {
			//����ָ��
			send(_socket, cmdBuf, strlen(cmdBuf) + 1, 0);
		}
		char recvBuf[128] = {};
		//���շ�������Ϣ recv
		int nlens = recv(_socket, recvBuf, 128, 0);
		if (nlens > 0) {
			printf("�յ����ݣ�%s\n", recvBuf);
		}
	}
	//�ر� socket closesocket
	closesocket(_socket);
	WSACleanup();
	printf("���˳����������\n");
	getchar();
	return 0;
}