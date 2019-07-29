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
	// ソケット通信winsockの立ち上げ
	// wsaDataはエラー取得等に使用する
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと

											// socket作成
											// socketは通信の出入り口 ここを通してデータのやり取りをする
											// socket(アドレスファミリ, ソケットタイプ, プロトコル)
	sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？

											// アドレス等格納
	addr.sin_family = AF_INET;  //IPv4
	addr.sin_port = htons(34567);   //通信ポート
	//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");  // 送信アドレスを127.0.0.1に設定 
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.68");
}

void MoveObjSend::UDPSocketClose() {
	// socketの破棄
	closesocket(sock);
	// winsockの終了
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
	// バッファ ここに送信するデータを入れる  サイズは自由に決められるが、char配列
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
	// 送信
	// sendto(ソケット, 送信するデータ, データのバイト数, フラグ, アドレス情報, アドレス情報のサイズ);
	// 送信するデータに直接文字列 "HELLO" 等を入れることもできる
	// バインドしている場合は send(sock, buf, 5, 0); でもOK？

	//std::wcout << "send" << std::endl;
	sendto(sock, (const char *)(&bufall), sizeof(bufall), 0, (struct sockaddr *)&addr, sizeof(addr));
}


/*********************************/
//
//   ここから下，使わない
//
/*********************************/
void MoveObjSend::UDPSendAll(std::vector<scanDot_Complication> _obj_send) {
	// ソケット通信winsockの立ち上げ
	// wsaDataはエラー取得等に使用する
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと

											// socket作成
											// socketは通信の出入り口 ここを通してデータのやり取りをする
											// socket(アドレスファミリ, ソケットタイプ, プロトコル)
	SOCKET sock;
	sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？

											// アドレス等格納
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;  //IPv4
	addr.sin_port = htons(34567);   //通信ポート
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	/*--------------*/
	//単体デモ用（送信しない）
	/*--------------*/

	// バッファ ここに送信するデータを入れる  サイズは自由に決められるが、char配列
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
	// 送信
	// sendto(ソケット, 送信するデータ, データのバイト数, フラグ, アドレス情報, アドレス情報のサイズ);
	// 送信するデータに直接文字列 "HELLO" 等を入れることもできる
	// バインドしている場合は send(sock, buf, 5, 0); でもOK？

	//std::wcout << "send" << std::endl;
	sendto(sock, (const char *)(&bufall), sizeof(bufall), 0, (struct sockaddr *)&addr, sizeof(addr));

	// socketの破棄
	closesocket(sock);

	// winsockの終了
	WSACleanup();
}
