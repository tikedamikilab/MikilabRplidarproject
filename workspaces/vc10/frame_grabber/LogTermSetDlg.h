#pragma once
#include "scanView.h"

class LogTermSetDlg : public CDialogImpl<LogTermSetDlg>,
	public CWinDataExchange<LogTermSetDlg>
{
public:
	LogTermSetDlg();
	LogTermSetDlg(CScanView* argpt);
	enum { IDD = IDD_DLG_SETLOGTERM };

	BEGIN_MSG_MAP(LogTermSetDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_STEDIT_SET, BN_CLICKED, OnBnClickedStartSet)
		COMMAND_HANDLER(IDC_EDEDIT_SET, BN_CLICKED, OnBnClickedEndSet)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()


	BEGIN_DDX_MAP(LogTermSetDlg)
		DDX_CONTROL_HANDLE(IDC_STEDIT_MONTH, m_stedt[0])
		DDX_CONTROL_HANDLE(IDC_STEDIT_DAY  , m_stedt[1])
		DDX_CONTROL_HANDLE(IDC_STEDIT_HOUR , m_stedt[2])
		DDX_CONTROL_HANDLE(IDC_STEDIT_MIN  , m_stedt[3])
		DDX_CONTROL_HANDLE(IDC_STEDIT_SEC  , m_stedt[4])
		DDX_CONTROL_HANDLE(IDC_EDEDIT_MONTH, m_ededt[0])
		DDX_CONTROL_HANDLE(IDC_EDEDIT_DAY  , m_ededt[1])
		DDX_CONTROL_HANDLE(IDC_EDEDIT_HOUR , m_ededt[2])
		DDX_CONTROL_HANDLE(IDC_EDEDIT_MIN  , m_ededt[3])
		DDX_CONTROL_HANDLE(IDC_EDEDIT_SEC  , m_ededt[4])
		DDX_CONTROL_HANDLE(IDC_MAGNIFICATION_EDIT  , m_magnification)
		DDX_CONTROL_HANDLE(IDC_SIZESPLITDOT, m_sizesplitdot)
		DDX_CONTROL_HANDLE(IDC_COLORMODE, m_colormode)
	END_DDX_MAP();

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedStartSet(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedEndSet(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	CEdit           m_stedt[5];
	CEdit           m_ededt[5];
	CEdit           m_magnification;
	CEdit           m_sizesplitdot;
	CEdit           m_colormode;
protected: 
	CScanView * scanviewpt;
	std::tm* start;
	std::tm* end;
	time_t timeallstart;
	time_t timeallend;
	time_t timestart;
	time_t timeend;
	int magnification;
	int sizesplitdot;
	int colormode;
};