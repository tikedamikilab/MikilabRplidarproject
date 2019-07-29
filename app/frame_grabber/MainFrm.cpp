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

// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "scanView.h"
#include "MainFrm.h"
#include "FreqSetDlg.h"
#include "LogTermSetDlg.h"
#include "drvlogic\lidarmgr.h"
#include "..\..\..\sdk\workspaces\vc10\frame_grabber\ReadDataSet.h"

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include <thread>
#include "MoveObjRecieve.h"
using namespace std;

const int REFRESEH_TIMER = 0x800;

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg)) {
		return TRUE;
	}
    if (m_hWndClient == m_scanview.m_hWnd){
        return m_scanview.PreTranslateMessage(pMsg);
    } else {
        return FALSE;
    }
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE); 

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	/***********************************************************/
	//
	//ツールバー系
	//
	AddSimpleReBarBand(hWndCmdBar);
	//AddSimpleReBarBand(hWndToolBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);
	//CreateSimpleStatusBar();
    /***********************************************************/
	m_hWndClient =m_scanview.Create(m_hWnd, rcDefault, NULL, WS_CHILD  | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
	
    workingMode = WORKING_MODE_IDLE;
    LidarMgr::GetInstance().lidar_drv->getDeviceInfo(devInfo);
    LidarMgr::GetInstance().lidar_drv->checkMotorCtrlSupport(support_motor_ctrl);
    onUpdateTitle();
    // setup timer
    this->SetTimer(REFRESEH_TIMER, 1000/30);
    checkDeviceHealth();
    UISetCheck(ID_CMD_STOP, 1);
    forcescan = 0;
    useExpressMode = true;
    inExpressMode = false;
    UISetCheck(ID_OPTION_EXPRESSMODE, TRUE);
	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    LidarMgr::GetInstance().lidar_drv->stopMotor();
	// unregister message filtering and idle updates
    this->KillTimer(REFRESEH_TIMER);
    
    CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;

	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document

	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
    switch (workingMode)
    {
    case WORKING_MODE_SCAN:
        onRefreshScanData();
        break;
    }
}

LRESULT CMainFrame::OnCmdReset(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    if (MessageBox("The device will reboot.", "Are you sure?",
        MB_OKCANCEL|MB_ICONQUESTION) != IDOK) {
            return 0;
    }


    onSwitchMode(WORKING_MODE_IDLE);
    LidarMgr::GetInstance().lidar_drv->reset();
	return 0;
}
LRESULT CMainFrame::OnCmdStop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    onSwitchMode(WORKING_MODE_IDLE);
	return 0;
}


LRESULT CMainFrame::OnCmdScan(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    onSwitchMode(WORKING_MODE_SCAN);
	return 0;
}

LRESULT CMainFrame::OnCmdView(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	onSwitchMode(WORKING_MODE_VIEW);
	return 0;
}

LRESULT CMainFrame::OnOptForcescan(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    forcescan=!forcescan;
    UISetCheck(ID_OPT_FORCESCAN, forcescan?1:0);
	return 0;
}

LRESULT CMainFrame::OnOptExpressMode(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    useExpressMode=!useExpressMode;
    UISetCheck(ID_OPTION_EXPRESSMODE, useExpressMode?1:0);
	return 0;
}

LRESULT CMainFrame::OnFileDumpdata(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    switch (workingMode) {
    case WORKING_MODE_SCAN:
        {
            //capture the snapshot
            std::vector<scanDot> snapshot = m_scanview.getScanList();

            //prompt
            CFileDialog dlg(FALSE);
            if (dlg.DoModal()==IDOK) {
                FILE * outputfile = fopen(dlg.m_szFileName, "w");
                fprintf(outputfile, "#RPLIDAR SCAN DATA\n#COUNT=%d\n#Angle\tDistance\tQuality\n",snapshot.size());
                for (int pos = 0; pos < (int)snapshot.size(); ++pos) {
                    fprintf(outputfile, "%.4f %.1f %d\n", snapshot[pos].angle, snapshot[pos].dist, snapshot[pos].quality);
                }
                fclose(outputfile);
            }
        }
        break;

    }
	return 0;
}
LRESULT CMainFrame::OnViewMove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_scanview.setSwitchViewModeLog();
	return 0;
}

