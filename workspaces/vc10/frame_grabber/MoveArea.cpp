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
	//�����̒��Ȃ�Ture,
	bool re_bool = false;
	if (area == 0) {
		if (-6300 < distX && distX < 8900) {  //kc104�̕����̒�
			if (-4400 < distY && distY < 4000) {
				//�ʂ̐Î~��
				if (4100 < distX && distX < 4500 && 0 < distY && distY < 1100);  //��(���ړI)
				else if (-850 < distX && distX < -400 && -3200 < distY && distY < -2900);  //��(�W��)
				else if (-2400 < distX && distX < 3600 && -3350 < distY && distY < -3200);  //�K���X(�W��)
				else if (-2400 < distX && distX < -2200 && -5000 < distY && distY < -3350);  //�K���X(�W��)
				else if (3400 < distX && distX < 3500 && -4500 < distY && distY < -3500);  //�K���X(�W��)
				else if (8000 < distX  &&  3000< distY );  //���ړI�X�y�[�X
				else if (distX < -4400 && distY < 2850);  //���b�J�[
				else    re_bool = True; //kc104�̕����̒����ʂ̐Î~���̒��łȂ�
			}
		}
		return re_bool;
	}
	else if (area == 1) {
		if(-9000 < distX && distX < 3000) {  //kc104�̕����̒�
			if (-6000 < distY && distY < 2150) {
				//�ʂ̐Î~��
				if (1990 < distX && 0 < distY);  //�p�[�e�B�V����
				else if (-1700 < distX && distX < 1200 && -1800 < distY && distY < -1450);  //�z���C�g�{�[�h
				else if (-3000 < distX && distX < -1600 && 1900 < distY && distY < 2200);  //��
				else if (-2000 < distX && distX < -1500 && -1500 < distY && distY < -1000);  //��
				else    re_bool = True; //kc104�̕����̒����ʂ̐Î~���̒��łȂ�
			}
		}
		return re_bool;
	}else if (area == 2) {
		if (-9000 < distX && distX < 3200) {  //kc104�̕����̒�
			if ( -2500 < distY && distY < 5900) {
				//�ʂ̐Î~��
				if (-1800 < distX && distX < 1300 && 2900 < distY && distY < 3200 );  //�z���C�g�{�[�h
				else if (-2400 < distX && distX < 200 && -2500 < distY && distY < -2300);  //�f�C�X�v���C
				else if (-1650 < distX && distX < -1500 && 1030 < distY && distY < 1180);  //��
				else if (-2450 < distX && distX < -2350 && -1050 < distY && distY < -900);  //�W��
				else    re_bool = True; //kc104�̕����̒����ʂ̐Î~���̒��łȂ�
			}
		}
		return re_bool;
	}
	else if (area == 3) {
		if (-4300 < distX && distX < 9000) {  //kc104�̕����̒�
			if (-9000 < distY && distY < 150) {
				//�ʂ̐Î~��
				if (-4300 < distX && distX < -600 &&  distY < -1100);  //��
				else if ( distX < -600 && distY < -1200);  //��
				else    re_bool = True; //kc104�̕����̒����ʂ̐Î~���̒��łȂ�
			}
		}
		return re_bool;
	}
	else if (area == 50) {   //�r�b�O�T�C�g
 		if (-8000 < distX && distX < 3100) {
			if (-3500 < distY && distY < 2300) {
			//�ʂ̐Î~��
			if(0 < distX && 2200 < distY);  //��
			else if (-2700 < distX && distX < 500 && 1800 < distY && distY <= 2300);  //�[����
			else if (-8000 < distX && distX < -2500 && -700 < distY && distY < -400);  //
			else if (-2800 < distX && distX < -2600 && -700 < distY && distY < 200);  // 
			else if (700 < distX && distX < 900 && -700 < distY && distY < 200);
			else    re_bool = True; //kc104�̕����̒����ʂ̐Î~���̒��łȂ�
			}
		}
		return re_bool;
	}
	else if (area == 99) {  //�e�X�g�p 
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
