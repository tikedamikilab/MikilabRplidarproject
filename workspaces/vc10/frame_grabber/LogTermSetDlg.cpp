#include "stdafx.h"
#include "resource.h"
#include "drvlogic\lidarmgr.h"
#include "LogTermSetDlg.h"
//#include "scanView.h"

LogTermSetDlg::LogTermSetDlg(){}
LogTermSetDlg::LogTermSetDlg(CScanView* argpt)
{
	scanviewpt = argpt;
	timestart = (*scanviewpt).getReadStartTime();
	timeallstart = (*scanviewpt).getReadStartnow();
	if (timestart == 0) {
		timestart = (*scanviewpt).getReadStartnow();
	}
	timeend = (*scanviewpt).getReadEndTime();
	timeallend = (*scanviewpt).getReadEndnow();
	if (timeend == 0) {
		timeend = (*scanviewpt).getReadEndnow();
	}
	magnification = (*scanviewpt).getMagnification();
	sizesplitdot = (*scanviewpt).getSizeSplitViewDot();
	colormode = (*scanviewpt).getColorMode();
}

LRESULT LogTermSetDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	this->DoDataExchange();
	
	//start
	CString ststr;
	start = std::localtime(&timestart);
	ststr.Format("%d", start->tm_mon);
	m_stedt[0].SetWindowTextA(ststr);
	ststr.Format("%d", start->tm_mday);
	m_stedt[1].SetWindowTextA(ststr);
	ststr.Format("%d", start->tm_hour);
	m_stedt[2].SetWindowTextA(ststr);
	ststr.Format("%d", start->tm_min);
	m_stedt[3].SetWindowTextA(ststr);
	ststr.Format("%d", start->tm_sec);
	m_stedt[4].SetWindowTextA(ststr);
	//end
	CString edstr;
	end = std::localtime(&timeend);
	edstr.Format("%d", end->tm_mon);
	m_ededt[0].SetWindowTextA(edstr);
	edstr.Format("%d", end->tm_mday);
	m_ededt[1].SetWindowTextA(edstr);
	edstr.Format("%d", end->tm_hour);
	m_ededt[2].SetWindowTextA(edstr);
	edstr.Format("%d", end->tm_min);
	m_ededt[3].SetWindowTextA(edstr);
	edstr.Format("%d", end->tm_sec);
	m_ededt[4].SetWindowTextA(edstr);

	CString magstr;
	magstr.Format("%d", magnification);
	m_magnification.SetWindowTextA(magstr);
	CString sizestr;
	sizestr.Format("%d", sizesplitdot);
	m_sizesplitdot.SetWindowTextA(sizestr);
	CString colorstr;
	colorstr.Format("%d", colormode);
	m_colormode.SetWindowTextA(colorstr);

	return TRUE;
}
LRESULT LogTermSetDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT  LogTermSetDlg::OnBnClickedStartSet(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	char stdata[10];
	struct tm ststm;
	ststm.tm_year = start->tm_year;
	m_stedt[0].GetWindowTextA(stdata, _countof(stdata));
	ststm.tm_mon = atoi(stdata);
	m_stedt[1].GetWindowTextA(stdata, _countof(stdata));
	ststm.tm_mday = atoi(stdata);
	m_stedt[2].GetWindowTextA(stdata, _countof(stdata));
	ststm.tm_hour = atoi(stdata);
	m_stedt[3].GetWindowTextA(stdata, _countof(stdata));
	ststm.tm_min = atoi(stdata);
	m_stedt[4].GetWindowTextA(stdata, _countof(stdata));
	ststm.tm_sec = atoi(stdata);
	time_t sttimeset = mktime(&ststm);
	if (timeallstart < sttimeset) {
		(*scanviewpt).setReadStartTime(sttimeset);
	}
	else{
		(*scanviewpt).setReadStartTime(timestart);
	}
	char eddata[10];
	struct tm edstm;
	edstm.tm_year = end->tm_year;
	m_ededt[0].GetWindowTextA(eddata, _countof(eddata));
	edstm.tm_mon = atoi(eddata);
	m_ededt[1].GetWindowTextA(eddata, _countof(eddata));
	edstm.tm_mday = atoi(eddata);
	m_ededt[2].GetWindowTextA(eddata, _countof(eddata));
	edstm.tm_hour = atoi(eddata);
	m_ededt[3].GetWindowTextA(eddata, _countof(eddata));
	edstm.tm_min = atoi(eddata);
	m_ededt[4].GetWindowTextA(eddata, _countof(eddata));
	edstm.tm_sec = atoi(eddata);

	time_t edtimeset = mktime(&edstm);
	if ( sttimeset < edtimeset && edtimeset < timeallend) {
		(*scanviewpt).setReadEndTime(edtimeset);
	}
	else {
		(*scanviewpt).setReadEndTime(timeend);
	}
	
	char magdata[10];
	m_magnification.GetWindowTextA(magdata, _countof(magdata));
	int mag = atoi(magdata);
	(*scanviewpt).setMagnification(mag);
	(*scanviewpt).setSwitchViewModeSplit();

	char sizedata[10];
	m_sizesplitdot.GetWindowTextA(sizedata, _countof(sizedata));
	int size = atoi(sizedata);
	(*scanviewpt).setSizeSplitViewDot(size);
	
	char colordata[10];
	m_colormode.GetWindowTextA(colordata, _countof(colordata));
	int color = atoi(colordata);
	(*scanviewpt).setColorMode(color);

	EndDialog(wID);
	return 0;
}
LRESULT  LogTermSetDlg::OnBnClickedEndSet(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	char data[10];
	struct tm stm;
	stm.tm_year = end->tm_year;
	m_ededt[0].GetWindowTextA(data, _countof(data));
	stm.tm_mon = atoi(data);
	m_ededt[1].GetWindowTextA(data, _countof(data));
	stm.tm_mday = atoi(data);
	m_ededt[2].GetWindowTextA(data, _countof(data));
	stm.tm_hour = atoi(data);
	m_ededt[3].GetWindowTextA(data, _countof(data));
	stm.tm_min = atoi(data);
	m_ededt[4].GetWindowTextA(data, _countof(data));
	stm.tm_sec = atoi(data);

	time_t timeset = mktime(&stm);
	if (timeset < timeend) {
		(*scanviewpt).setReadEndTime(timeset);
	}
	else {
		(*scanviewpt).setReadEndTime(timeend);
	}
	EndDialog(wID);
	return 0;
}