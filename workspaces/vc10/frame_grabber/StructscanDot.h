#pragma once
#include "stdafx.h"
//#include <ctime>
#include <windows.h>
using namespace std;

struct scanDot {
	_u8   quality;
	float angle;
	float dist;
};
struct scanDot_Simple {
	int   cnt;
	float angle;
	float dist;
	float distX;  //mm単位の直交座標
	float distY;  //mm単位の直交座標
	int X;
	int Y;
};
struct scanDot_Complication :public scanDot_Simple {
	float previousangle;
	float previousdist;
	int previousX;
	int previousY;
	int uniqueid;
	int flag;
	int milliseconds;
	time_t now;
};
struct scanDot_time :public scanDot_Complication {
	int month;
	int day;
	int hour;
	int min;
	int sec;
};
struct scanDot_RGBCount {
	int R;
	int G;
	int B;
	int RGBsum;
	std::vector<int> previousX;
	std::vector<int> previousY;
	std::vector<int> uniqeIDs;
};
