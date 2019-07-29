#pragma once
#include "StructscanDot.h"

class MoveObjSend
{
private:
	SOCKET sock;
	struct sockaddr_in addr;

	void UDPSendAll(std::vector<scanDot_Complication> _obj_send);
	void UDPSocketMake();
	void UDPSocketClose();
	void UDPSendData(std::vector<scanDot_Complication> _obj_send);
public:
	MoveObjSend();
	~MoveObjSend();
	
	void UDPSendCheck(std::vector<scanDot_Complication> _obj_send, int position);
};

