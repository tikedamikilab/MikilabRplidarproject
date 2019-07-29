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
/////////////////////////////////////////////////////////////////////////////

#pragma once
//#include <ctime>
#include "LogDataSplit.h"
#include "StructscanDot.h"
#include "MoveArea.h"
#include "MoveObjSend.h"
#include "ObjAnotherCalc.h"
using namespace std;

const int SCAN_DATA_SIZE = 1;  //_scan_dataの数，_scan_dataはscanDot型の配列をSCAN_DATA_SIZE個持つ
const int OBJ_CENTER_SIZE = 1;
const int OBJ_SEMI_SIZE = 1;
const int OBJ_MOVE_SIZE = 40;
const int OBJ_MOVE_FEW = 10;
const int OBJ_MOVE_MANY = 100;
const int MINIMUM_ELE_NUM = 2;
const int TERM_LOG_DATA_DUMP = 100;

struct BuffInt {
	int i;
	int j;
};

class CScanView : public CWindowImpl<CScanView>
{
public:
	LogDataSplit m_logdatasplit;
	MoveArea m_movearea;
	MoveObjSend m_moveobjsend;

	DECLARE_WND_CLASS(NULL)
	enum MODE{
		VIEW_MODE_SCAN           = 0,
		VIEW_MODE_LOG            = 1,
		VIEW_MODE_FILE           = 2,
		VIEW_MODE_SPLIT          = 3,
		VIEW_MODE_SPLITVIEW      = 4,
		VIEW_MODE_NONE           = 99,
	};
	
	enum factor {
		FACTOR_ANGLE             = 0,
		FACTOR_DIST              = 1,
		FACTOR_QUALITY           = 2,
		FACTOR_X                 = 3,
		FACTOR_Y                 = 4,
		FACTOR_CNT               = 5,
		FACTOR_NOW               = 6,
		FACTOR_UNIQUEID          = 7,
		FACTOR_FLAG              = 8,
	};

