#include "DataCalc.h"
#include "scanView.h"
#include <vector> 
#include <algorithm>

const int DEF_DISTANCE = 8;
const float PI = (float)3.14159265;
bool operator<(const scanDot& left, const scanDot& right)
{
	return left.angle < right.angle;
}

bool operator>(const scanDot& left, const scanDot& right)
{
	return left.angle > right.angle;
}
DataCalc::DataCalc(std::vector<scanDot> &ScanData,const int getmaxPixlR, const float distScale)
{
	maxPixeIR = getmaxPixlR;
}

DataCalc::~DataCalc()
{
}

void DataCalc::OnObjCenterCalc(std::vector<scanDot> &ScanData, std::vector<scanDot_Complication> &_obj_center
	, const int maxPixelR, const float distScale) {
	int picked_point = 0;
	float min_picked_dangle = 100;
	scanDot_Simple objsum;
	objsum.angle = 0; objsum.dist = 0; objsum.cnt = 0;
	float endptX_bf = 0;
	float endptY_bf = 0;
	float endptX_0 = 0;
	float endptY_0 = 0;
	float distance;

	//sort
	std::sort(ScanData.begin(), ScanData.end());
	//noise delete
	for (int pos = 1; pos < (int)ScanData.size() - 1; ++pos) {
		float dist_now = ScanData[pos].dist;
		float dist_bf = ScanData[pos - 1].dist;
		float dist_af = ScanData[pos + 1].dist;
		if (abs(dist_now - dist_bf) > 300 && abs(dist_bf - dist_af) < 600) {
			ScanData[pos].dist = (dist_bf + dist_af) / 2;
		}
	}
	//file define 部分
	//char file_name[32] = "save.csv";
	//FILE *fpw;
	//fpw = fopen(file_name, "w");
	//fprintf(fpw, "%d,\n", _cnt_all);
	//fclose(fpw);
	//ここまで↑
	for (int pos = 0; pos < (int)ScanData.size(); ++pos) {
		float dist = ScanData[pos].dist;
		float angle = ScanData[pos].angle;
		float distPixel = dist * distScale;
		float rad = (float)(angle*PI / 180.0);
		float endptX = sin(rad)*(distPixel)+centerPt.x;
		float endptY = centerPt.y - cos(rad)*(distPixel);
		float dangle = fabs(rad - _mouse_angle);
		float endptXaf;
		float endptYaf;
		//float _endptX = sin((float)(_scan_data[pos].angle*PI/180.0))*(_scan_data[pos].dist*distScale) + centerPt.x;
		//float _endptY = centerPt.y - cos((float)(_scan_data[pos].angle*PI/180.0))*(_scan_data[pos].dist*distScale);
		//追加↓ 塊に分ける

		bool move_area = OnMoveArea(endptX, endptY);
		if (move_area == False) {  //データが条件に合うかの確認
			//memDC.FillSolidRect((int)endptX - 1, (int)endptY - 1, 2, 2, RGB(128, 128, 128));
			memDC.FillSolidRect((int)endptX - 1, (int)endptY - 1, 2, 2, RGB(0, 0, 0));
			if (pos == 0) {
				endptX_0 = endptX;
				endptY_0 = endptY;
			}
			if (dangle < min_picked_dangle) {
				min_picked_dangle = dangle;
				picked_point = pos;
			}
			continue;  //合わなければ処理を飛ばす
		}
		//OnColorPolt(&memDC, (int)_obj_center[0].size(), endptX, endptY);
		//memDC.FillSolidRect((int)endptX - 1, (int)endptY - 1, 2, 2, RGB(150, 150, 50));
		memDC.FillSolidRect((int)endptX - 1, (int)endptY - 1, 2, 2, RGB(0, 0, 0));
		if (pos == 0) {
			OnObjSum(&objsum, dist, angle, False);
			endptX_0 = endptX;
			endptY_0 = endptY;
		}
		else if (pos != (int)ScanData.size() - 1) {
			//一個先との差
			endptXaf = sin((float)(ScanData[pos + 1].angle*PI / 180.0))*(ScanData[pos + 1].dist*distScale) + centerPt.x;
			endptYaf = centerPt.y - cos((float)(ScanData[pos + 1].angle*PI / 180.0))*(ScanData[pos + 1].dist*distScale);
			distance = sqrtf(std::pow(endptX - endptXaf, 2) + std::pow(endptY - endptYaf, 2));
			if (distance < DEF_DISTANCE  &&  OnMoveArea(endptXaf, endptYaf) == True) {
				if (objsum.cnt == 0) OnObjSum(&objsum, dist, angle, False);
				else                 OnObjSum(&objsum, dist, angle, True);
			}
			else {
				if (objsum.cnt < 2) {		//点の数が一定以下のとき，カウントしない
					for (int i = 0; i <= objsum.cnt; i++) { //前に戻って色変更
															//色塗り特殊
						float _endptX = sin((float)(ScanData[pos - i].angle*PI / 180.0))*(ScanData[pos - i].dist*distScale) + centerPt.x;
						float _endptY = centerPt.y - cos((float)(ScanData[pos - i].angle*PI / 180.0))*(ScanData[pos - i].dist*distScale);
						memDC.FillSolidRect((int)_endptX - 1, (int)_endptY - 1, 2, 2, RGB(0, 128, 128));
					}
					objsum.cnt = 0; //flag
				}
				else {
					OnObjSum(&objsum, dist, angle, True);
					OnObjCenterPush(&objsum, distScale, centerPt.x, centerPt.y);
				}
			}
		}
		else if (pos == (int)ScanData.size() - 1) {	//最後の時の処理
			distance = sqrtf(std::pow(endptX - endptX_0, 2) + std::pow(endptY - endptY_0, 2));
			if (objsum.cnt == 0) OnObjSum(&objsum, dist, angle, False);
			else                 OnObjSum(&objsum, dist, angle, True);
			OnObjCenterPush(&objsum, distScale, centerPt.x, centerPt.y);
			//最初と最後のつながり
			if (distance < DEF_DISTANCE) {
				scanDot_Simple tmpscan;
				tmpscan.dist = (_obj_center[0][0].dist + _obj_center[0].back().dist) / 2;
				tmpscan.angle = (_obj_center[0][0].angle + (_obj_center[0].back().angle + 360)) / 2;
				if (tmpscan.angle > 360) {
					tmpscan.angle -= 360;
				}
				tmpscan.X = sin((float)(tmpscan.angle*PI / 180.0))*(tmpscan.dist*distScale) + centerPt.x;
				tmpscan.Y = centerPt.y - cos((float)(tmpscan.angle*PI / 180.0))*(tmpscan.dist*distScale);
				tmpscan.cnt = 0;
				_obj_center[0].pop_back();  //2つをつなげて新しいの作る，古いの消す
				std::swap(_obj_center[0][0], _obj_center[0].back());
				_obj_center[0].pop_back();
				_obj_center[0].push_back(tmpscan);
			}
			else {
			}
		}
		//fprintf(fpw, "%f,%f,%f,%f,\n", angle, dist, endptX, endptY);
		//ここまで ↑
		if (dangle < min_picked_dangle) {
			min_picked_dangle = dangle;
			picked_point = pos;
		}

		//int brightness = (_scan_data[pos].quality<<1) + 128;
		//if (brightness>255) brightness=255;
		//単色でプロット
		//memDC.FillSolidRect((int)endptX-1,(int)endptY-1, 2, 2,RGB(0,brightness,brightness));
		//memDC.FillSolidRect((int)endptX-1,(int)endptY-1, 2, 2,RGB(0,255,255));
	}
}
