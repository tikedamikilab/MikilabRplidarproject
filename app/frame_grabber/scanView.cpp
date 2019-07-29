/*
 *  RPLIDAR
 *  Win32 Demo Application
 *
 *  Copyright (c) 2009 - 2014 RoboPeak Team
 *  http://www.robopeak.com
 *  Copyright (c) 2014 - 2016 Shanghai Slamtec Co., Ltd.
 *  http://www.slamtec.com
 *
 */
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//
////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "resource.h"
#include "scanView.h"
#include <math.h>
#include <stdio.h>
#include <vector> 
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include <mutex>
using namespace std;
#pragma once

const int DEF_MARGIN = 20;
const int DISP_RING_ABS_DIST  = 100;
const float DISP_FULL_DIST    = 16000;
const float DISP_DEFAULT_DIST = 8000;
const float DISP_MIN_DIST     = 1000;
const float PI   = (float)3.14159265;
//const int DEF_DISTANCE = 200; //10000*sin(1*PI()/180)
const int DEF_DISTANCE = 300;
const COLORREF dot_full_brightness = RGB(44, 233, 22);

std::mutex mtx_; // 排他制御用ミューテックス
std::vector<scanDot_Complication> _globle_obj_another;
std::vector<scanDot_Complication> _globle_obj_another_calc_old;

bool operator<(const scanDot& left, const scanDot& right)
{
	return left.angle < right.angle;
}

bool operator>(const scanDot& left, const scanDot& right)
{
	return left.angle > right.angle;
}
bool operator<(const scanDot_Complication& left, const scanDot_Complication& right)
{
	return left.cnt < right.cnt;
}
bool operator>(const scanDot_Complication& left, const scanDot_Complication& right)
{
	return left.cnt > right.cnt;
}
CScanView::CScanView()
{
    bigfont.CreateFontA(32/*-MulDiv(8, GetDeviceCaps(desktopdc, LOGPIXELSY), 72)*/,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH ,"Verdana");
    stdfont.CreateFontA(14/*-MulDiv(8, GetDeviceCaps(desktopdc, LOGPIXELSY), 72)*/,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH ,"Verdana");

    _current_display_range = DISP_DEFAULT_DIST;

    _mouse_angle = 0;
    _mouse_pt.x= _mouse_pt.y = 0;

	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
}

BOOL CScanView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

void CScanView::DoPaint(CDCHandle dc)
{

}

