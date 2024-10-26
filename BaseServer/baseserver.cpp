#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

//�ṹ����������
struct DataPackage {
	int age;
	char name[32];
};
int main() {
	WORD w = MAKEWORD(2, 2);
	WSAData dt;
	//��ʼ��winsock����
	WSAStartup(w, &dt);
	//����socket
	SOCKET _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET) {
		printf("Socket����ʧ��!\n");
	}
	else {
		printf("Socket�����ɹ�!\n");
	}
	//�����ڽ��տͻ������ӵ�����˿�
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (SOCKET_ERROR == bind(_socket, (sockaddr*)&_sin, sizeof(_sin))) {
		printf("�˿ڰ�ʧ��\n");
	}
	else {
		printf("�˿ڰ󶨳ɹ�\n");
	}
	//listen ��������˿�
	if (SOCKET_ERROR == listen(_socket, 5)) {
		printf("�˿ڼ���ʧ��\n");
	}
	else {
		printf("�˿ڼ����ɹ�\n");
	}
	//accept �ȴ��ͻ�������
	sockaddr_in _csockaddr = {};
	int clens = sizeof(_csockaddr);
	SOCKET _csock = accept(_socket, (sockaddr*)&_csockaddr, &clens);
	if (INVALID_SOCKET == _csock) {
		printf("�ͻ���Socket��Ч\n");
	}
	else {
		printf("�¿ͻ��˼���IP = %s\n", inet_ntoa(_csockaddr.sin_addr));
	}
	//���տͻ������� ������������
	while (true) {
		char _recvBuff[128] = {};
		int nLen = recv(_csock, _recvBuff, 128, 0);
		if (nLen <= 0) {
			printf("�ͻ���û�з�����Ч����");
			break;
		}
		else {
			printf("���յ���Ϣ��%s\n", _recvBuff);
			char returnBuff[128] = {};
			if (0 == strcmp(_recvBuff, "getName")) {
				strcpy_s(returnBuff, "���Ƿ�����\n");
			}
			else if (0 == strcmp(_recvBuff, "getAge")) {
				strcpy_s(returnBuff, "��0����\n");
			}
			else if (0 == strcmp(_recvBuff, "getInfo")) {
				DataPackage data = {41,"С��"};
				send(_csock, (const char*) &data, sizeof(data), 0);
			}
			else {
				strcpy_s(returnBuff, "�ҿ��԰�����ʲô��\n");
			}
			send(_csock, returnBuff, sizeof(returnBuff), 0);
		}
	}

	//�ر�socket
	closesocket(_socket);
	WSACleanup();
	printf("����������رշ�������\n");
	getchar();
	return 0;
}