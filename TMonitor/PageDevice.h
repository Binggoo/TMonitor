#pragma once
#include "afxcmn.h"
#include "MachineInfo.h"

// CPageDevice 对话框

typedef enum _ENUM_SLOT_COLOR
{
	SD_EMPTY = 0,
	SD_RED,
	SD_GREEN,
	SD_YELLOW
}SLOT_COLOR;

class CPageDevice : public CDialogEx
{
	DECLARE_DYNAMIC(CPageDevice)

public:
	CPageDevice(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageDevice();

// 对话框数据
	enum { IDD = IDD_DIALOG_DEVICE_STATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CImageList *m_pImageList;
	CListCtrl  m_ListCtrl;
	CRect      m_rect;
	UINT       m_nSlotCount;
	UINT       m_nColumn;
	BOOL       m_bMaximied;

	CMachineInfo *m_pMachine;

	void ChangeSize(CWnd *pWnd,int cx,int cy);
	void AdjustListCtrlSpace();

public:
	virtual BOOL OnInitDialog();

	void ChangeDeviceStatus(int iSlot,SLOT_COLOR color);

	void SetMachineInfo(CMachineInfo *pMachine);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	afx_msg LRESULT OnInitDevice(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
	afx_msg LRESULT OnChangeSlotCount(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeIconSize(WPARAM wParam, LPARAM lParam);
};