void CScanView::onDrawSelf(CDCHandle dc)
{
	CDC memDC;
	CBitmap memBitmap;
	memDC.CreateCompatibleDC(dc);
	CRect clientRECT;
	this->GetClientRect(&clientRECT);

	memBitmap.CreateCompatibleBitmap(dc, clientRECT.Width(), clientRECT.Height());
	HBITMAP oldBitmap = memDC.SelectBitmap(memBitmap);

	HPEN oldPen = memDC.SelectStockPen(DC_PEN);
	HBRUSH oldBrush = memDC.SelectStockBrush(NULL_BRUSH);
	HFONT  oldFont = memDC.SelectFont(stdfont);

	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextColor(RGB(90, 90, 90));
	memDC.SetTextAlign(TA_CENTER | TA_BASELINE);

	memDC.SetDCPenColor(RGB(60, 60, 60));

	CPoint centerPt(clientRECT.Width() / 2, clientRECT.Height() / 2);
	
	const int maxPixelR = min(clientRECT.Width(), clientRECT.Height()) / 2 - DEF_MARGIN;
	const float distScale = (float)maxPixelR / _current_display_range;

	/*-------------------*/
	//複数台追跡
	/*-------------------*/
	ATLASSERT(_CrtCheckMemory());
	ObjAnotherCalc m_objanothercalc;
	std::vector<scanDot_Complication> _obj_another_calc;
	OnObjAnotherCopy(_obj_another_calc);
	std::thread th_anothercalc([&m_objanothercalc, &_obj_another_calc, &centerPt, &distScale]() {m_objanothercalc.ObjAnotherAll(_obj_another_calc, _globle_obj_another_calc_old, distScale, centerPt); });
	int size_globle_obj_another = _globle_obj_another.size();
	OnObjAnotherClear();  //ここでclearしてる
	ATLASSERT(_CrtCheckMemory());

	setCenterPt(centerPt);
	setDistscal(distScale);
	/********************/
	//todo:コメントアウト消す
	//OnRingPlot(memDC, centerPt, maxPixelR, distScale);
	//memDC.FillSolidRect(0, 0, clientRECT.Width()-1, clientRECT.Height()-1, RGB(0, 0, 139));
	if (_position == 50) {
		OnbigsiteBackground(memDC, position_bigsite_X + view_X, position_bigsite_Y + view_Y, clientRECT);
	}

	//for (int i = 0; i < (int)_obj_background.size(); i++) {
	//	memDC.FillSolidRect(_obj_background[i].X - 1, _obj_background[i].Y - 1, 2, 2, RGB(128, 128, 128));
	//}

	char txtBuffer[100];
	int picked_point = 0;
	float min_picked_dangle = 100;

	//plot _scan_data
	//追加↓
	scanDot_Simple objsum;
	objsum.angle = 0; objsum.dist = 0; objsum.cnt = 0;
	float endptX_bf = 0;
	float endptY_bf = 0;
	float distance;

	//sort
	std::sort(_scan_data[0].begin(), _scan_data[0].end());
	//noise delete
	/*
	for (int pos = 1; pos < (int)_scan_data[0].size() - 1; ++pos) {
		float dist_now = _scan_data[0][pos].dist;
		float dist_bf = _scan_data[0][pos-1].dist;
		float dist_af = _scan_data[0][pos+1].dist;
		if (abs(dist_now - dist_bf) > 300 && abs(dist_bf - dist_af) < 600) {
			_scan_data[0][pos].dist = (dist_bf + dist_af) / 2;
		}
	}
	*/
	//file define 部分
	/*/
	char file_name[32] = "save.csv";
	FILE *fpw;
	fpw = fopen(file_name, "w");
	fprintf(fpw, "%d,%d,\n", (int)centerPt.x, (int)centerPt.y);
	fclose(fpw);
	*/
	//float dist = _scan_data[0][0].dist;
	//float angle = _scan_data[0][0].angle;
	/*
	char filename[50];
	int ofstreamtesttmp = 1;
	sprintf(filename, "./LogData/ofstream_%d.csv", ofstreamtesttmp);
	std::ofstream log_file(filename);
	*/
	//log_file.close();
	
	//ここまで↑
	for (int pos = 0; pos < (int)_scan_data[0].size(); ++pos) {
		float dist = _scan_data[0][pos].dist;
		float angle = _scan_data[0][pos].angle;
		//float distPixel = dist * distScale;
		float rad = (float)(angle*PI / 180.0);
		float distX = sin(rad)*(dist);
		float distY = cos(rad)*(dist);
		float endptX = distX*distScale + centerPt.x;
		float endptY = centerPt.y - distY * distScale;
		float dangle = fabs(rad - _mouse_angle);
		float distXaf;
		float distYaf;
		float endptXaf;
		float endptYaf;
		//file
		//log_file << dist << "," << angle << "," << distScale << "," << rad << "," << distX << "," << distY << "," 
		//	<< centerPt.x << "," << centerPt.y << "," << endptX << "," << endptY << ",\n";
		//float _endptY = centerPt.y - cos((float)(_scan_data[pos].angle*PI/180.0))*(_scan_data[pos].dist*distScale);
		//float _endptX = sin((float)(_scan_data[pos].angle*PI/180.0))*(_scan_data[pos].dist*distScale) + centerPt.x;
		//追加↓ 塊に分ける
		bool move_area = m_movearea.checkMoveArea(distX, distY, _position);
		if (move_area == false) {  //データが条件に合うかの確認
			//memDC.FillSolidRect((int)endptX - 1, (int)endptY - 1, 2, 2, RGB(128, 128, 128));
			memDC.FillSolidRect((int)endptX + view_X * getDistscal(), (int)endptY + view_Y * getDistscal(), 3, 3, RGB(255, 255, 255));
			if (dangle < min_picked_dangle) {
				min_picked_dangle = dangle;
				picked_point = pos;
			}
			continue;  //ループ処理を飛ばす
		}
		//memDC.FillSolidRect((int)endptX - 1, (int)endptY - 1, 2, 2, RGB(150, 150, 50));
		memDC.FillSolidRect((int)endptX + view_X * getDistscal(), (int)endptY + view_Y * getDistscal(), 3, 3, RGB(255, 255, 255));
		if (pos != (int)_scan_data[0].size() - 1) {
			//一個先との差
			distXaf = sin((float)(_scan_data[0][pos + 1].angle*PI / 180.0))*_scan_data[0][pos + 1].dist;
			distYaf = cos((float)(_scan_data[0][pos + 1].angle*PI / 180.0))*_scan_data[0][pos + 1].dist;
			endptXaf = distXaf * distScale + centerPt.x;
			endptYaf = centerPt.y - distYaf * distScale;
			distance = sqrtf(std::pow(distX - distXaf, 2) + std::pow(distY - distYaf, 2));
			if (distance < DEF_DISTANCE  &&  m_movearea.checkMoveArea(distXaf, distYaf, _position) == true) {
				if (objsum.cnt == 0) OnObjSum(&objsum, dist, angle, false);
				else                 OnObjSum(&objsum, dist, angle, true);
			}
			else {
				if (objsum.cnt < MINIMUM_ELE_NUM && pos >= MINIMUM_ELE_NUM) {		//点の数が一定以下のとき，カウントしない
					for (int i = 0; i <= objsum.cnt; i++) { //前に戻って色変更
						//色塗り特殊
						float _endptX = sin((float)(_scan_data[0][pos - i].angle*PI / 180.0))*(_scan_data[0][pos - i].dist*distScale) + centerPt.x;
						float _endptY = centerPt.y - cos((float)(_scan_data[0][pos - i].angle*PI / 180.0))*(_scan_data[0][pos - i].dist*distScale);
						memDC.FillSolidRect((int)_endptX + view_X * getDistscal(), (int)_endptY + view_Y * getDistscal(), 2, 2, RGB(255, 255, 255));
					}
					objsum.cnt = 0; //flag
				}
				else {
					OnObjSum(&objsum, dist, angle, true);
					OnObjCenterPush(&objsum, distScale, centerPt.x, centerPt.y);
				}
			}
		}
		else if (pos == (int)_scan_data[0].size() - 1 && _obj_center[0].size() > 0) {	//最後の時の処理
			if (objsum.cnt == 0) OnObjSum(&objsum, dist, angle, false);
			else                 OnObjSum(&objsum, dist, angle, true);
			OnObjCenterPush(&objsum, distScale, centerPt.x, centerPt.y);
			//float distance_center = sqrtf(std::pow(_obj_center[0][0].X - _obj_center[0].back().X, 2) + std::pow(_obj_center[0][0].Y - _obj_center[0].back().Y, 2));
			float radend = (float)(_obj_center[0].back().angle*PI / 180.0);
			float distXend = sin(radend)*(_obj_center[0].back().dist);
			float distYend = cos(radend)*(_obj_center[0].back().dist);
			float radzero = (float)(_obj_center[0][0].angle*PI / 180.0);
			float distXzero = sin(radzero)*(_obj_center[0][0].dist);
			float distYzero = cos(radzero)*(_obj_center[0][0].dist);
			float distance_center = sqrtf(std::pow(distXend - distXzero, 2) + std::pow(distYend - distYzero, 2));
			//最初と最後のつながり
			if (distance_center < 900) { //5000*sin(10*PI()/180)
				scanDot_Simple tmpscan;
				tmpscan.dist = (_obj_center[0][0].dist + _obj_center[0].back().dist) / 2;
				tmpscan.angle = (_obj_center[0][0].angle + (_obj_center[0].back().angle + 360)) / 2;
				if (tmpscan.angle > 360) {
					tmpscan.angle -= 360;
				}
				tmpscan.X = sin((float)(tmpscan.angle*PI / 180.0))*(tmpscan.dist*distScale) + centerPt.x;
				tmpscan.Y = centerPt.y - cos((float)(tmpscan.angle*PI / 180.0))*(tmpscan.dist*distScale);
				tmpscan.cnt = 0;
				try {
					_obj_center[0].pop_back();  //2つをつなげて新しいの作る，古いの消す
					_obj_center[0].push_back(tmpscan);
				}
				catch (std::exception &e) {
					std::cout<<"*********1*********";
					std::cout << e.what() << std::endl;
				}
				std::swap(_obj_center[0][0], _obj_center[0].back());
				try {
					_obj_center[0].pop_back();
				}
				catch (std::exception &e) {
					std::cout << "*********2*********";
					std::cout << e.what() << std::endl;
				}
			}
			else {
			}
		}
		//ここまで ↑
		if (dangle < min_picked_dangle) {
			min_picked_dangle = dangle;
			picked_point = pos;
		}
	}
	//ファイルクローズ
	//log_file.close();

	memDC.SelectFont(bigfont);

	memDC.SetTextAlign(TA_LEFT | TA_BASELINE);
	memDC.SetTextColor(RGB(255, 255, 255));

	//sprintf_s(txtBuffer, 100, "%.1f Hz (%d RPM)", _scan_speed, (int)(_scan_speed * 60));
	//memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 40, txtBuffer);
	
	//ここから↓
	//追跡
	std::vector<int> buffmove;
	std::vector<int> buffmovekill;
	std::vector<BuffInt> buffsemi;
	int pop_back_cnt = 0;
	for (int j = (int)_obj_move[0].size()-1; j >= 0; --j) { //条件に合った_obj_moveをkillする
		try {
			if (_obj_move[0].at(j).cnt < -50) {  //静止物体キル
				std::swap(_obj_move[0][j], _obj_move[0].back());
				_obj_move[0].pop_back();
			}
		}
		catch (std::exception &e) {
			std::cout << "*********_move_kill*********";
			std::cout << e.what() << std::endl;
		}
	}

	for (int j = 0; j < _obj_move[0].size() ; ++j) {  //基本centerとmoveを対応づけ
		float distance;
		float best_distance = 300;  //テキトー
		int best_center_ele = -1;
		float radmove = (float)(_obj_move[0][j].angle*PI / 180.0);
		float distXmove = sin(radmove)*(_obj_move[0][j].dist);
		float distYmove = cos(radmove)*(_obj_move[0][j].dist);
		//std::time_t t = std::time(0);   // get time now
		//std::tm* now = std::localtime(&t);
		_obj_move[0][j].now = std::time(0); //nowの更新
		SYSTEMTIME st;
		GetLocalTime(&st);
		_obj_move[0][j].milliseconds = st.wMilliseconds;
		for (int i = 0; i < (int)_obj_center[0].size(); i++) {
			if (_obj_center[0][i].cnt != 1) { //cntは使ったかどうかのflag
				//簡易追跡
				float radcenter = (float)(_obj_center[0][i].angle*PI / 180.0);
				float distXcenter = sin(radcenter)*(_obj_center[0][i].dist);
				float distYcenter = cos(radcenter)*(_obj_center[0][i].dist);
				distance = sqrtf(std::pow(distXmove - distXcenter, 2)+ std::pow(distYmove - distYcenter, 2));
				//if (_obj_move[0][j].cnt < -10)  distance = distance + 3;  //動いてないのは動かない
				//else if (_obj_move[0][j].cnt < -30) distance = distance + 6;
				if (distance < best_distance) {  //moveから一番近いcenter
					best_distance = distance;
					best_center_ele = i;
				}
			}
		}
		if (best_center_ele > -1) {  //一番近いのに_obj_moveを更新
			OnCopy(_obj_move, best_center_ele, j);
			_obj_center[0][best_center_ele].cnt = 1;  //cntは使ったかどうかのflag
			OnObjCntUpdata(_obj_move, j, true);  //_obj_move[0][j].cnt++;  と同じcntは連続データ更新の回数　
		}
		else {
			OnObjCntUpdata(_obj_move, j, false);
			buffmove.push_back(j);  //_obj_move使ってないリスト
		}
	}
	for (int j = 0; j < (int)_obj_semi_move[0].size(); j++) {  //_obj_semi_moveと_obj_centerを対応づけ
		float distance;
		float best_distance = 300;  //テキトー
		int best_center_ele = -1;
		float radsemi = (float)(_obj_semi_move[0][j].angle*PI / 180.0);
		float distXsemi = sin(radsemi)*(_obj_semi_move[0][j].dist);
		float distYsemi = cos(radsemi)*(_obj_semi_move[0][j].dist);
		for (int i = 0; i < (int)_obj_center[0].size(); i++) {
			if (_obj_center[0][i].cnt != 1) {  //_obj_centerの使ってないやつ　0:未使用　1:使用済み
				float radcenter = (float)(_obj_center[0][i].angle*PI / 180.0);
				float distXcenter = sin(radcenter)*(_obj_center[0][i].dist);
				float distYcenter = cos(radcenter)*(_obj_center[0][i].dist);
				distance = sqrtf(std::pow(distXsemi-distXcenter, 2)+ std::pow(distYsemi - distYcenter, 2));
				if (distance < best_distance) {  //_obj_centerと_obj_semi_moveの一番近いの
					best_distance = distance;
					best_center_ele = i;
				}
			}
		}
		if (best_center_ele > -1) {
			OnCopy(_obj_semi_move, best_center_ele, j);  //一番近いのに_obj_semi_moveを更新
			_obj_center[0][best_center_ele].cnt = 1;  //cntは使ったかどうかのflag
			_obj_semi_move[0][j].cnt++;    //cntは連続データ更新の回数
			if (_obj_semi_move[0][j].cnt > 1) {  //一定回連続で更新されたとき
				BuffInt tmp;
				tmp.i = best_center_ele;
				tmp.j = j;
				buffsemi.push_back(tmp);  //_obj_move生成用に_obj_semi_moveのデータを保持
			}
		}
		else {
			_obj_semi_move[0][j].cnt = 1000;  //削除処理用
			pop_back_cnt++;  //削除する個数
		}
	}
	for (int j = 0; j < (int)buffsemi.size(); j++) {  //未使用の_obj_moveと条件を満たした_obj_semi_moveを対応づけ，出来なければ_obj_moveを新規生成
		float distance;
		float best_distance = 2000;  //テキトー
		int best_ele = -1;
		float radbuffsemi = (float)(_obj_semi_move[0][buffsemi[j].j].angle*PI / 180.0);
		float distXbuffsemi = sin(radbuffsemi)*(_obj_semi_move[0][buffsemi[j].j].dist);
		float distYbuffsemi = cos(radbuffsemi)*(_obj_semi_move[0][buffsemi[j].j].dist);
		for (int i = 0; i < (int)buffmove.size(); i++) {  //未使用の_obj_moveと条件を満たした_obj_semi_moveを対応づけ
			if (_obj_move[0][buffmove[i]].cnt > -30) {
				float radmove = (float)(_obj_move[0][buffmove[i]].angle*PI / 180.0);
				float distXmove = sin(radmove)*(_obj_move[0][buffmove[i]].dist);
				float distYmove = cos(radmove)*(_obj_move[0][buffmove[i]].dist);
				distance = sqrtf(std::pow(distXbuffsemi - distXmove, 2)+ std::pow(distYbuffsemi - distYmove, 2));
				if (distance < best_distance) {
					best_distance = distance;
					best_ele = i;
				}
			}
		}
		if (best_ele > -1) {  //_obj_moveを_obj_semi_moveを対応づけ
			OnCopy(_obj_move, buffsemi[j].i, buffmove[best_ele]);
		}
		else {  //_obj_semi_moveの位置に_obj_moveを生成
			OnMoveComplicationMake(buffsemi[j].i, _obj_move);
			_obj_move[0].back().uniqueid = _UniqueId;
			_UniqueId++;  //uniqueIdを入れる
		}
		_obj_semi_move[0][buffsemi[j].j].cnt = 1000;  //削除処理用
		pop_back_cnt++;  //削除する個数
	}
	std::sort(_obj_semi_move[0].begin(), _obj_semi_move[0].end());
	for (int i = pop_back_cnt; i > 0; --i) {
		try {
			if (_obj_semi_move[0].at(_obj_semi_move[0].size() - 1).cnt >= 1000) {
				_obj_semi_move[0].at(_obj_semi_move[0].size() - 1);
				_obj_semi_move[0].pop_back();
			}
		}
		catch (std::exception &e) {
			std::cout << "*********4*********";
			std::cout << e.what() << std::endl;
		}
	}
	for (int i = 0; i < (int)_obj_center[0].size(); i++) {
		if (_obj_center[0][i].cnt != 1) {
			OnMoveComplicationMake(i, _obj_semi_move);
		}
	}
	/*-----------------------*/
	//追跡処理ここまで
	/*-----------------------*/
	th_anothercalc.join();
	//
	/************************/
	//
	std::wcout << "recieve:" << size_globle_obj_another << "->" << _obj_another_calc.size() << std::endl;
	//
	/************************/
	ATLASSERT(_CrtCheckMemory());
	for (int i = 0; i < _obj_another_calc.size(); i++) {
		_obj_another_calc_log.push_back(_obj_another_calc.at(i));
	}
	ATLASSERT(_CrtCheckMemory());
	//背景　プロット
	if (_position == 0) {
		OnPlotBackground(memDC, 0, 0);
	}
	else if (_position == 1) {
		OnPlotBackground(memDC, position_one_X, position_one_Y);
	}
	else if (_position == 2) {
		OnPlotBackground(memDC, position_two_X, position_two_Y);
	}
	else if (_position == 3) {
		OnPlotBackground(memDC, position_three_X, position_three_Y);
	}else if(_position == 50) {
		//drow OnbigsiteBackground()
	}
	else if (_position == 99) {
	}
	else {
		OnPlotBackground(memDC, 0, 0);
	}
	//点　プロット
	if (_ViewMode == VIEW_MODE_SCAN) {
		/*-------------------*/
		//他のLiDARの計測値
		/*-------------------*/
		for (int i=0; i<_obj_another_calc.size(); i++) {
			//std::wcout << i << std::endl;
			//OnColorPolt(memDC, _obj_another_calc.at(i).uniqueid, (int)_obj_another_calc.at(i).X, (int)_obj_another_calc.at(i).Y, 6);
		}
		/*
		for (int j = 0; j < (int)_obj_center[0].size(); j++) {  //全物体中心
			memDC.FillSolidRect((int)_obj_center[0][j].X - 1, (int)_obj_center[0][j].Y - 1, 2, 2, RGB(255, 255, 0));
		}
		for (int j = 0; j < (int)_obj_semi_move[0].size(); j++) {  //semi移動物体中心
			memDC.FillSolidRect((int)_obj_semi_move[0][j].X - 1, (int)_obj_semi_move[0][j].Y - 1, 2, 2, RGB(255, 255, 255));
		}
		*/
		for (int i = 0; i < OBJ_MOVE_SIZE; i++) {
			for (int j = 0; j < (int)_obj_move[i].size(); j++) {  //Line表示
				OnLinePlot(_obj_move[i][j], memDC);
				//OnColorPolt(memDC, _obj_move[i][j].uniqueid, (int)_obj_move[i][j].X + view_X*getDistscal(), (int)_obj_move[i][j].Y + view_Y*getDistscal(), 4);  //過去の点
			}
		}
		/*
		int cntcalclog = 0;
		for (int i = _obj_another_calc_log.size() -1; i >= 0 ; i--) {  //Line表示
			cntcalclog++;
			if (cntcalclog > 100) {
				break;
			}
			OnLinePlot(_obj_another_calc_log[i], memDC);
		}
		*/
		for (int j = 0; j < (int)_obj_move[0].size(); j++) {  //移動物体中心
			OnColorPolt(memDC, _obj_move[0][j].uniqueid, (int)_obj_move[0][j].X + view_X*getDistscal(), (int)_obj_move[0][j].Y + view_Y*getDistscal(), 13);
			memDC.SelectFont(bigfont);  //文字書く
			memDC.SetTextAlign(TA_LEFT | TA_BASELINE);
			memDC.SetTextColor(RGB(255, 255, 255));
			std::tm* now = std::localtime(&_obj_move[0][j].now);
			//sprintf_s(txtBuffer,100, "%d,%d,%.1f,%d,%d,%d,%d", now->tm_sec, _obj_move[0][j].uniqueid, _obj_move[0][j].angle, (int)_obj_move[0][j].dist, (int)_obj_move[0][j].X, (int)_obj_move[0][j].Y, _obj_move[0][j].cnt);
			//memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 60 + (20 * j), txtBuffer);
		}
	}
	else if (_ViewMode == VIEW_MODE_LOG) {
		OnViewLogData(memDC);
	}
	else if (_ViewMode == VIEW_MODE_FILE) {
		OnViewFileData(memDC);
		OnLogText(memDC);
	}
	else if (_ViewMode == VIEW_MODE_SPLIT) {
		m_logdatasplit.DataSplitBasic(setstarttime, setendtime, _obj_read_data, magnification, colormodedefine);
		setSwitchViewModeSplitView();
	}
	else if (_ViewMode == VIEW_MODE_SPLITVIEW) {
		_obj_split_data = m_logdatasplit.getReSplitData();  //ここで帰ってくるのは1次元配列
		int Height = clientRECT.Height();
		int Width = clientRECT.Width();
		if (colormodedefine == 0) {
			for (int y = 0; y < Height; y++) {  //ディスプレイサイズ分だけループ
				for (int x = 0; x < Width; x++) {
					int tmpi = y * SPLIT_VIEW_X + x;
					if (_obj_split_data.at(tmpi).RGBsum > 0) {
						OnPlotRectHeatmap(memDC, tmpi, x, y);
						/******************************************/
						//todo:コメントアウト消す
						for (int i = 0; i < _obj_split_data.at(tmpi).previousX.size(); i++) {
							OnSetColorHeatmap(memDC, tmpi);
							memDC.SelectStockPen(DC_PEN);
							memDC.MoveTo(_obj_split_data[tmpi].previousX[i], _obj_split_data[tmpi].previousY[i]);
							memDC.LineTo(x, y);
						}
						/******************************************/
					}
				}
			}
		}
		else if(colormodedefine == 1){  //(colormodedefine != 0)
			for (int y = 0; y < Height; y++) {  //ディスプレイサイズ分だけループ
				for (int x = 0; x < Width; x++) {
					int tmpi = y * SPLIT_VIEW_X + x;
					if (_obj_split_data.at(tmpi).RGBsum > 0) {
						OnPlotRectHeatmap(memDC, tmpi, x, y);
					}
				}
			}
		}
		else if (colormodedefine == 2) {
			for (int y = 0; y < Height; y++) {  //ディスプレイサイズ分だけループ
				for (int x = 0; x < Width; x++) {
					int tmpi = y * SPLIT_VIEW_X + x;
					if (_obj_split_data.at(tmpi).RGBsum > 0) {
						for (int i = 0; i < _obj_split_data.at(tmpi).previousX.size(); i++) {
							OnSetColorHeatmap(memDC, tmpi);
							memDC.SelectStockPen(DC_PEN);
							memDC.MoveTo(_obj_split_data[tmpi].previousX[i], _obj_split_data[tmpi].previousY[i]);
							memDC.LineTo(x, y);
						}
					}
				}
			}
		}
		else if (colormodedefine == 3) {  //(colormodedefine != 0)
			for (int y = 0; y < Height; y++) {  //ディスプレイサイズ分だけループ
				for (int x = 0; x < Width; x++) {
					int tmpi = y * SPLIT_VIEW_X + x;
					if (_obj_split_data.at(tmpi).RGBsum > 0) {
						OnPlotRectHeatmap(memDC, tmpi, x, y);
					}
				}
			}
		}
		else if (colormodedefine == 99) {  //(colormodedefine != 0)
			for (int y = 0; y < Height; y++) {  //ディスプレイサイズ分だけループ
				for (int x = 0; x < Width; x++) {
					int tmpi = y * SPLIT_VIEW_X + x;
					if (_obj_split_data.at(tmpi).RGBsum > 0) {
						if(_obj_split_data.at(tmpi).uniqeIDs[0] % 6 == 0) memDC.FillSolidRect(x, y, 1, 1, RGB(255, 0, 0));
						else if (_obj_split_data.at(tmpi).uniqeIDs[0] % 6 == 1) memDC.FillSolidRect(x, y, 1, 1, RGB(255, 128, 0));
						else if (_obj_split_data.at(tmpi).uniqeIDs[0] % 6 == 2) memDC.FillSolidRect(x, y, 1, 1, RGB(128, 255, 0));
						else if (_obj_split_data.at(tmpi).uniqeIDs[0] % 6 == 3) memDC.FillSolidRect(x, y, 1, 1, RGB( 0, 255, 0));
						else if (_obj_split_data.at(tmpi).uniqeIDs[0] % 6 == 4) memDC.FillSolidRect(x, y, 1, 1, RGB( 0, 255, 128));
						else if (_obj_split_data.at(tmpi).uniqeIDs[0] % 6 == 5) memDC.FillSolidRect(x, y, 1, 1, RGB( 0, 128, 255));
					}
				}
			}
		}
		/*********************/
		//todo:コメントアウト外す
		OnLogText(memDC);
	}
	else if (_ViewMode == VIEW_MODE_NONE) {
	}
	
	/*
	//ここまで↑
    //角度と距離表示するやつ
	if ((int)_scan_data[0].size() > picked_point) {
		//(x,y)に直す
        float distPixel = _scan_data[0][picked_point].dist*distScale;
        float rad = (float)(_scan_data[0][picked_point].angle*PI/180.0);
		float distX = sin(rad)*_scan_data[0][picked_point].dist;
		float distY = cos(rad)*_scan_data[0][picked_point].dist;
		float endptX = sin(rad)*(distPixel) + centerPt.x;
        float endptY = centerPt.y - cos(rad)*(distPixel);

		memDC.SetDCPenColor(RGB(129,10,16));
		memDC.SetDCPenColor(RGB(0,0,0));
        memDC.SelectStockPen(DC_PEN);
        memDC.MoveTo(centerPt.x,centerPt.y);
        memDC.LineTo((int)endptX,(int)endptY);
        memDC.SelectStockPen(NULL_PEN);

        memDC.FillSolidRect((int)endptX-1,(int)endptY-1, 2, 2,RGB(255,0,0));
		//memDC.FillSolidRect((int)endptX, (int)endptY, 2, 2, RGB(128, 0, 0));

		//sprintf(txtBuffer, "%.2f ,%.2f", endptX, endptY);
		//memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 80, txtBuffer);

		memDC.SetTextColor(RGB(255, 0, 0));
        sprintf_s(txtBuffer,100, "dist: %.2f Deg: %.2f, %.1f, %.1f", _scan_data[0][picked_point].dist,  _scan_data[0][picked_point].angle, distX, distY);
		memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 20, txtBuffer);
    }
	*/
	/*
    if (_is4kmode) {
        memDC.SetTextColor(RGB(255,255,255));
        memDC.SetDCBrushColor(RGB(255,0,0));
        memDC.Rectangle(clientRECT.Width() - 100, DEF_MARGIN + 25, clientRECT.Width() - 60, DEF_MARGIN + 30);
        memDC.TextOutA(clientRECT.Width() - 100, DEF_MARGIN + 20, "4K");
    }
	*/
    dc.BitBlt(0, 0, clientRECT.Width(), clientRECT.Height(), memDC, 0, 0, SRCCOPY);

    memDC.SelectFont(oldFont);
    memDC.SelectBrush(oldBrush);
    memDC.SelectPen(oldPen);
    memDC.SelectBitmap(oldBitmap);
}
void CScanView::OnCopy(std::vector<scanDot_Complication> *obj, int i, int j) { //物体中心のデータを移動物体にコピー，iは物体中心，jは移動物体
	obj[0][j].previousangle = obj[0][j].angle;
	obj[0][j].previousdist = obj[0][j].dist;
	obj[0][j].previousX = obj[0][j].X;
	obj[0][j].previousY = obj[0][j].Y;
	obj[0][j].angle = _obj_center[0][i].angle;
	obj[0][j].dist = _obj_center[0][i].dist;
	obj[0][j].X = _obj_center[0][i].X;
	obj[0][j].Y = _obj_center[0][i].Y;
}

