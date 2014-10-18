#pragma once
#include "afxdtctl.h"
#include "afxcmn.h"
#include "SlotData.h"

// CPageExportSN 对话框

class CPageExportSN : public CDialogEx
{
	DECLARE_DYNAMIC(CPageExportSN)

public:
	CPageExportSN(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageExportSN();

// 对话框数据
	enum { IDD = IDD_DIALOG_EXPORT_SN };

	void SetMySlotData(CMySlotData *pMySlotData);
	void EnableControls(BOOL bEnable);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CDateTimeCtrl m_StartTimeCtrl;
	CDateTimeCtrl m_EndTimeCtrl;
	CString m_strEditSN;
	CListCtrl m_ListSN;

	CRect m_rect;
	CMySlotData *m_pMySlotData;

	void ChangeSize(CWnd *pWnd,int cx,int cy,DWORD flag);
	void InitialListCtrl();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnBnClickedButtonExportSn();
private:
	CString m_strEditAlias;
	CString m_strEditMachineID;
public:
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonClear();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
