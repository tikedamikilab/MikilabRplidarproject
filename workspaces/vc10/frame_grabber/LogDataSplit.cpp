#include "stdafx.h"
#include "LogDataSplit.h"

#include <iostream>
using namespace std;

LogDataSplit::LogDataSplit()
{
}
LogDataSplit::~LogDataSplit()
{
}
void LogDataSplit::DataSplitBasic(time_t start, time_t end, std::vector<scanDot_Complication> &BaseData, int magnification, int colormodedefine)
{
	//******************
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	std::cout << "LogDataSplit" << std::endl;
	//******************
	Initialization();
	setSplitStartTime(start);
	setSplitEndTime(end);

	std::cout << "LogDataSplit ******Initialization******" << std::endl;
	if (colormodedefine == 0) {
		for (int i = 0; i < BaseData.size(); i++) {
			if (start <= BaseData[i].now && BaseData[i].now <= end) {
				//同じUniqeIDが連続することを防ぐ
				if (resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).uniqeIDs.size() != 0) {
					if (resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).uniqeIDs.back() == BaseData[i].uniqueid) {
						continue;
					}
				}
				//XY座標にpreviousのデータをpush
				resplitdata[SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X].previousX.push_back(BaseData[i].previousX);
				resplitdata[SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X].previousY.push_back(BaseData[i].previousY);
				resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).RGBsum++;
				resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).uniqeIDs.push_back(BaseData[i].uniqueid);

				if (BaseData[i].uniqueid % 3 == 0) {
					resplitdata[SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X].R++;
				}
				else if (BaseData[i].uniqueid % 3 == 1) {
					resplitdata[SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X].G++;
				}
				else {
					resplitdata[SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X].B++;
				}
				/*
				resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X)
					.previousX[resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).RGBsum] = BaseData[i].previousX;
				resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X)
					.previousY[resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).RGBsum] = BaseData[i].previousY;
				*/
			}
		}
	}
	else if(colormodedefine == 1){ //(colormodedefine == 1)
		for (int i = 0; i < BaseData.size(); i++) {
			if (start <= BaseData[i].now && BaseData[i].now <= end) {
				//点と点の間を埋めるプログラム
				if (abs(BaseData[i].X - BaseData[i].previousX) >= abs(BaseData[i].Y - BaseData[i].previousY)) {
					float ratio = 0.0;
					if (BaseData[i].previousX - BaseData[i].X != 0)
					{
						ratio = (float)(BaseData[i].previousY - BaseData[i].Y) / (float)(BaseData[i].previousX - BaseData[i].X);
					}
					if (BaseData[i].previousX >= BaseData[i].X) {
						try{
							for (int x = BaseData[i].X; x < BaseData[i].previousX; x++) {
								resplitdata.at(SPLIT_VIEW_X * (int)(BaseData[i].Y + ratio * (x - BaseData[i].X)) + x).RGBsum++;
							}
						}
						catch (std::exception &e) {
							std::cout << "LogDataSplit******1******" << std::endl;
						}
					}
					else {  //BaseData[i].previousX < BaseData[i].X
						try{
							for (int x = BaseData[i].X; x > BaseData[i].previousX; x--) {
								resplitdata.at(SPLIT_VIEW_X * (int)(BaseData[i].Y + ratio * (x - BaseData[i].X)) + x).RGBsum++;
							}
						}
						catch (std::exception &e) {
							std::cout << "LogDataSplit******2******" << std::endl;
						}
					}
				}
				else {  //abs(BaseData[i].X - BaseData[i].previousX) < abs(BaseData[i].Y - BaseData[i].previousY)
					float ratio = 0.0;
					if (BaseData[i].previousY - BaseData[i].Y != 0) {  //elseに入る条件から，このifにはじかれる場合は
						ratio = (float)(BaseData[i].previousX - BaseData[i].X) / (float)(BaseData[i].previousY - BaseData[i].Y);
					}
					if (BaseData[i].previousY >= BaseData[i].Y) {
						try{
							for (int y = BaseData[i].Y; y < BaseData[i].previousY; y++) {
								resplitdata.at(SPLIT_VIEW_X * y + (BaseData[i].X + ratio * (y - BaseData[i].Y))).RGBsum++;
							}
						}
						catch (std::exception &e) {
							std::cout << "LogDataSplit******3******" << std::endl;
						}
					}
					else {  //BaseData[i].previousX - BaseData[i].X < 0
						try {
							for (int y = BaseData[i].Y; y > BaseData[i].previousY; y--) {
								resplitdata.at(SPLIT_VIEW_X * y + (BaseData[i].X + ratio * (y - BaseData[i].Y))).RGBsum++;
							}
						}
						catch (std::exception &e) {
							std::cout << "LogDataSplit******4******" << std::endl;
						}
					}
				}
				//std::cout << "LogDataSplit******8******" << std::endl;
				resplitdata[SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X].previousX.push_back(BaseData[i].previousX);
			}
		}
	}
	else if (colormodedefine == 2) {
		for (int i = 0; i < BaseData.size(); i++) {
			//std::cout << " : " << BaseData[i].previousX << " : " << BaseData[i].X << " : " << BaseData[i].previousY << " : " << BaseData[i].Y << std::endl;
			if (start <= BaseData[i].now && BaseData[i].now <= end) {
				try {
					//同じUniqeIDが連続することを防ぐ
					if (resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).uniqeIDs.size() != 0) {
						if (resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).uniqeIDs.back() == BaseData[i].uniqueid) {
							continue;
						}
					}
					//XY座標にpreviousのデータをpush
					resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).previousX.push_back(BaseData[i].previousX);
					resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).previousY.push_back(BaseData[i].previousY);
					resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).RGBsum++;
					resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).uniqeIDs.push_back(BaseData[i].uniqueid);
				}
				catch (std::exception &e) {
					std::cout << "LogDataSplit******3******" << std::endl;
				}
			}
		}
	}
	else if (colormodedefine == 3) { //(colormodedefine == 1)
		for (int i = 0; i < BaseData.size(); i++) {
			if (start <= BaseData[i].now && BaseData[i].now <= end) {
				//点と点の間を埋めるプログラム
				if (abs(BaseData[i].X - BaseData[i].previousX) >= abs(BaseData[i].Y - BaseData[i].previousY)) {
					float ratio = 0.0;
					if (BaseData[i].previousX - BaseData[i].X != 0)
					{
						ratio = (float)(BaseData[i].previousY - BaseData[i].Y) / (float)(BaseData[i].previousX - BaseData[i].X);
					}
					if (BaseData[i].previousX >= BaseData[i].X) {
						try {
							for (int x = BaseData[i].X; x < BaseData[i].previousX; x++) {
								//同じUniqeIDが連続することを防ぐ
								int tmpi = SPLIT_VIEW_X * (int)(BaseData[i].Y + ratio * (x - BaseData[i].X)) + x;
								OnUniqeIDCheck(tmpi, BaseData[i].uniqueid);
							}
						}
						catch (std::exception &e) {
							std::cout << "LogDataSplit******1******" << std::endl;
						}
					}
					else {  //BaseData[i].previousX < BaseData[i].X
						try {
							for (int x = BaseData[i].X; x > BaseData[i].previousX; x--) {
								int tmpi = SPLIT_VIEW_X * (int)(BaseData[i].Y + ratio * (x - BaseData[i].X)) + x;
								OnUniqeIDCheck(tmpi, BaseData[i].uniqueid);
							}
						}
						catch (std::exception &e) {
							std::cout << "LogDataSplit******2******" << std::endl;
						}
					}
				}
				else {  //abs(BaseData[i].X - BaseData[i].previousX) < abs(BaseData[i].Y - BaseData[i].previousY)
					float ratio = 0.0;
					if (BaseData[i].previousY - BaseData[i].Y != 0) {  //elseに入る条件から，このifにはじかれる場合は
						ratio = (float)(BaseData[i].previousX - BaseData[i].X) / (float)(BaseData[i].previousY - BaseData[i].Y);
					}
					if (BaseData[i].previousY >= BaseData[i].Y) {
						try {
							for (int y = BaseData[i].Y; y < BaseData[i].previousY; y++) {
								int tmpi = SPLIT_VIEW_X * y + (BaseData[i].X + ratio * (y - BaseData[i].Y));
								OnUniqeIDCheck(tmpi, BaseData[i].uniqueid);
							}
						}
						catch (std::exception &e) {
							std::cout << "LogDataSplit******3******" << std::endl;
						}
					}
					else {  //BaseData[i].previousX - BaseData[i].X < 0
						try {
							for (int y = BaseData[i].Y; y > BaseData[i].previousY; y--) {
								int tmpi = SPLIT_VIEW_X * y + (BaseData[i].X + ratio * (y - BaseData[i].Y));
								OnUniqeIDCheck(tmpi, BaseData[i].uniqueid);
							}
						}
						catch (std::exception &e) {
							std::cout << "LogDataSplit******4******" << std::endl;
						}
					}
				}
				//std::cout << "LogDataSplit******8******" << std::endl;
				resplitdata[SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X].previousX.push_back(BaseData[i].previousX);
			}
		}
	}
	else if (colormodedefine == 99) {
		for (int i = 0; i < BaseData.size(); i++) {
			//std::cout << " : " << BaseData[i].previousX << " : " << BaseData[i].X << " : " << BaseData[i].previousY << " : " << BaseData[i].Y << std::endl;
			if (start <= BaseData[i].now && BaseData[i].now <= end) {
				try {
					//同じUniqeIDが連続することを防ぐ
					if (resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).uniqeIDs.size() != 0) {
						if (resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).uniqeIDs.back() == BaseData[i].uniqueid) {
							continue;
						}
					}
					//XY座標にpreviousのデータをpush
					resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).previousX.push_back(BaseData[i].previousX);
					resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).previousY.push_back(BaseData[i].previousY);
					resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).RGBsum++;
					resplitdata.at(SPLIT_VIEW_X * BaseData[i].Y + BaseData[i].X).uniqeIDs.push_back(BaseData[i].uniqueid);
				}
				catch (std::exception &e) {
					std::cout << "LogDataSplit******3******" << std::endl;
				}
			}
		}
	}
	else {}

	for (int i = 0; i < resplitdata.size(); i++) {
		resplitdata[i].RGBsum *= magnification;
		resplitdata[i].R *= magnification;
		resplitdata[i].G *= magnification;
		resplitdata[i].B *= magnification;
		if (resplitdata[i].RGBsum > 255) {
			resplitdata[i].RGBsum = 255;
		}
		if (resplitdata[i].R > 255) {
			resplitdata[i].R = 255;
		}
		if (resplitdata[i].G > 255) {
			resplitdata[i].G = 255;
		}
		if (resplitdata[i].B > 255) {
			resplitdata[i].B = 255;
		}
	}
}

void LogDataSplit::setSplitStartTime(time_t start){
	timesplitstart = start;
}
void LogDataSplit::setSplitEndTime(time_t end){
	timesplitend = end;
}
void LogDataSplit::Initialization() {
	resplitdata = vector<scanDot_RGBCount> (SPLIT_VIEW_X * SPLIT_VIEW_Y);
	for (int i = 0; i < SPLIT_VIEW_X * SPLIT_VIEW_Y; i++) {
		try{
			resplitdata.at(i).RGBsum = 0;
			resplitdata.at(i).R = 0;
			resplitdata.at(i).G = 0;
			resplitdata.at(i).B = 0;
		}
		catch (std::exception &e) {
		}
	}
}
std::vector<scanDot_RGBCount>& LogDataSplit::getReSplitData() {
	return resplitdata;
}
inline bool LogDataSplit::OnUniqeIDCheck(int argi, int arguniqeid) {
	if (resplitdata.at(argi).uniqeIDs.size() != 0) {
		if (resplitdata.at(argi).uniqeIDs.back() == arguniqeid) {
			return false;
		}
	}
	resplitdata.at(argi).RGBsum++;
	resplitdata.at(argi).uniqeIDs.push_back(arguniqeid);
	return true;
}