void CScanView::OnMoveBase(scanDot_Simple *tmp_scan, int i) {
	tmp_scan->angle = _obj_center[0][i].angle;
	tmp_scan->dist = _obj_center[0][i].dist;
	tmp_scan->X = _obj_center[0][i].X;
	tmp_scan->Y = _obj_center[0][i].Y;
	tmp_scan->cnt = 0;
}
void CScanView::OnMoveComplicationBase(scanDot_Complication *tmp_scan, int i){
	tmp_scan->previousangle = _obj_center[0][i].angle;
	tmp_scan->previousdist = _obj_center[0][i].dist;
	tmp_scan->previousX = _obj_center[0][i].X;
	tmp_scan->previousY = _obj_center[0][i].Y;
	tmp_scan->angle = _obj_center[0][i].angle;
	tmp_scan->dist = _obj_center[0][i].dist;
	tmp_scan->X = _obj_center[0][i].X;
	tmp_scan->Y = _obj_center[0][i].Y;
	tmp_scan->cnt = 0;
	tmp_scan->uniqueid = 1000;
	tmp_scan->flag = 0;   
	tmp_scan->now = std::time(0); 
	SYSTEMTIME st;
	GetLocalTime(&st);
	tmp_scan->milliseconds = st.wMilliseconds;
}
inline void CScanView::OnMoveMake(int i, std::vector<scanDot_Simple> *obj) {  //移動物体を作る．iは物体中心の方
	scanDot_Simple tmp_scan;
	OnMoveBase(&tmp_scan, i);
	obj[0].push_back(tmp_scan);
}
inline void CScanView::OnMoveComplicationMake(int i, std::vector<scanDot_Complication> *obj) {
	scanDot_Complication tmp_scan;
	OnMoveComplicationBase(&tmp_scan, i);
	obj[0].push_back(tmp_scan);
}

