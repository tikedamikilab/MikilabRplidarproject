#pragma once
#include <ctime>
#include "StructscanDot.h"

const int SPLIT_VIEW_X = 1600;
const int SPLIT_VIEW_Y = 1000;
class LogDataSplit
{
public:
	LogDataSplit();
	~LogDataSplit();
	void DataSplitBasic(time_t start, time_t end, std::vector<scanDot_Complication> &BaseData, int magnification, int colormodedefine);
	void setSplitStartTime(time_t start);
	void setSplitEndTime(time_t end);
	void Initialization();
	std::vector<scanDot_RGBCount>& getReSplitData();
	bool OnUniqeIDCheck(int argi, int argUniqeID);
	
protected:
	time_t timesplitstart;
	time_t timesplitend;
	std::vector<scanDot_RGBCount> resplitdata;
};

