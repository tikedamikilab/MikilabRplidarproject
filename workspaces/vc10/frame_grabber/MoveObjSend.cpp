#pragma once
#include "stdafx.h"
#include "MoveObjSend.h"
#include <iostream>
#include <winsock2.h>
#include <string>

using namespace std;

MoveObjSend::MoveObjSend()
{
	UDPSocketMake();
}
MoveObjSend::~MoveObjSend()
{
	UDPSocketClose();
}

void MoveObjSend::UDPSocketMake() {
	// �\�P�b�g�ʐMwinsock�̗����グ
	// wsaData�̓G���[�擾���Ɏg�p����
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)��winsock�̃o�[�W����2.0���Ă���

											// socket�쐬
											// socket�͒ʐM�̏o����� ������ʂ��ăf�[�^�̂���������
											// socket(�A�h���X�t�@�~��, �\�P�b�g�^�C�v, �v���g�R��)
	sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INET��IPv4�ASOCK_DGRAM��UDP�ʐM�A0�́H

											// �A�h���X���i�[
	addr.sin_family = AF_INET;  //IPv4
	addr.sin_port = htons(34567);   //�ʐM�|�[�g
	//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");  // ���M�A�h���X��127.0.0.1�ɐݒ� 
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.68");
}

void MoveObjSend::UDPSocketClose() {
	// socket�̔j��
	closesocket(sock);
	// winsock�̏I��
	WSACleanup();
}

void MoveObjSend::UDPSendCheck(std::vector<scanDot_Complication> _obj_send, int position) {
	if (_obj_send.size() > 0 /*&& position != 0*/) {
		UDPSendData(_obj_send);
	}
	else {
	}
}

void MoveObjSend::UDPSendData(std::vector<scanDot_Complication> _obj_send) {
	// �o�b�t�@ �����ɑ��M����f�[�^������  �T�C�Y�͎��R�Ɍ��߂��邪�Achar�z��
	char buf[100];
	char bufall[2000];
	//memset(bufall, 0, sizeof(bufall));
	int bufuniqeid;
	double bufdist;
	double bufangle;
	for (int i = 0; i < (int)_obj_send.size(); i++) {
		ATLASSERT(_CrtCheckMemory());
		memset(buf, 0, sizeof(buf));
		bufuniqeid = _obj_send[i].uniqueid;
		bufdist = _obj_send[i].dist;
		bufangle = _obj_send[i].angle;
		sprintf_s(buf, "%d,%d,%d,", bufuniqeid, (int)bufdist, (int)(bufangle * 100));
		if (i == 0) {
			strcpy(bufall, buf);
		}
		else {
			strcat(bufall, buf);
		}
	}
	ATLASSERT(_CrtCheckMemory());

	//strcat(bufall, buf);
	// ���M
	// sendto(�\�P�b�g, ���M����f�[�^, �f�[�^�̃o�C�g��, �t���O, �A�h���X���, �A�h���X���̃T�C�Y);
	// ���M����f�[�^�ɒ��ڕ����� "HELLO" �������邱�Ƃ��ł���
	// �o�C���h���Ă���ꍇ�� send(sock, buf, 5, 0); �ł�OK�H

	//std::wcout << "send" << std::endl;
	sendto(sock, (const char *)(&bufall), sizeof(bufall), 0, (struct sockaddr *)&addr, sizeof(addr));
}


/*********************************/
//
//   �������牺�C�g��Ȃ�
//
/*********************************/
void MoveObjSend::UDPSendAll(std::vector<scanDot_Complication> _obj_send) {
	// �\�P�b�g�ʐMwinsock�̗����グ
	// wsaData�̓G���[�擾���Ɏg�p����
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)��winsock�̃o�[�W����2.0���Ă���

											// socket�쐬
											// socket�͒ʐM�̏o����� ������ʂ��ăf�[�^�̂���������
											// socket(�A�h���X�t�@�~��, �\�P�b�g�^�C�v, �v���g�R��)
	SOCKET sock;
	sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INET��IPv4�ASOCK_DGRAM��UDP�ʐM�A0�́H

											// �A�h���X���i�[
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;  //IPv4
	addr.sin_port = htons(34567);   //�ʐM�|�[�g
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	/*--------------*/
	//�P�̃f���p�i���M���Ȃ��j
	/*--------------*/

	// �o�b�t�@ �����ɑ��M����f�[�^������  �T�C�Y�͎��R�Ɍ��߂��邪�Achar�z��
	char buf[100];
	char bufall[4000];
	//memset(bufall, 0, sizeof(bufall));
	int bufuniqeid;
	double bufdist;
	double bufangle;
	for (int i = 0; i < (int)_obj_send.size(); i++) {
		ATLASSERT(_CrtCheckMemory());
		memset(buf, 0, sizeof(buf));
		bufuniqeid = _obj_send[i].uniqueid;
		bufdist = _obj_send[i].dist;
		bufangle = _obj_send[i].angle;
		sprintf_s(buf, "%d,%d,%d,", bufuniqeid, (int)bufdist, (int)(bufangle * 100));
		if (i == 0) {
			strcpy(bufall, buf);
		}
		else {
			strcat(bufall, buf);
		}
	}
	ATLASSERT(_CrtCheckMemory());

	//strcat(bufall, buf);
	// ���M
	// sendto(�\�P�b�g, ���M����f�[�^, �f�[�^�̃o�C�g��, �t���O, �A�h���X���, �A�h���X���̃T�C�Y);
	// ���M����f�[�^�ɒ��ڕ����� "HELLO" �������邱�Ƃ��ł���
	// �o�C���h���Ă���ꍇ�� send(sock, buf, 5, 0); �ł�OK�H

	//std::wcout << "send" << std::endl;
	sendto(sock, (const char *)(&bufall), sizeof(bufall), 0, (struct sockaddr *)&addr, sizeof(addr));

	// socket�̔j��
	closesocket(sock);

	// winsock�̏I��
	WSACleanup();
}