void CScanView::OnObjCenterPush(scanDot_Simple *objsum , float distScale, long x, long y) {
	//距離と角度の合計もらって中心計算，_obj_centerにpushする
	scanDot_Simple tmp_scan;
	tmp_scan.dist = objsum->dist / float(objsum->cnt);
	tmp_scan.angle = objsum->angle / float(objsum->cnt);
	tmp_scan.X = sin((float)(tmp_scan.angle*PI / 180.0))*(tmp_scan.dist*distScale) + x;
	tmp_scan.Y = y - cos((float)(tmp_scan.angle*PI / 180.0))*(tmp_scan.dist*distScale);
	tmp_scan.cnt = 0;
	_obj_center[0].push_back(tmp_scan);
	objsum->cnt = 0;  //OnObjCenterPushのflagになる
}

void CScanView::OnObjSum(scanDot_Simple *objsum, float dist, float angle, bool IfTrueppElone) {
	//中心計算用のsum
	if (IfTrueppElone){
		objsum->dist += dist;
		objsum->angle += angle;
		objsum->cnt += 1;
	}
	else {
		objsum->dist = dist;
		objsum->angle = angle;
		objsum->cnt = 1;
	}
}

void CScanView::OnObjCntUpdata(std::vector<scanDot_Complication> *obj, int j, bool boo) {  //trueが+に変更，falseが-に変更
	if (obj[0][j].cnt >= 0) {
		if (boo == true) obj[0][j].cnt++;
		else obj[0][j].cnt = -1;
	}
	else {
		if (boo == true) obj[0][j].cnt = 1;
		else obj[0][j].cnt--;
	}
}

