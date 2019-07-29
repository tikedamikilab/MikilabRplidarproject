#pragma once
#include "stdafx.h"
#include "ObjAnotherCalc.h"
#include "StructscanDot.h"
#include <iostream>
#include <ctime>
using namespace std;

const float PI = (float)3.14159265;

ObjAnotherCalc::ObjAnotherCalc()
{
}

ObjAnotherCalc::~ObjAnotherCalc()
{
}

void ObjAnotherCalc::ObjAnotherAll(std::vector<scanDot_Complication> &_obj_another, std::vector<scanDot_Complication> &_obj_another_old, float distscal, CPoint centerPt)
{
	/*-----------------------*/
	//複数追跡処理
	/*-----------------------*/
	OnsetDistScal(distscal);
	OnsetCenterPt(centerPt);
	OnObjAnotherInitialize(_obj_another, _obj_another_old);
	OnObjAnotherIntegration(_obj_another);
	OnObjAnotherTrace(_obj_another, _obj_another_old);

	_obj_another_old.clear();
	_obj_another_old = _obj_another;
}

void ObjAnotherCalc::OnObjAnotherInitialize(std::vector<scanDot_Complication> &_obj_another, std::vector<scanDot_Complication> &_obj_another_old) {
	SYSTEMTIME st;
	GetLocalTime(&st);
	try {
		for (int i = 0; i < _obj_another.size(); i++) {
			float rad = (float)(_obj_another.at(i).angle*PI / 180.0);
			_obj_another.at(i).distX = sin(rad)*(_obj_another.at(i).dist);
			_obj_another.at(i).distY = cos(rad)*(_obj_another.at(i).dist);
			_obj_another.at(i).X = _obj_another.at(i).distX * _distscal + _centerPt.x;
			_obj_another.at(i).Y = _centerPt.y - _obj_another.at(i).distY * _distscal;
			_obj_another.at(i).now = std::time(0); //nowの更新
			_obj_another.at(i).milliseconds = st.wMilliseconds;
			_obj_another.at(i).flag = -1;
			_obj_another.at(i).cnt = -1;
		}
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	try {
		for (int i = 0; i < _obj_another_old.size(); i++) {
			_obj_another_old.at(i).flag = -1;
			_obj_another_old.at(i).cnt = -1;
		}
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
void ObjAnotherCalc::OnObjAnotherIntegration(std::vector<scanDot_Complication> &_obj_another) {
	try {
		for (int i = _obj_another.size() - 1; i >= 0; i--) {
			for (int j = i - 1; j >= 0; j--) {
				float distance = sqrtf(std::pow(_obj_another.at(i).distX - _obj_another.at(j).distX, 2) + std::pow(_obj_another.at(i).distY - _obj_another.at(j).distY, 2));
				if (distance < 300  /*テキトー*/) {
					_obj_another.at(j).dist = (_obj_another.at(i).dist + _obj_another.at(j).dist) / 2;
					if (abs(_obj_another.at(i).angle - _obj_another.at(j).angle) > 300) {
						_obj_another.at(j).angle = (_obj_another.at(i).angle + _obj_another.at(j).angle + 360) / 2;
						if (_obj_another.at(j).angle > 360) {
							_obj_another.at(j).angle -= 360;
						}
					}
					else {
						_obj_another.at(j).angle = (_obj_another.at(i).angle + _obj_another.at(j).angle) / 2;
					}
					_obj_another.at(j).distX = (_obj_another.at(i).distX + _obj_another.at(j).distX) / 2;
					_obj_another.at(j).distY = (_obj_another.at(i).distY + _obj_another.at(j).distY) / 2;
					_obj_another.at(j).X = _obj_another.at(j).distX * _distscal + _centerPt.x;
					_obj_another.at(j).Y = _centerPt.y - _obj_another.at(j).distY * _distscal;
					std::swap(_obj_another.at(i), _obj_another.back());
					_obj_another.pop_back();
					break;
				}
			}
		}
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
void ObjAnotherCalc::OnObjAnotherTrace(std::vector<scanDot_Complication> &_obj_another, std::vector<scanDot_Complication> &_obj_another_old) {
	try {
		for (int i = _obj_another.size() - 1; i >= 0; i--) {
			float bestdistance = 400;  //肩幅
			int usedold = -1;
			for (int j = _obj_another_old.size() - 1; j >= 0; j--) {
				float distance = sqrtf(std::pow(_obj_another.at(i).distX - _obj_another_old.at(j).distX, 2) + std::pow(_obj_another.at(i).distY - _obj_another_old.at(j).distY, 2));
				if (distance < bestdistance && _obj_another_old.at(j).flag == -1) {
					bestdistance = distance;
					usedold = j;
				}
			}
			if (usedold == -1) {
				_obj_another.at(i).previousangle = _obj_another.at(i).angle;
				_obj_another.at(i).previousdist = _obj_another.at(i).dist;
				_obj_another.at(i).previousX = _obj_another.at(i).X;
				_obj_another.at(i).previousY = _obj_another.at(i).Y;
			}
			else {
				_obj_another.at(i).previousangle = _obj_another_old.at(usedold).angle;
				_obj_another.at(i).previousdist = _obj_another_old.at(usedold).dist;
				_obj_another.at(i).previousX = _obj_another_old.at(usedold).X;
				_obj_another.at(i).previousY = _obj_another_old.at(usedold).Y;
				_obj_another.at(i).uniqueid = _obj_another_old.at(usedold).uniqueid;
				_obj_another_old.at(usedold).flag = i;
				_obj_another_old.at(usedold).cnt = (int)bestdistance;
			}
		}
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
void ObjAnotherCalc::OnsetDistScal(float distscal){
	_distscal = distscal;
}
void ObjAnotherCalc::OnsetCenterPt(CPoint centerPt){
	_centerPt = centerPt;
}