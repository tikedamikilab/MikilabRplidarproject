#pragma once

#include "stdafx.h"
#include "MoveObjRecieve.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>
//#include <mutex>

using namespace std;
//std::mutex mtx_recieve; // �r������p�~���[�e�b�N�X
//std::vector<scanDot_Complication> _globle_obj_recieve;

MoveObjRecieve::MoveObjRecieve()
{
}

MoveObjRecieve::~MoveObjRecieve()
{
}

void MoveObjRecieve::UDPRecieveAll() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// �\�P�b�g�ʐMwinsock�̗����グ
	// wsaData�̓G���[�擾���Ɏg�p����
	WSAData wsaData;
	int iResult;
	WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)��winsock�̃o�[�W����2.0���Ă���
											// socket�쐬
											// socket�͒ʐM�̏o����� ������ʂ��ăf�[�^�̂���������
											// socket(�A�h���X�t�@�~��, �\�P�b�g�^�C�v, �v���g�R��)
	SOCKET sock;
	sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INET��IPv4�ASOCK_DGRAM��UDP�ʐM�A0�́H
											// �A�h���X���i�[
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;  //IPv4
	addr.sin_port = htons(Port);   //�ʐM�|�[�g�ԍ��ݒ�
	addr.sin_addr.S_un.S_addr = INADDR_ANY; // INADDR_ANY�͂��ׂẴA�h���X����̃p�P�b�g����M����
											// �o�C���h
											// �A�h���X���̏���socket�ɓo�^����

	bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	// �u���b�L���O�A�m���u���b�L���O�̐ݒ�
	//�i�C�ӁA�K�{�R�[�h�ł͂Ȃ��i�\�P�b�g�̏����ݒ�̓u���b�L���O���[�h�Ȃ��߁j
	// val = 0 : �u���b�L���O���[�h �f�[�^����M�����܂őҋ@
	// val = 1 : �m���u���b�L���O �f�[�^����M����Ȃ��Ă����̏�����
	u_long val = 0;
	iResult = ioctlsocket(sock, FIONBIO, &val);
	if (iResult != NO_ERROR){
		std::wcout << "ioctlsocket failed with error: %ld\n" << std::endl;
	}
	// �o�b�t�@ �����Ɏ�M�����f�[�^������ �T�C�Y�͎��R�Ɍ��߂��邪�Achar�z��
	char buf[2000];
	char bufuniqeid[15];
	char bufdist[15];
	char bufangle[15];
	while (1) {
		int cntuniqeid = 0;
		int cntdist = 0;
		int cntangle = 0;
		int cntall = 0;
		//std::unique_ptr<char[]> buf(new char[6000]);
		//memset(buf, 0, sizeof(buf)); // �������M����Ƃ��͑O��l���c��Ȃ��悤�ɂ��̂悤�� �o�b�t�@��0�ŃN���A����̂���ʓI �������[����֐�
		// ��M
		// recv(�\�P�b�g, ��M����f�[�^�̊i�[��, �f�[�^�̃o�C�g��, �t���O);
		// �o�C���h���Ă��Ȃ��ꍇ�� recvfrom(sock, buf, 5, 0, (struct sockaddr *)&addr, sizeof(addr)); �ł�OK�H
		recv(sock, buf, sizeof(buf), 0);
		//ATLASSERT(_CrtCheckMemory());
		for (int i = 0; i < sizeof(buf); i++) {
			//ATLASSERT(_CrtCheckMemory());
			if (buf[i] == '\0') {
				break;
			}
			else if (buf[i] == ',') {
				cntall++;
				if (cntall % 3 == 0) {
					std::string str1 = std::string(bufuniqeid);
					int uniqueid = stoi(str1);
					std::string str2 = std::string(bufdist);
					float dist = stof(str2);
					std::string str3 = std::string(bufangle);
					float angle = stof(str3) / 100;
					//printf("%d:%f:%f\n", uniqueid, dist, angle);
					/*---------------*/
					recievescanview.setObjAnother(uniqueid, dist, angle);
					/*---------------*/
					cntuniqeid = 0;
					cntdist = 0;
					cntangle = 0;
				}
			}
			else {
				if (cntall % 3 == 0 && cntuniqeid < sizeof(bufuniqeid)) {
					bufuniqeid[cntuniqeid] = buf[i];
					bufuniqeid[cntuniqeid + 1] = '\0';
					cntuniqeid++;
				}
				else if (cntall % 3 == 1 && cntdist < sizeof(bufdist)) {
					bufdist[cntdist] = buf[i];
					bufdist[cntdist + 1] = '\0';
					cntdist++;
				}
				else if (cntangle < sizeof(bufangle)) {
					bufangle[cntangle] = buf[i];
					bufangle[cntangle + 1] = '\0';
					cntangle++;
				}
				else {
					break;
				}
			}
		}
		ATLASSERT(_CrtCheckMemory());
		//std::wcout << "recieve:" << cntall / 3 << std::endl;
	}
	// socket�̔j��
	closesocket(sock);
	// winsock�̏I��
	WSACleanup();
}
/*
std::vector<scanDot_Complication>& MoveObjRecieve::GetObjRecive() {
	//std::unique_lock<std::mutex> lock(mtx_recieve);
	std::vector<scanDot_Complication> _obj_tmp(_globle_obj_recieve);
	OnObjReciveClear();
	return _obj_tmp;
}
void MoveObjRecieve::OnObjRecivePush(scanDot_Complication &tmp) {
	//std::unique_lock<std::mutex> lock(mtx_recieve);
	_globle_obj_recieve.push_back(tmp);
}
void MoveObjRecieve::OnObjReciveClear() {
	//std::unique_lock<std::mutex> lock(mtx_recieve);
	_globle_obj_recieve.clear();
}
*/