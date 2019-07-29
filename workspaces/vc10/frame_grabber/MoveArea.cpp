#pragma once
#include "stdafx.h"
#include "MoveArea.h"


MoveArea::MoveArea()
{
}
MoveArea::~MoveArea()
{
}
bool MoveArea::checkMoveArea(float distX, float distY, int area) {
	//条件の中ならTure,
	bool re_bool = false;
	if (area == 0) {
		if (-6300 < distX && distX < 8900) {  //kc104の部屋の中
			if (-4400 < distY && distY < 4000) {
				//個別の静止物
				if (4100 < distX && distX < 4500 && 0 < distY && distY < 1100);  //木(多目的)
				else if (-850 < distX && distX < -400 && -3200 < distY && distY < -2900);  //木(集中)
				else if (-2400 < distX && distX < 3600 && -3350 < distY && distY < -3200);  //ガラス(集中)
				else if (-2400 < distX && distX < -2200 && -5000 < distY && distY < -3350);  //ガラス(集中)
				else if (3400 < distX && distX < 3500 && -4500 < distY && distY < -3500);  //ガラス(集中)
				else if (8000 < distX  &&  3000< distY );  //多目的スペース
				else if (distX < -4400 && distY < 2850);  //ロッカー
				else    re_bool = True; //kc104の部屋の中かつ個別の静止物の中でない
			}
		}
		return re_bool;
	}
	else if (area == 1) {
		if(-9000 < distX && distX < 3000) {  //kc104の部屋の中
			if (-6000 < distY && distY < 2150) {
				//個別の静止物
				if (1990 < distX && 0 < distY);  //パーティション
				else if (-1700 < distX && distX < 1200 && -1800 < distY && distY < -1450);  //ホワイトボード
				else if (-3000 < distX && distX < -1600 && 1900 < distY && distY < 2200);  //柱
				else if (-2000 < distX && distX < -1500 && -1500 < distY && distY < -1000);  //木
				else    re_bool = True; //kc104の部屋の中かつ個別の静止物の中でない
			}
		}
		return re_bool;
	}else if (area == 2) {
		if (-9000 < distX && distX < 3200) {  //kc104の部屋の中
			if ( -2500 < distY && distY < 5900) {
				//個別の静止物
				if (-1800 < distX && distX < 1300 && 2900 < distY && distY < 3200 );  //ホワイトボード
				else if (-2400 < distX && distX < 200 && -2500 < distY && distY < -2300);  //デイスプレイ
				else if (-1650 < distX && distX < -1500 && 1030 < distY && distY < 1180);  //柱
				else if (-2450 < distX && distX < -2350 && -1050 < distY && distY < -900);  //集中
				else    re_bool = True; //kc104の部屋の中かつ個別の静止物の中でない
			}
		}
		return re_bool;
	}
	else if (area == 3) {
		if (-4300 < distX && distX < 9000) {  //kc104の部屋の中
			if (-9000 < distY && distY < 150) {
				//個別の静止物
				if (-4300 < distX && distX < -600 &&  distY < -1100);  //壁
				else if ( distX < -600 && distY < -1200);  //壁
				else    re_bool = True; //kc104の部屋の中かつ個別の静止物の中でない
			}
		}
		return re_bool;
	}
	else if (area == 50) {   //ビッグサイト
 		if (-8000 < distX && distX < 3100) {
			if (-3500 < distY && distY < 2300) {
			//個別の静止物
			if(0 < distX && 2200 < distY);  //壁
			else if (-2700 < distX && distX < 500 && 1800 < distY && distY <= 2300);  //擬似窓
			else if (-8000 < distX && distX < -2500 && -700 < distY && distY < -400);  //
			else if (-2800 < distX && distX < -2600 && -700 < distY && distY < 200);  // 
			else if (700 < distX && distX < 900 && -700 < distY && distY < 200);
			else    re_bool = True; //kc104の部屋の中かつ個別の静止物の中でない
			}
		}
		return re_bool;
	}
	else if (area == 99) {  //テスト用 
		if (-1800 < distX && distX < 5550) {  
			if (-4800 < distY && distY < 1100) {
				if (distX < 600 && 400 < distY);
				else re_bool = True; 			}
		}
		return re_bool;
	}
	else {
		return re_bool;
	}
}
