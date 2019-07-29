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
//std::mutex mtx_recieve; // 排他制御用ミューテックス
//std::vector<scanDot_Complication> _globle_obj_recieve;

MoveObjRecieve::MoveObjRecieve()
{
}

MoveObjRecieve::~MoveObjRecieve()
{
}

void MoveObjRecieve::UDPRecieveAll() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// ソケット通信winsockの立ち上げ
	// wsaDataはエラー取得等に使用する
	WSAData wsaData;
	int iResult;
	WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
											// socket作成
											// socketは通信の出入り口 ここを通してデータのやり取りをする
											// socket(アドレスファミリ, ソケットタイプ, プロトコル)
	SOCKET sock;
	sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？
											// アドレス等格納
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;  //IPv4
	addr.sin_port = htons(Port);   //通信ポート番号設定
	addr.sin_addr.S_un.S_addr = INADDR_ANY; // INADDR_ANYはすべてのアドレスからのパケットを受信する
											// バインド
											// アドレス等の情報をsocketに登録する

	bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	// ブロッキング、ノンブロッキングの設定
	//（任意、必須コードではない（ソケットの初期設定はブロッキングモードなため）
	// val = 0 : ブロッキングモード データが受信されるまで待機
	// val = 1 : ノンブロッキング データが受信されなくても次の処理へ
	u_long val = 0;
	iResult = ioctlsocket(sock, FIONBIO, &val);
	if (iResult != NO_ERROR){
		std::wcout << "ioctlsocket failed with error: %ld\n" << std::endl;
	}
	// バッファ ここに受信したデータが入る サイズは自由に決められるが、char配列
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
		//memset(buf, 0, sizeof(buf)); // 複数回受信するときは前回値が残らないようにこのように バッファを0でクリアするのが一般的 メモリー操作関数
		// 受信
		// recv(ソケット, 受信するデータの格納先, データのバイト数, フラグ);
		// バインドしていない場合は recvfrom(sock, buf, 5, 0, (struct sockaddr *)&addr, sizeof(addr)); でもOK？
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
	// socketの破棄
	closesocket(sock);
	// winsockの終了
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