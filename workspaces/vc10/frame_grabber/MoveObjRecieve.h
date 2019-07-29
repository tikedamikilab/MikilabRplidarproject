#pragma once
#include "scanView.h"

const int Port = 34567;
class MoveObjRecieve
{
private:
	CScanView recievescanview;

	//void OnObjRecivePush(scanDot_Complication &tmp);
	//void OnObjReciveClear();

public:
	MoveObjRecieve();
	~MoveObjRecieve();

	void UDPRecieveAll();
	//std::vector<scanDot_Complication>& GetObjRecive();
};

