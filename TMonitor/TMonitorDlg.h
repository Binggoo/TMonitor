
// TMonitorDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "PageDevice.h"
#include "PageLog.h"
#include "afxwin.h"
#include "SerialPort.h"

#define  TIMER_TIME 1
#define  PRJ_NAME   _T("PHIYO-ST123-Ver:")
#define  TEMP_FILE  _T("\\temp.log")
#define  DEFAULT_PASSWORD  _T("phiyo")

// CTMonitorDlg dialog
class CTMonitorDlg : public CDialogEx
{
// Construction
public:
	CTMonitorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CTabCtrl    m_tabMain;
	BOOL        m_bCheckAutoSave;
	BOOL        m_bCheckDate;
	BOOL        m_bCheckTime;
	CComboBox   m_comboCom;

	//CPageDevice m_PageDevice;
	CPageLog    m_PageLog;
	//CDialogEx   *m_pDlg[2];
	INT         m_nCurSelTab;

	CBrush      m_brush;
	BOOL        m_bConnected;
	CSerialPort m_SerialPort;
	CString     m_strReadLine;
	CString     m_strAppPath;
	CString     m_strPassWord;
	CRect       m_Rect;

	void EnableAutoSaveCtrl(BOOL bEnable = TRUE);
	void EnableAllSettingCtrl(BOOL bEnable = TRUE);
	void UpdateFileName();
	int  GetSystemPorts(CStringArray &strArrayName, CStringArray &strArrayPort);
	CString * SplitString(CString str, char split, int& iSubStrs);
	void ChangeSize(CWnd *pWnd, int cx, int cy);

public:
	static CString GetAppVersion(CString strPath);
	static CString GetErrorMessage( DWORD dwErrorCode );

public:
	//afx_msg void OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCheckAutoSave();
	afx_msg void OnEnChangeEditPrefix();
	afx_msg void OnBnClickedCheckDate();
	afx_msg void OnBnClickedCheckTime();
	afx_msg void OnBnClickedButtonBrowser();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonSaveAs();
	afx_msg void OnBnClickedButtonLock();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnAbout();
	afx_msg void OnCbnDropdownComboCom();
protected:
	afx_msg LRESULT OnOnComReceive(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