inline void CScanView::OnColorPolt(CDC &memDC,int color, int x, int y, int size) {
	//カラーでプロット
	if (color % 3 == 0) memDC.FillSolidRect(x - 1, y - 1, size, size, RGB(255, 0, 0));
	else if (color % 3 == 1) memDC.FillSolidRect(x - 1, y - 1, size, size, RGB(0, 255, 0));
	else memDC.FillSolidRect(x - 1, y - 1, size, size, RGB(120, 255, 255));
}
void CScanView::OnbigsiteBackground(CDC &memDC, int calibration_X, int calibration_Y, CRect clientRECT) {
	CPen backpen;
	//backpen.CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
	backpen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	memDC.SelectPen(backpen);
	//領域
	memDC.FillSolidRect(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(3000 - calibration_Y), 12000*getDistscal(), 6000*getDistscal(), RGB(255, 255, 255));
	//市松
	for (int i = 0; i < 138; i++) { // 床
		if (i % 2 == 0) {
			memDC.FillSolidRect(OndistXtoendPtX(-5500 + calibration_X + ((int)(i % 23) * 500)), OndistYtoendPtY(3000 - calibration_Y - (i / 23) * 500), 510 * getDistscal(), 510 * getDistscal(), RGB(245, 245, 245));
		}
		else {
			memDC.FillSolidRect(OndistXtoendPtX(-5500 + calibration_X + ((int)(i % 23) * 500)), OndistYtoendPtY(3000 - calibration_Y - (i / 23) * 500), 510 * getDistscal(), 510 * getDistscal(), RGB(211, 211, 211));
		}
	}
	//枠
	memDC.MoveTo(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(3000 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(6000 + calibration_X), OndistYtoendPtY(3000 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(6000 + calibration_X), OndistYtoendPtY(0 - calibration_Y));
	memDC.MoveTo(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(0 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(3000 - calibration_Y));
	//ストック
	memDC.FillSolidRect(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(3000 - calibration_Y), 600 * getDistscal(), 3000 * getDistscal(), RGB(100, 100, 100));
	memDC.MoveTo(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(3000 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-5400 + calibration_X), OndistYtoendPtY(3000 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-5400 + calibration_X), OndistYtoendPtY(0 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(0 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(3000 - calibration_Y));
	//左机
	memDC.FillSolidRect(OndistXtoendPtX(-4800 + calibration_X), OndistYtoendPtY(2100 - calibration_Y), 4000 * getDistscal(), 1200 * getDistscal(), RGB(255, 218, 185));
	memDC.MoveTo(OndistXtoendPtX(-4800 + calibration_X), OndistYtoendPtY(2100 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-800 + calibration_X), OndistYtoendPtY(2100 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-800 + calibration_X), OndistYtoendPtY(900 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4800 + calibration_X), OndistYtoendPtY(900 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4800 + calibration_X), OndistYtoendPtY(2100 - calibration_Y));
	//擬似窓
	memDC.FillSolidRect(OndistXtoendPtX(0 + calibration_X), OndistYtoendPtY(3000 - calibration_Y), 3000 * getDistscal(), 420 * getDistscal(), RGB(100, 100, 100));
	memDC.MoveTo(OndistXtoendPtX(0 + calibration_X), OndistYtoendPtY(3000 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3000 + calibration_X), OndistYtoendPtY(3000 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3000 + calibration_X), OndistYtoendPtY(2580 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(0 + calibration_X), OndistYtoendPtY(2580 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(0 + calibration_X), OndistYtoendPtY(3000 - calibration_Y));
	//降雨音声
	memDC.FillSolidRect(OndistXtoendPtX(3465 + calibration_X), OndistYtoendPtY(3000 - calibration_Y), 2000 * getDistscal(), 500 * getDistscal(), RGB(255, 218, 185));
	memDC.MoveTo(OndistXtoendPtX(3465 + calibration_X), OndistYtoendPtY(3000 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(5465 + calibration_X), OndistYtoendPtY(3000 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(5465 + calibration_X), OndistYtoendPtY(2500 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3465 + calibration_X), OndistYtoendPtY(2500 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3465 + calibration_X), OndistYtoendPtY(3000 - calibration_Y));
	//配席
	memDC.FillSolidRect(OndistXtoendPtX(0 + calibration_X), OndistYtoendPtY(900 - calibration_Y), 450 * getDistscal(), 900 * getDistscal(), RGB(255, 218, 185));
	memDC.MoveTo(OndistXtoendPtX(0 + calibration_X), OndistYtoendPtY(900 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(450 + calibration_X), OndistYtoendPtY(900 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(450 + calibration_X), OndistYtoendPtY(0 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(0 + calibration_X), OndistYtoendPtY(0 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(0 + calibration_X), OndistYtoendPtY(900 - calibration_Y));
	//人流
	memDC.FillSolidRect(OndistXtoendPtX(2500 + calibration_X), OndistYtoendPtY(450 - calibration_Y), 900 * getDistscal(), 450 * getDistscal(), RGB(255, 218, 185));
	memDC.MoveTo(OndistXtoendPtX(2500 + calibration_X), OndistYtoendPtY(450 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3400 + calibration_X), OndistYtoendPtY(450 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3400+ calibration_X), OndistYtoendPtY(0 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(2500 + calibration_X), OndistYtoendPtY(0 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(2500 + calibration_X), OndistYtoendPtY(450 - calibration_Y));
	//右机
	memDC.FillSolidRect(OndistXtoendPtX(4300 + calibration_X), OndistYtoendPtY(1800 - calibration_Y), 800 * getDistscal(), 1500 * getDistscal(), RGB(255, 218, 185));
	memDC.MoveTo(OndistXtoendPtX(4300 + calibration_X), OndistYtoendPtY(1800 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(5100+ calibration_X), OndistYtoendPtY(1800 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(5100 + calibration_X), OndistYtoendPtY(300 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(4300 + calibration_X), OndistYtoendPtY(300 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(4300 + calibration_X), OndistYtoendPtY(1800 - calibration_Y));


	memDC.FillSolidRect(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(25 - calibration_Y), 2600 * getDistscal(), 50 * getDistscal(), RGB(0, 0, 0));  //壁
	memDC.FillSolidRect(OndistXtoendPtX(-1700 + calibration_X), OndistYtoendPtY(25 - calibration_Y), 1700 * getDistscal(), 50 * getDistscal(), RGB(0, 0, 0));  //壁
	memDC.FillSolidRect(0, 0, 10, clientRECT.Height() +10, RGB(0, 0, 139)); //左枠
	memDC.FillSolidRect(clientRECT.Width() - 500, 0, 500, clientRECT.Height(), RGB(0, 0, 139)); //右枠
	memDC.FillSolidRect(0, 0, clientRECT.Width() + 10, 10, RGB(0, 0, 139)); //上枠
	memDC.FillSolidRect(0, clientRECT.Height() - 10, clientRECT.Width(), 30, RGB(0, 0, 139)); //下枠
}

void CScanView::OnPlotBackground(CDC &memDC, int calibration_X, int calibration_Y) {
	CPen backpen;
	backpen.CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
	//backpen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	memDC.SelectPen(backpen);
	//s外枠
	memDC.MoveTo(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.MoveTo(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-5220 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-5220 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2992 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2992 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2290 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2290 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3240 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3240 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(4180 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(4180 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(9250 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(9250 + calibration_X), OndistYtoendPtY(755 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(10000 + calibration_X), OndistYtoendPtY(755 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(10000 + calibration_X), OndistYtoendPtY(-983 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(9250 + calibration_X), OndistYtoendPtY(-983 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(9250 + calibration_X), OndistYtoendPtY(-5050 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4900 + calibration_X), OndistYtoendPtY(-5050 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4900 + calibration_X), OndistYtoendPtY(-4490 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4500 + calibration_X), OndistYtoendPtY(-4490 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4500 + calibration_X), OndistYtoendPtY(2740 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(2740 - calibration_Y));
	//集中
	memDC.MoveTo(OndistXtoendPtX(3420 + calibration_X), OndistYtoendPtY(-5050 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3420 + calibration_X), OndistYtoendPtY(-3330 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2400 + calibration_X), OndistYtoendPtY(-3330 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2400 + calibration_X), OndistYtoendPtY(-5050 - calibration_Y));
	//和室と多目的のパーティション
	memDC.MoveTo(OndistXtoendPtX(4000 + calibration_X), OndistYtoendPtY(755 - calibration_Y));
	memDC.MoveTo(OndistXtoendPtX(4000 + calibration_X), OndistYtoendPtY(755 - calibration_Y));
	memDC.MoveTo(OndistXtoendPtX(4000 + calibration_X), OndistYtoendPtY(755 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(7450 + calibration_X), OndistYtoendPtY(755 - calibration_Y));
	//机
	OnDeskStandardPlot(memDC, 0 + calibration_X, 0 - calibration_Y);
	OnDeskStandardPlot(memDC, -2465 + calibration_X, 1749 - calibration_Y);
	OnDeskStandardPlot(memDC, -2318 + calibration_X, -1933 - calibration_Y);
	OnDeskStandardPlot(memDC, 2428 + calibration_X, -2018 - calibration_Y);
	OnDeskStandardPlot(memDC, 2155 + calibration_X, 1950 - calibration_Y);
	//擬似窓前のキャビネット
	memDC.MoveTo(OndistXtoendPtX(-2290 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2290 + calibration_X), OndistYtoendPtY(3700 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3240 + calibration_X), OndistYtoendPtY(3700 - calibration_Y));
	memDC.MoveTo(OndistXtoendPtX(3240 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3240 + calibration_X), OndistYtoendPtY(3470 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3890 + calibration_X), OndistYtoendPtY(3470 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3890 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	//配席PC
	memDC.MoveTo(OndistXtoendPtX(-5220 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-5220 + calibration_X), OndistYtoendPtY(3750 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4090 + calibration_X), OndistYtoendPtY(3750 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4090 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	//多目的
	memDC.MoveTo(OndistXtoendPtX(4815 + calibration_X), OndistYtoendPtY(2883 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(4815 + calibration_X), OndistYtoendPtY(1483 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(7215 + calibration_X), OndistYtoendPtY(1483 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(7215 + calibration_X), OndistYtoendPtY(2883 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(4815 + calibration_X), OndistYtoendPtY(2883 - calibration_Y));
	//和室
	memDC.MoveTo(OndistXtoendPtX(9250 + calibration_X), OndistYtoendPtY(-1200 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(6506 + calibration_X), OndistYtoendPtY(-1200 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(6506 + calibration_X), OndistYtoendPtY(-2298 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(5769 + calibration_X), OndistYtoendPtY(-2298 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(5769 + calibration_X), OndistYtoendPtY(-5050 - calibration_Y));
}
inline void CScanView::OnDeskStandardPlot(CDC &memDC, int deskcenterX, int deskcenterY) {
	//机
	memDC.MoveTo(OndistXtoendPtX(1000 + deskcenterX), OndistYtoendPtY(130 + deskcenterY));
	memDC.LineTo(OndistXtoendPtX(-130 + deskcenterX), OndistYtoendPtY(1000 + deskcenterY));
	memDC.LineTo(OndistXtoendPtX(-1000 + deskcenterX), OndistYtoendPtY(-130 + deskcenterY));
	memDC.LineTo(OndistXtoendPtX(130 + deskcenterX), OndistYtoendPtY(-1000 + deskcenterY));
	memDC.LineTo(OndistXtoendPtX(1000 + deskcenterX), OndistYtoendPtY(130 + deskcenterY));
}
inline float CScanView::OndistXtoendPtX(float distX) {
	return distX * getDistscal() + getCenterPt().x;
}
inline float CScanView::OndistYtoendPtY(float distY) {
	return getCenterPt().y - distY * getDistscal();
}
void CScanView::OnRingPlot(CDC &memDC, CPoint &centerPt, int maxPixelR, float distScale) {
	// plot rings
	char txtBuffer[100];
	for (int angle = 0; angle<360; angle += 30) {
		float rad = (float)(angle*PI / 180.0);
		float endptX = sin(rad)*(maxPixelR + DEF_MARGIN / 2) + centerPt.x;
		float endptY = centerPt.y - cos(rad)*(maxPixelR + DEF_MARGIN / 2);

		memDC.MoveTo(centerPt);
		memDC.LineTo((int)endptX, (int)endptY);

		sprintf_s(txtBuffer,100, "%d", angle);
		memDC.TextOutA((int)endptX, (int)endptY, txtBuffer);

	}

	for (int plotR = maxPixelR; plotR>0; plotR -= DISP_RING_ABS_DIST)
	{
		memDC.Ellipse(centerPt.x - plotR, centerPt.y - plotR,
			centerPt.x + plotR, centerPt.y + plotR);

		sprintf_s(txtBuffer,100, "%.1f", (float)plotR / distScale);
		memDC.TextOutA(centerPt.x, centerPt.y - plotR, txtBuffer);
	}

	memDC.SelectStockBrush(DC_BRUSH);
	memDC.SelectStockPen(NULL_PEN);
}

void  CScanView::OnPictureView(CDC &memDC, CBitmap &memBitmap)
{
}

CPoint CScanView::OnTransformationPolortoRectangular(float angle, float dist, CPoint &centerPt, float distScale) {
	CPoint rePoint;
	rePoint.y = centerPt.y - cos((float)(angle*PI/180.0))*(dist*distScale);
	rePoint.x = sin((float)(angle*PI/180.0))*(dist*distScale) + centerPt.x;
	return rePoint;
}
CPoint CScanView::TransformationPolortoRectangular(float angle, float dist) {
	return OnTransformationPolortoRectangular(angle, dist, getCenterPt(), getDistscal());
}

void CScanView::TransformationPolortoRectangular(scanDot_Complication &tmp_scan) {
	CPoint tmp_point = TransformationPolortoRectangular(tmp_scan.angle, tmp_scan.dist);
	tmp_scan.X = tmp_point.x;
	tmp_scan.Y = tmp_point.y;
	tmp_point = TransformationPolortoRectangular(tmp_scan.previousangle, tmp_scan.previousdist);
	tmp_scan.previousX = tmp_point.x;
	tmp_scan.previousY = tmp_point.y;
}

BOOL CScanView::OnEraseBkgnd(CDCHandle dc)
{

    return 0;
}

void CScanView::OnMouseMove(UINT nFlags, CPoint point)
{
    _mouse_pt = point;

    CRect clientRECT;
    this->GetClientRect(&clientRECT);

    int dy = -(point.y - ((clientRECT.bottom-clientRECT.top)/2));
    int dx = point.x - ((clientRECT.right-clientRECT.left)/2);

    if (dx >=0 ) {
        _mouse_angle = atan2((float)dx, (float)dy);
    } else {
        _mouse_angle = PI*2 - atan2((float)-dx, (float)dy);
    }

    this->Invalidate();
}

int CScanView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    _last_update_ts = 0;
    _scan_speed = 0;
    _sample_counter = 0;
    _is4kmode = false;
    return 0;//CScrollWindowImpl<CPeakgrabberView>::OnCreate(lpCreateStruct);
}
void CScanView::OnPaint(CDCHandle dc)
{
    if (dc) 
    {
        onDrawSelf(dc);
    }
    else
    {
        CPaintDC tDC(m_hWnd);
        onDrawSelf(tDC.m_hDC);
    }
}

BOOL CScanView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    _current_display_range+=zDelta;
    if (_current_display_range > DISP_FULL_DIST) _current_display_range= DISP_FULL_DIST;
    else if (_current_display_range < DISP_MIN_DIST) _current_display_range = DISP_MIN_DIST;
    this->Invalidate();
	return 0;
}

void CScanView::setScanData(rplidar_response_measurement_node_t *buffer, size_t count, float frequency, bool is4kmode)
{
	//_obj_move_log
	std::vector<scanDot_Complication> _obj_send;
	for (int i = 0; i < (int)_obj_move[0].size(); ++i) {
		_obj_send.push_back(_obj_move[0][i]);  //sendデータ作成
		//_positionに応じた処理
		if (_position == 1) {
			OnPositionGloble(position_one_X, position_one_Y, _obj_send);
		}
		else if (_position == 2) {
			OnPositionGloble(position_two_X, position_two_Y, _obj_send);
		}else if(_position == 3) {
			OnPositionGloble(position_three_X, position_three_Y, _obj_send);
		}
		_obj_move_log.push_back(_obj_send[i]);  //logとして記録 
	}
	//_scan_dataの処理
	for (int i = SCAN_DATA_SIZE - 1; i > 0; --i) {  
		_scan_data[i].swap(_scan_data[i - 1]);
	}
    _scan_data[0].clear();
	_obj_center[0].clear();
	//_obj_moveの処理
	for (int i = OBJ_MOVE_SIZE - 1; i > 1; --i) { //1～9まで
		_obj_move[i].swap(_obj_move[i - 1]);
	}
	_obj_move[1].clear(); //あってる
	for (int i = 0; i < (int)_obj_move[0].size(); ++i) {
		_obj_move[1].push_back(_obj_move[0][i]);
	}
	/*-----------------------*/
	//複数台のとき送る．
	/*-----------------------*/
	int sendposition = _position;
	m_moveobjsend.UDPSendCheck(_obj_send, sendposition);
	//std::thread th_send([&m_moveobjsend, _obj_send, sendposition]() {m_moveobjsend.UDPSendCheck(_obj_send, sendposition); });

	if (_obj_move_log.empty() == false && _obj_move_log.back().now - logstarttime >= TERM_LOG_DATA_DUMP) { //ここでlogデータを記録
		std::tm* logstart = std::localtime(&_obj_move_log.front().now);
		char fname[100];
		//sprintf_s(fname, 100, "./LogData/MOVE_%d_%d_%d_%d_%d.csv", logstart->tm_mon + 1, logstart->tm_mday, logstart->tm_hour, logstart->tm_min, logstart->tm_sec);
		sprintf_s(fname, 100, "./LogData/MOVE_%d_%d.csv", logstart->tm_mon + 1, logstart->tm_mday);
		OnLogDataDump(_obj_move_log, fname);
		//sprintf_s(fname, 100, "./LogData/ANOTHER_%d_%d_%d_%d_%d.csv", logstart->tm_mon + 1, logstart->tm_mday, logstart->tm_hour, logstart->tm_min, logstart->tm_sec);
		sprintf_s(fname, 100, "./LogData/ANOTHER_%d_%d.csv", logstart->tm_mon + 1, logstart->tm_mday);
		OnLogDataDump(_obj_another_calc_log, fname);
		logstarttime = _obj_move_log.back().now;
		_obj_move_log.clear();
		_obj_another_calc_log.clear();
	}
    for (int pos = 0; pos < (int)count; ++pos) {
        scanDot dot;
        if (!buffer[pos].distance_q2) continue;

        dot.quality = (buffer[pos].sync_quality>>RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT);
        dot.angle = (buffer[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT)/64.0f;
        dot.dist = buffer[pos].distance_q2/4.0f;
        _scan_data[0].push_back(dot);
    }

    _is4kmode = is4kmode;
    _scan_speed = frequency;
	/*---------------*/
	//th_send.join();
	/*---------------*/

    this->Invalidate();
}
void CScanView::OnViewLogData(CDC &memDC) {
	for (int i = 0; i < (int)_obj_move_log.size(); i++) {
		OnColorPolt(memDC, _obj_move_log[i].uniqueid, _obj_move_log[i].X, _obj_move_log[i].Y, 2);
	}
}
void CScanView::ClearLogData() {
	_obj_move_log.clear();
}
void CScanView::OnPositionGloble(int positionX, int positionY, std::vector<scanDot_Complication> &_obj) {
	float rad = (float)(_obj.back().angle * PI / 180.0);
	float distX = sin(rad)*(_obj.back().dist) - positionX;
	float distY = cos(rad)*(_obj.back().dist) + positionY;
	float previousrad = (float)(_obj.back().previousangle * PI / 180.0);
	float previousdistX = sin(previousrad)*(_obj.back().previousdist) - positionX;
	float previousdistY = cos(previousrad)*(_obj.back().previousdist) + positionY;
	if (distX > 0) _obj.back().angle = (PI / 2.0 - atan(distY / distX)) * 180 / PI;
	else           _obj.back().angle = (PI + PI / 2.0 - atan(distY / distX)) * 180 / PI;
	_obj.back().dist = sqrtf(std::pow(distX, 2) + std::pow(distY, 2));
	if(previousdistX > 0) _obj.back().previousangle = (PI / 2.0 - atan(previousdistY / previousdistX)) * 180 / PI;
	else                  _obj.back().previousangle = (PI + PI / 2.0 - atan(previousdistY / previousdistX)) * 180 / PI;
	_obj.back().previousdist = sqrtf(std::pow(previousdistX, 2) + std::pow(previousdistY, 2));
}
void CScanView::OnLogDataDump(std::vector<scanDot_Complication> &_obj , char fname[]){
	if (_obj.empty() == false) {
		/*
		std::tm* logstart = std::localtime(&_obj_move_log.front().now);
		char fname[100];
		sprintf_s(fname, 100,"./LogData/DON'T_DELETE_LogData_%d_%d_%d_%d_%d.csv", logstart->tm_mon + 1, logstart->tm_mday,
			logstart->tm_hour, logstart->tm_min, logstart->tm_sec);
		*/
		std::ofstream log_file(fname, std::ios::app);
		for (int i = 0; i < (int)_obj.size(); ++i) {
			log_file << _obj[i].now << "," << _obj[i].uniqueid << ","
				<< _obj[i].angle << "," << _obj[i].dist << ","
				<< _obj[i].previousangle << "," << _obj[i].previousdist << ","
				<< _obj[i].milliseconds <<",\n";
		}
		log_file.close();
	}
	else {
		char filename[50];
		int ofstreamtesttmp = 2;
		sprintf_s(filename, 50,"./LogData/ofstream_%d.csv", ofstreamtesttmp);
		std::ofstream log_file(filename);
		log_file << ofstreamtesttmp << ",\n";
		log_file.close();
	}
}

void CScanView::OnViewFileData(CDC &memDC) {
	for (int i = 0; i < (int)_obj_read_data.size(); i++) {
		OnColorPolt(memDC, _obj_read_data[i].uniqueid, _obj_read_data[i].X, _obj_read_data[i].Y, 2);
	}
}
void CScanView::setReadComplicationData(int number, int data, scanDot_Complication &tmp_scan) {
	switch (number)
	{
	case 1: tmp_scan.uniqueid = data;       break;
	case 6: tmp_scan.milliseconds = data;   break;
	default: break;
	}
}
void CScanView::setReadComplicationData(int number, float data, scanDot_Complication &tmp_scan) {
	switch (number)
	{
	case 2: tmp_scan.angle = data;          break;
	case 3: tmp_scan.dist = data;           break;
	case 4: tmp_scan.previousangle = data;  break;
	case 5: tmp_scan.previousdist = data;   break;
	default: break;
	}
}
void CScanView::setReadComplicationData(int number, long long data, scanDot_Complication &tmp_scan) {
	switch (number)
	{
	case 0: tmp_scan.now = data;          break;
	default: break;
	}
}
void CScanView::setReadComplicationData(scanDot_Complication &tmp_scan) {
	_obj_read_data.push_back(tmp_scan);
}

void CScanView::setReadTimeData(int number, int data, scanDot_time &tmp_scan){
	switch (number)
	{
	case 0: tmp_scan.month = data;        break;
	case 1: tmp_scan.day = data;          break;
	case 2: tmp_scan.hour = data;         break;
	case 3: tmp_scan.min = data;          break;
	case 4: tmp_scan.sec = data;          break;
	case 5: tmp_scan.uniqueid = data;     break;
	case 8: tmp_scan.X = data;            break;
	case 9: tmp_scan.Y = data;            break;
	case 10: tmp_scan.cnt = data;         break;
	default: break;
	}
}

void CScanView::setReadTimeData(int number, float data, scanDot_time &tmp_scan) {
	switch (number)
	{
	case 6: tmp_scan.angle = data;        break;
	case 7: tmp_scan.dist = data;         break;
	default: break;
	}
}

void CScanView::setReadTimeData(scanDot_time &tmp_scan) {
	_obj_read_data.push_back(tmp_scan);
}
void CScanView::ClearReadData() {
	_obj_read_data.clear();
}
void CScanView::setReadBackData(int number, float data, scanDot_Simple &tmp_scan) {
	switch (number)
	{
	case 0: tmp_scan.X = data;        break;
	case 1: tmp_scan.Y = data;         break;
	default: break;
	}
}
void CScanView::setReadBackData(scanDot_Simple &tmp_scan) {
	_obj_background.push_back(tmp_scan);
}
void CScanView::OnSwitchViewMode(int newViewMode) {
	switch (newViewMode)
	{
	case VIEW_MODE_SCAN: 
		_ViewMode = VIEW_MODE_SCAN; 
		break;
	case VIEW_MODE_LOG: 
		_ViewMode = VIEW_MODE_LOG; 
		break;
	case VIEW_MODE_FILE: 
		_ViewMode = VIEW_MODE_FILE; 
		break;
	case VIEW_MODE_SPLIT:
		_ViewMode = VIEW_MODE_SPLIT;
		break;
	case VIEW_MODE_SPLITVIEW:
		_ViewMode = VIEW_MODE_SPLITVIEW;
		break;
	case VIEW_MODE_NONE:
		_ViewMode = VIEW_MODE_NONE;
		break;
	default: 
		break;
	}
}
void CScanView::setSwitchViewModeScan() {
	OnSwitchViewMode(VIEW_MODE_SCAN);
}
void CScanView::setSwitchViewModeLog() {
	OnSwitchViewMode(VIEW_MODE_LOG);
}
void CScanView::setSwitchViewModeFile() {
	OnSwitchViewMode(VIEW_MODE_FILE);
}
void CScanView::setSwitchViewModeSplit() {
	OnSwitchViewMode(VIEW_MODE_SPLIT);
}
void CScanView::setSwitchViewModeSplitView() {
	OnSwitchViewMode(VIEW_MODE_SPLITVIEW);
}
void CScanView::setSwitchViewModeNone() {
	OnSwitchViewMode(VIEW_MODE_NONE);
}

void CScanView::OnInputComplicationTime() {
	for (int i = 0; i < (int)_obj_move[0].size(); ++i) {
		scanDot_time tmp_scan;
		tmp_scan.angle = _obj_move[0][i].angle;
		tmp_scan.cnt = _obj_move[0][i].cnt;
		tmp_scan.dist = _obj_move[0][i].dist;
		tmp_scan.flag = _obj_move[0][i].flag;
		tmp_scan.uniqueid = _obj_move[0][i].uniqueid;
		tmp_scan.X = _obj_move[0][i].X;
		tmp_scan.Y = _obj_move[0][i].Y;
		std::tm* now = std::localtime(&_obj_move[0][i].now);
		tmp_scan.month = now->tm_mon;
		tmp_scan.day = now->tm_mday;
		tmp_scan.hour = now->tm_hour;
		tmp_scan.min = now->tm_min;
		tmp_scan.sec = now->tm_sec;
		_obj_move_log.push_back(tmp_scan);
	}
}

time_t CScanView::getReadStartnow() {
	try {
		return _obj_read_data.at(0).now;
	}
	catch (std::exception &e) {
		std::cout << "*******_obj_read_data.at(0).now********";
		std::cout << e.what() << std::endl;
		return 0;
	}
}
time_t CScanView::getReadEndnow() {
	try {
		return _obj_read_data.at(_obj_read_data.size() - 1).now;
	}
	catch (std::exception &e) {
		std::cout << "*******_obj_read_data[_obj_read_data.size() - 1].now********";
		std::cout << e.what() << std::endl;
		return 0;
	}
}
void CScanView::setReadStartTime(time_t time) {
	setstarttime = time;
}
void CScanView::setReadEndTime(time_t time) {
	setendtime = time;
}
time_t CScanView::getReadStartTime() {
	return setstarttime;
}
time_t CScanView::getReadEndTime() {
	return setendtime;
}
void CScanView::setMagnification(int tmp_num) {
	if (tmp_num > 0) {
		magnification = tmp_num;
	}
}
int CScanView::getMagnification() {
	return magnification;
}
void CScanView::setSizeSplitViewDot(int size) {
	if (size > 0) {
		sizesplitviewdot = size;
	}
}
int CScanView::getSizeSplitViewDot() {
	return sizesplitviewdot;
}
void CScanView::OnLogText(CDC &memDC) {
	char txtBuffer[100];
	memDC.SelectFont(bigfont);  //文字書く
	memDC.SetTextAlign(TA_LEFT | TA_BASELINE);
	memDC.SetTextColor(RGB(255, 255, 255));
	try {
		std::tm* start = std::localtime(&_obj_read_data.at(0).now);
		sprintf_s(txtBuffer,100, "%d/%d %d:%d:%d", start->tm_mon + 1, start->tm_mday, start->tm_hour, start->tm_min, start->tm_sec);
	}
	catch (std::exception &e) {
		std::cout << "*******6****7****";
		std::cout << e.what() << std::endl;
	}
	memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 60, txtBuffer);
	try {
		std::tm* end = std::localtime(&_obj_read_data.at(_obj_read_data.size() - 1).now);
		sprintf_s(txtBuffer, 100,"%d/%d %d:%d:%d", end->tm_mon + 1, end->tm_mday, end->tm_hour
			, end->tm_min, end->tm_sec);
	}
	catch (std::exception &e) {
		std::cout << "*******7*****7***";
		std::cout << e.what() << std::endl;
	}
	memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 80, txtBuffer);
	/*
	try {
		sprintf(txtBuffer, "%lld - %lld", _obj_read_data.at(0).now, _obj_read_data.at(_obj_read_data.size() - 1).now);
	}
	catch (std::exception &e) {
		std::cout << "*******8********";
		std::cout << e.what() << std::endl;
	}
	memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 100, txtBuffer);
	*/
	/*
	sprintf(txtBuffer, "%lld - %lld", setstarttime, setendtime);
	memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 120, txtBuffer);
	*/
	try {
		std::tm* setstart = std::localtime(&setstarttime);
		sprintf_s(txtBuffer,100, "%d/%d %d:%d:%d", setstart->tm_mon+1, setstart->tm_mday, setstart->tm_hour
			, setstart->tm_min, setstart->tm_sec);
	}
	catch (std::exception &e) {
		std::cout << "*******9********";
		std::cout << e.what() << std::endl;
	}
	memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 140, txtBuffer);
	try {
		std::tm* setend = std::localtime(&setendtime);
		sprintf_s(txtBuffer, "%d/%d %d:%d:%d", setend->tm_mon+1, setend->tm_mday, setend->tm_hour
			, setend->tm_min, setend->tm_sec);
	}
	catch (std::exception &e) {
		std::cout << "*******10********";
		std::cout << e.what() << std::endl;
	}
	memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 160, txtBuffer);
}

void CScanView::setColorMode(int colormode) {
	if (0 <= colormode && colormode <= 3 || colormode == 99) {
		colormodedefine = colormode;
	}
}
int  CScanView::getColorMode() {
	return colormodedefine;
}

void CScanView::OnLinePlot(scanDot_Complication &tmp_scan, CDC &memDC) {
	
	CPen linePen;
	if (tmp_scan.uniqueid % 3 == 0) {
		memDC.SetDCPenColor(RGB(255, 60, 60));  //色 
		linePen.CreatePen(PS_SOLID, 3, RGB(255, 60, 60));
	}
	else if (tmp_scan.uniqueid % 3 == 1) {
		memDC.SetDCPenColor(RGB(60, 255, 60));  //色
		linePen.CreatePen(PS_SOLID, 3, RGB(60, 255, 60));
	}
	else {
		memDC.SetDCPenColor(RGB(60, 255, 255));  //色
		linePen.CreatePen(PS_SOLID, 3, RGB(60, 255, 255));
	}
	//memDC.SelectStockPen(DC_PEN);
	memDC.SelectPen(linePen);
	memDC.MoveTo(tmp_scan.previousX + view_X * getDistscal(), tmp_scan.previousY + view_Y * getDistscal());
	memDC.LineTo(tmp_scan.X + view_X * getDistscal(), tmp_scan.Y + view_Y * getDistscal());
}
float CScanView::getDistscal() {
	return _distscal;
}
void CScanView::setDistscal(float distscal) {
	_distscal = distscal;
}
CPoint CScanView::getCenterPt() {
	return _centerPt;
}
void CScanView::setCenterPt(CPoint &centerPt) {
	_centerPt = centerPt;
}
void CScanView::OnSetColorHeatmap(CDC &memDC, int argi) {
	if (_obj_split_data[argi].RGBsum > 200) {
		memDC.SetDCPenColor(RGB(255, 0, 0));  //色
	}
	else if (_obj_split_data[argi].RGBsum > 150) {
		memDC.SetDCPenColor(RGB(255, 128, 0));  //色
	}
	else if (_obj_split_data[argi].RGBsum > 100) {
		memDC.SetDCPenColor(RGB(255, 255, 0));  //色
	}
	else if (_obj_split_data[argi].RGBsum > 50) {
		memDC.SetDCPenColor(RGB(0, 255, 255));  //色
	}
	else if (_obj_split_data[argi].RGBsum > 0) {
		memDC.SetDCPenColor(RGB(0, 0, 255));  //色
	}
}
void CScanView::OnPlotRectHeatmap(CDC &memDC, int argi, int x, int y) {
	if (_obj_split_data[argi].RGBsum > 200) {
		memDC.FillSolidRect(x, y, 1, 1, RGB(255, 0, 0));
		//memDC.FillSolidRect(x, y, 1, 1, RGB(255, 255, 255));
	}
	else if (_obj_split_data[argi].RGBsum > 150) {
		memDC.FillSolidRect(x, y, 1, 1, RGB(255, 128, 0));
		//memDC.FillSolidRect(x, y, 1, 1, RGB(200, 200, 200));
	}
	else if (_obj_split_data[argi].RGBsum > 100) {
		memDC.FillSolidRect(x, y, 1, 1, RGB(255, 255, 0));
		//memDC.FillSolidRect(x, y, 1, 1, RGB(150, 150, 150));
	}
	else if (_obj_split_data[argi].RGBsum > 50) {
		memDC.FillSolidRect(x, y, 1, 1, RGB(0, 255, 255));
		//memDC.FillSolidRect(x, y, 1, 1, RGB(100, 100, 100));
	}
	else if (_obj_split_data[argi].RGBsum > 0) {
		memDC.FillSolidRect(x, y, 1, 1, RGB(0, 0, 255));
		//memDC.FillSolidRect(x, y, 1, 1, RGB(50, 50, 50));
	}
}
void CScanView::setReadDataUnification() {
	
	for (int i = (int)_obj_read_data.size() - 1; i >= 0; --i) { //複数台の統合_obj_read_dataの統合
		try {
			float radi = (float)(_obj_read_data[i].angle*PI / 180.0);
			float distXi = sin(radi)*(_obj_read_data[i].dist);
			float distYi = cos(radi)*(_obj_read_data[i].dist);
			for (int j = i-1 ; j>= 0; --j) {
				if ((int)(_obj_read_data[i].milliseconds / 100) == (int)(_obj_read_data[j].milliseconds / 100)) {
					if (_obj_read_data[i].uniqueid != _obj_read_data[j].uniqueid) {
						float radj = (float)(_obj_read_data[j].angle*PI / 180.0);
						float distXj = sin(radj)*(_obj_read_data[j].dist);
						float distYj = cos(radj)*(_obj_read_data[j].dist);
						float distance = sqrtf(std::pow(abs(distXi) - abs(distXj), 2) + std::pow(abs(distYi) - abs(distYj), 2));
						if (distance <= 500) { //2つの物体の間の距離
							_obj_read_data.erase(_obj_read_data.begin() + i);
							break;
						}
					}
				}
				if (_obj_read_data[i].now != _obj_read_data[j].now) {
					break;
				}
			}
		}
		catch (std::exception &e) {
			std::cout << "*********setReadDataUnification*********";
			std::cout << e.what() << std::endl;
		}
	}
}

void CScanView::setObjAnother(int uniqueid, float dist, float angle) {
	scanDot_Complication tmp;
	tmp.uniqueid = uniqueid;
	tmp.dist = dist;
	tmp.angle = angle;
	/*
	tmp.cnt = 0;
	tmp.distX = 0;
	tmp.distY = 0;
	tmp.flag = 0;
	tmp.milliseconds = 0;
	tmp.previousangle = 0;
	tmp.previousdist = 0;
	tmp.previousX = 0;
	tmp.previousY = 0;
	tmp.X = 0;
	tmp.Y = 0;
	*/
		//tmp.Y = (int)(getCenterPt().y - cos(angle*PI/180.0) * (dist*getDistscal()));
		//tmp.X = (int)(sin((float)(angle*PI/180.0)) * (dist*getDistscal()) + getCenterPt().x);
		OnObjAnotherPush(tmp);
		//std::wcout << "thread:"<< _globle_obj_another.size() <<std::endl;
		ATLASSERT(_CrtCheckMemory());
}

void CScanView::OnObjAnotherPush(scanDot_Complication tmp) {
	std::unique_lock<std::mutex> lock(mtx_);
	try{
		_globle_obj_another.push_back(tmp);
	}
	catch (std::exception &e) {
		std::cout << "**********************************" << std::endl;
		std::cout << e.what() << std::endl;
	}
}
void CScanView::OnObjAnotherClear() {
	std::unique_lock<std::mutex> lock(mtx_);
	try {
		_globle_obj_another.clear();
		_globle_obj_another.shrink_to_fit();
	}
	catch (std::exception &e) {
		std::cout << "**********************************" << std::endl;
		std::cout << e.what() << std::endl;
	}
}
void CScanView::OnObjAnotherCopy(std::vector<scanDot_Complication> &_obj) {
	std::unique_lock<std::mutex> lock(mtx_);
	try {
		for (int i = 0; i < _globle_obj_another.size(); i++) {
			_obj.push_back(_globle_obj_another.at(i));
		}
	}
	catch (std::exception &e) {
		std::cout << "**********************************" << std::endl;
		std::cout << e.what() << std::endl;
	}
}