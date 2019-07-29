#pragma once
const int OBJ_MOVE_SIZE = 10;
class DataCalc
{
public:
	DataCalc(std::vector<scanDot> &ScanDot, const int maxPixelR, const float distScale);
	virtual ~DataCalc();
	void OnObjCenterCalc(std::vector<scanDot> &ScanData, std::vector<scanDot_Complication> &_obj_center
		, const int maxPixelR, const float distScale);


protected:
	std::vector<scanDot_Simple> _obj_center;
	std::vector<scanDot_Complication> _obj_semi_move;
	std::vector<scanDot_Complication> _obj_move[OBJ_MOVE_SIZE];
	int maxPixeIR;
	float distScale;
	
};