LRESULT CMainFrame::OnLogDumpDataAndClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{	
	//capture the snapshot
	std::vector<scanDot_Complication> snapshot = m_scanview.getLogData();

	//prompt
	CFileDialog dlg(FALSE);
	if (dlg.DoModal() == IDOK) {
		FILE * outputfile = fopen(dlg.m_szFileName, "w");
		for (int pos = 0; pos < (int)snapshot.size(); ++pos) {
			fprintf(outputfile, "%lld,%d,%.2f,%.0f,%.2f,%f,%d\n", (long long)snapshot[pos].now, snapshot[pos].uniqueid, 
				snapshot[pos].angle, snapshot[pos].dist, snapshot[pos].previousangle, snapshot[pos].previousdist, 
				snapshot[pos].milliseconds);
		}
		fclose(outputfile);
	}
	m_scanview.ClearLogData();
	return 0;
}

vector<string> OnSplit(string& input, char delimiter)
{
	istringstream stream(input);
	string field;
	vector<string> result;
	while (getline(stream, field, delimiter)) {
		result.push_back(field);
	}
	return result;
}

LRESULT CMainFrame::OnLogRead(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK) {
		ifstream ifs(dlg.m_szFileName);
		string line;
		
		while (getline(ifs, line)) {
			std::vector<string> strvec = OnSplit(line, ',');
			scanDot_time tmp_scan;
			for (int i = 0; i<strvec.size(); i++) {
				if (i == 0)
				{
					m_scanview.setReadComplicationData(i, stoll(strvec.at(i)), tmp_scan);
				}
				else if (i == 2 || i == 3 || i == 4 || i == 5)
				{
					m_scanview.setReadComplicationData(i, stof(strvec.at(i)), tmp_scan);
				}
				else {
					m_scanview.setReadComplicationData(i, stoi(strvec.at(i)), tmp_scan);
				}
			}
			m_scanview.TransformationPolortoRectangular(tmp_scan);
			m_scanview.setReadTimeData(tmp_scan);
		}
		//m_scanview.setReadDataUnification();
		m_scanview.setSwitchViewModeFile();
	}
	return 0;
}
LRESULT CMainFrame::OnBackgroundRead(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	switch (workingMode) {
	case WORKING_MODE_IDLE:
	{
		CFileDialog dlg(TRUE);
		if (dlg.DoModal() == IDOK) {
			ifstream ifs(dlg.m_szFileName);
			string line;
			while (getline(ifs, line)) {
				std::vector<string> strvec = OnSplit(line, ',');
				scanDot_Simple tmp_scan;
				for (int i = 0; i < strvec.size(); i++) {
					m_scanview.setReadBackData(i, stoi(strvec.at(i)), tmp_scan);
				}
				m_scanview.setReadBackData(tmp_scan);
			}
		}
	}
	break;
	default: 
		break;
	}
	return 0;
}

void    CMainFrame::onRefreshScanData()
{
    rplidar_response_measurement_node_t nodes[2048];
    //size_t cnt = _countof(nodes);
	size_t cnt = 2048;
	RPlidarDriver * lidar_drv = LidarMgr::GetInstance().lidar_drv;

	if (IS_OK(lidar_drv->grabScanData(nodes, cnt, 0)))
    {
		float frequency = 0;
        bool  is4kmode = false;
		lidar_drv->getFrequency(inExpressMode, cnt, frequency, is4kmode);
		m_scanview.setScanData(nodes, cnt, frequency, is4kmode);
    }
}