	const enum view {  //レイアウト用
		//view_X = -2000,
		view_X = 0,
		//view_Y = -2000,
		view_Y = 0,
	};
	const enum position_bigsite { //キャリブレーション用
		position_bigsite_X = -2700,
		position_bigsite_Y = 700,
	};
	const enum position_one {
		position_one_X = -6000,
		position_one_Y = 2200,
	};
	const enum position_two {
		position_two_X = -5800,
		position_two_Y = -2200,
	};
	const enum position_three {
		position_three_X = 3900,
		position_three_Y = 3800,
	};

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CScanView)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MSG_WM_PAINT(OnPaint)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_MOUSEMOVE(OnMouseMove)
	END_MSG_MAP()

    void DoPaint(CDCHandle dc);

    const std::vector<scanDot> & getScanList() const {
        return _scan_data[0];
    }
	const std::vector<scanDot_Complication> & getLogData() const {
		return _obj_move_log;
	}

    void onDrawSelf(CDCHandle dc);
    void setScanData(rplidar_response_measurement_node_t *buffer, size_t count, float frequency, bool is4kmode);

	CScanView();

    BOOL OnEraseBkgnd(CDCHandle dc);
	void OnMouseMove(UINT nFlags, CPoint point);
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnPaint(CDCHandle dc);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void OnCopy(std::vector<scanDot_Complication> *obj, int i, int j);
	void OnObjCenterPush(scanDot_Simple *objsum, float distScale, long x, long y);
	void OnMoveBase(scanDot_Simple *tmp_scan, int i);
	void OnMoveComplicationBase(scanDot_Complication *tmp_scan, int i);
	inline void OnMoveMake(int i, std::vector<scanDot_Simple> *obj);
	inline void OnMoveComplicationMake(int i, std::vector<scanDot_Complication> *obj);
	void OnObjSum(scanDot_Simple *objsum, float dist, float angle, bool IfTrueppElone);
	void OnObjCntUpdata(std::vector<scanDot_Complication> *obj, int j, bool boo);
	void OnColorPolt(CDC &memDC, int color, int x, int y, int size);
	void OnRingPlot(CDC &memDC, CPoint &centerPt, int maxPixelR, float distScale);
	CPoint OnTransformationPolortoRectangular(float angle, float dist, CPoint &centerPt, float distScale);
	CPoint TransformationPolortoRectangular(float angle, float dist);
	void TransformationPolortoRectangular(scanDot_Complication &tmp_scan);
	void OnViewLogData(CDC &memDC);
	void OnViewFileData(CDC &memDC);
	void OnSwitchViewMode(int newViewMode);
	void setSwitchViewModeScan();
	void setSwitchViewModeLog();
	void setSwitchViewModeFile();
	void setSwitchViewModeSplit();
	void setSwitchViewModeSplitView();
	void setSwitchViewModeNone();
	void ClearLogData(); 
	void setReadTimeData(int number, int data, scanDot_time &tmp_scan);
	void setReadTimeData(int number, float data, scanDot_time &tmp_scan);
	void setReadTimeData(scanDot_time &tmp_scan);
	void setReadBackData(int number, float data, scanDot_Simple &tmp_scan);
	void setReadBackData(scanDot_Simple &tmp_scan);
	void OnInputComplicationTime();
	std::time_t getReadStartnow();
    std::time_t getReadEndnow();
	void setReadComplicationData(int number, int data, scanDot_Complication &tmp_scan);
	void setReadComplicationData(int number, float data, scanDot_Complication &tmp_scan);
	void setReadComplicationData(int number, long long data, scanDot_Complication &tmp_scan);
	void setReadComplicationData(scanDot_Complication &tmp_scan);
	void setReadStartTime(time_t time);
	void setReadEndTime(time_t time);
	std::time_t getReadStartTime();
	std::time_t getReadEndTime();
	void OnLogDataDump(std::vector<scanDot_Complication> &_obj, char fname[]);
	void setMagnification(int tmp_num);
	int  getMagnification();
	void OnLogText(CDC &memDC);
	void setSizeSplitViewDot(int size);
	int  getSizeSplitViewDot();
	void ClearReadData();
	void setColorMode(int colormode);
	int  getColorMode();
	void OnLinePlot(scanDot_Complication &tmp_scan, CDC &memDC);
	void setDistscal(float distscal);
	float getDistscal();
	void setCenterPt(CPoint &centerpt);
	CPoint getCenterPt();
	void OnPlotBackground(CDC &memDC, int calibration_X, int calibration_Y);
	void OnbigsiteBackground(CDC &memDC, int calibration_X, int calibration_Y, CRect clientRECT);
	inline float OndistXtoendPtX(float distX);
	inline float OndistYtoendPtY(float distY);
	inline void OnDeskStandardPlot(CDC &memDC, int deskcenterX, int deskcenterY);
	void OnSetColorHeatmap(CDC &memDC, int argi);
	void OnPlotRectHeatmap(CDC &memDC, int argi, int x, int y);
	void OnPositionGloble(int positionX, int positionY, std::vector<scanDot_Complication> &_obj);
	void setReadDataUnification();
	void setObjAnother(int uniqueid, float dist, float angle);
	void OnObjAnotherPush(scanDot_Complication tmp);
	void OnObjAnotherClear();
	void OnObjAnotherCopy(std::vector<scanDot_Complication> &_obj);
	void OnPictureView(CDC &memDC, CBitmap &memBitmap);

private:
	//バグったら考える
	//std::vector<std::vector<scanDot_Complication>> tes;
	std::vector<scanDot_Simple> _obj_center[OBJ_CENTER_SIZE];
	std::vector<scanDot_Complication> _obj_semi_move[OBJ_SEMI_SIZE];
	std::vector<scanDot_Complication> _obj_move[OBJ_MOVE_SIZE];
	std::vector<scanDot_Complication> _obj_move_log;
	std::vector<scanDot_Complication> _obj_read_data;
	std::vector<scanDot_Complication> _obj_another_calc_log;
	std::vector<scanDot_Simple> _obj_background;
protected:
    CFont stdfont;
    CFont bigfont;
    POINT                _mouse_pt;
    float                _mouse_angle;
    std::vector<scanDot> _scan_data[SCAN_DATA_SIZE];
    float                _scan_speed;
    float                _current_display_range;
    int                  _sample_counter;
    _u64                 _last_update_ts;
    bool                 _is4kmode;
	//追加
	int                  _ViewMode = VIEW_MODE_SCAN;
	std::time_t               setstarttime = 0;
	std::time_t               setendtime = 0;
	std::time_t               logstarttime = std::time(0);
	int                  magnification = 100;
	std::vector<scanDot_RGBCount> _obj_split_data;
	int                  sizesplitviewdot = 2;
	int                  colormodedefine = 0;
	float                _distscal = 0;
	CPoint               _centerPt;
	int                  _position = 1;  //ここの数字変える 0: 1: 2: 3: 99:
	int                  _UniqueId = _position * 100000;
};
