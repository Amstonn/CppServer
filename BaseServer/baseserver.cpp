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
	short dataLength;//���ݳ���
	short cmd;//����
};

//��½�ṹ��
struct LoginData : public DataHeader{ //ͨ���̳н�ͷ���� �����Ϣ��
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

int process(SOCKET _csock) {
	DataHeader header = {};
	int nLen = recv(_csock, (char*)&header, sizeof(header), 0);
	if (nLen <= 0) {
		printf("�ͻ���û�з�����Ч����");
		return -1;
	}
	else {
		switch (header.cmd) {
		case CMD_LOGIN:
		{
			LoginData login_data = {};
			//��ǰ������һ��DataHeader ����������Ҫ��ƫ��  �Ի�ȡʣ�������
			recv(_csock, (char*)&login_data + sizeof(DataHeader), sizeof(LoginData) - sizeof(DataHeader), 0);
			printf("�յ����CMD_LGOIN ���ݳ��ȣ�%d �û�����%s\n", login_data.dataLength, login_data.userName);
			LoginResult login_res;
			login_res.result = 1;
			send(_csock, (char*)&login_res, sizeof(login_res), 0);
		}
		break;
		case CMD_LOGOUT:
		{
			LogoutData logout = {};
			recv(_csock, (char*)&logout + sizeof(DataHeader), sizeof(LogoutData) - sizeof(DataHeader), 0);
			printf("�յ����CMD_LOGOUT �û�����%s\n", logout.userName);
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
	
	//���տͻ������� ������������
	while (true) {
		fd_set readfds;
		FD_ZERO(&readfds); //��ʼ��Ϊ�ռ���
		FD_SET(_socket, &readfds);
		for (auto filedis : fd) {
			FD_SET(filedis, &readfds); //�׽��ּ��뵽set����
		}
		int ret = select(_socket + 1, &readfds, NULL,NULL,NULL);
		if (ret < 0) {  //����������ѭ��
			perror("select error!");
			break;
		}
		if (FD_ISSET(_socket, &readfds)) {
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
			fd.push_back(_csock); //�������ӵĿͻ���socket�ŵ��ļ���������
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

	//�ر�socket
	for (auto item : fd) {
		closesocket(item);
	}
	closesocket(_socket);
	WSACleanup();
	printf("����������رշ�������\n");
	getchar();
	return 0;
}