void    CMainFrame::onUpdateTitle()
{
    char titleMsg[200];
    const char * workingmodeDesc;
    switch (workingMode) {
    case WORKING_MODE_IDLE:
        workingmodeDesc = "IDLE";
        break;
    case WORKING_MODE_SCAN:
        workingmodeDesc = "SCAN";
		m_scanview.ClearReadData();
		m_scanview.setSwitchViewModeScan();
		break;
	case WORKING_MODE_VIEW:
		workingmodeDesc = "VIEW";
		m_scanview.setSwitchViewModeSplit();
        break;
    default:
        assert(!"should not come here");
    }

	/*
	//タイトルバー
    sprintf(titleMsg, "[%s] Model: %d FW: %d.%02d HW: %d Serial: "
        , workingmodeDesc
        , devInfo.model
        , devInfo.firmware_version>>8
        , devInfo.firmware_version & 0xFF, devInfo.hardware_version);
	*/
	sprintf(titleMsg, "人流計測システム");
    
	/*
	//タイトルバー
    for (int pos = 0, startpos = strlen(titleMsg); pos < sizeof(devInfo.serialnum); ++pos)
    {
        sprintf(&titleMsg[startpos], "%02X", devInfo.serialnum[pos]); 
        startpos+=2;
    }
	*/

    this->SetWindowTextA(titleMsg);
}

void    CMainFrame::onSwitchMode(int newMode)
{
    
    // switch mode
    if (newMode == workingMode) return;


    switch (newMode) {
    case WORKING_MODE_IDLE:
        {
            // stop the previous operation
            LidarMgr::GetInstance().lidar_drv->stop();
            LidarMgr::GetInstance().lidar_drv->stopMotor();

            UISetCheck(ID_CMD_STOP, 1);
            UISetCheck(ID_CMD_GRAB_PEAK, 0);
            UISetCheck(ID_CMD_GRAB_FRAME, 0);
            UISetCheck(ID_CMD_SCAN, 0);
            UISetCheck(ID_CMD_GRABFRAMENONEDIFF, 0);
        }
        break;
    case WORKING_MODE_SCAN:
        {
            CWindow  hwnd = m_hWndClient;
            hwnd.ShowWindow(SW_HIDE);
            m_hWndClient = m_scanview;
            m_scanview.ShowWindow(SW_SHOW);
            checkDeviceHealth();
            if (useExpressMode) {
                LidarMgr::GetInstance().lidar_drv->checkExpressScanSupported(inExpressMode);
            } else {
                inExpressMode = false;
            }
            LidarMgr::GetInstance().lidar_drv->startMotor();
            LidarMgr::GetInstance().lidar_drv->startScan(forcescan, inExpressMode);
            UISetCheck(ID_CMD_STOP, 0);
            UISetCheck(ID_CMD_GRAB_PEAK, 0);
            UISetCheck(ID_CMD_GRAB_FRAME, 0);
            UISetCheck(ID_CMD_SCAN, 1);
            UISetCheck(ID_CMD_GRABFRAMENONEDIFF, 0);
        }
        break;
	case WORKING_MODE_VIEW:
		{
			LidarMgr::GetInstance().lidar_drv->stop();
			LidarMgr::GetInstance().lidar_drv->stopMotor();

			UISetCheck(ID_CMD_STOP, 1);
			UISetCheck(ID_CMD_GRAB_PEAK, 0);
			UISetCheck(ID_CMD_GRAB_FRAME, 0);
			UISetCheck(ID_CMD_SCAN, 0);
			UISetCheck(ID_CMD_GRABFRAMENONEDIFF, 0);
			
		}
		break;
    default:
        assert(!"unsupported mode");
    }
    
    UpdateLayout();
    workingMode = newMode;
    onUpdateTitle();
}

void    CMainFrame::checkDeviceHealth()
{
    int errorcode;
    if (!LidarMgr::GetInstance().checkDeviceHealth(&errorcode)){
        char msg[200];
        sprintf(msg, "The device is in unhealthy status.\n"
                   "You need to reset it.\n"
                   "Errorcode: 0x%08x", errorcode);
        
        MessageBox(msg, "Warning", MB_OK);

    }
}
LRESULT CMainFrame::OnSetFreq(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (!support_motor_ctrl) {
        MessageBox("The device is not supported to set frequency.\n", "Warning", MB_OK);
    } else {
	    CFreqSetDlg dlg;
		dlg.DoModal();
    }
	return 0;
}

LRESULT CMainFrame::OnSetLogTerm(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	LogTermSetDlg dlg(&m_scanview);
	dlg.DoModal();

	return 0;
}

