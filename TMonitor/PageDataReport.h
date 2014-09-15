#pragma once
#include "afxdtctl.h"
#include "afxcmn.h"
#include "SlotData.h"

#include "..\CGridListCtrlEx\CGridListCtrlEx.h"

// CPageDataReport 对话框

class CPageDataReport : public CDialogEx
{
	DECLARE_DYNAMIC(CPageDataReport)

public:
	CPageDataReport(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageDataReport();

// 对话框数据
	enum { IDD = IDD_DIALOG_DATA_REPORT };

	void SetMySlotData(CMySlotData *pMySlotData);
	void EnableControls(BOOL bEnable);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CDateTimeCtrl m_StartTimeCtrl;
	CRect         m_rect;
	CDateTimeCtrl m_EndTimeCtrl;
	CString m_strEditSlot;
	CString m_strEditSN;
	CString m_strEditFunction;
	CString m_strEditResult;
	CString m_strEditMachineID;
	CString m_strEditAlias;
	CGridListCtrlEx m_ListCtrlQuery;
	BOOL      m_bEnableDel;

	CMySlotData *m_pMySlotData;

	enum
	{
		COL_ID = 0,
		COL_SLOT,
		COL_TYPE,
		COL_MACHINE_ID,
		COL_ALIAS,
		COL_SN,
		COL_SIZE,
		COL_FUNCTION,
		COL_START_TIME,
		COL_END_TIME,
		COL_SPEED,
		COL_RESULT
	};

	void ChangeSize(CWnd *pWnd,int cx,int cy);
	void InitialListCtrl();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnBnClickedButtonQueryExport();
	afx_msg void OnRightclickDeleteSelected();
	afx_msg void OnNMRClickListQuery(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRightclickDeleteAll();
	afx_msg void OnRightclickCleanTable();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonClear();
};
