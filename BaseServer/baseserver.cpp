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
		DataHeader header = {};
		int nLen = recv(_csock, (char *)&header, sizeof(header), 0);
		if (nLen <= 0) {
			printf("�ͻ���û�з�����Ч����");
			break;
		}
		else {
			printf("���յ���Ϣ������ %d  ���ݳ���  %d \n", header.cmd, header.dataLength);
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

	//�ر�socket
	closesocket(_socket);
	WSACleanup();
	printf("����������رշ�������\n");
	getchar();
	return 0;
}