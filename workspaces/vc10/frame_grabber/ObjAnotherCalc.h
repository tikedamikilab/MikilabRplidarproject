#pragma once
#include "StructscanDot.h"

class ObjAnotherCalc
{
private:
	void OnObjAnotherInitialize(std::vector<scanDot_Complication> &_obj_another, std::vector<scanDot_Complication> &_obj_another_old);
	void OnsetDistScal(float _distscal);
	void OnsetCenterPt(CPoint _centerPt);
	void OnObjAnotherIntegration(std::vector<scanDot_Complication> &_obj_another);
	void OnObjAnotherTrace(std::vector<scanDot_Complication> &_obj_another, std::vector<scanDot_Complication> &_obj_another_old);
	float                _distscal = 0;
	CPoint               _centerPt;
public:
	ObjAnotherCalc();
	~ObjAnotherCalc();
	void ObjAnotherAll(std::vector<scanDot_Complication> &_obj_another, std::vector<scanDot_Complication> &_obj_another_old, float distscal, CPoint _centerPt);

};

