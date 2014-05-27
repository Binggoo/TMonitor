#pragma once
#include "afxcmn.h"
#include "MachineInfo.h"
#include "ColorListCtrl.h"

// CPageList �Ի���

class CPageList : public CDialogEx
{
	DECLARE_DYNAMIC(CPageList)

public:
	CPageList(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageList();

// �Ի�������
	enum { IDD = IDD_DIALOG_DEVICE_LIST };

	void EnableControls(BOOL bEnable);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	CColorListCtrl m_ListCtrl;
	CRect      m_rect;
	CMachineInfo *m_pMachine;

	void ChangeSize(CWnd *pWnd,int cx,int cy);
	void InitialListCtrl();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void SetMachineInfo(CMachineInfo *pMachine);
protected:
	afx_msg LRESULT OnInitDevice(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonExport();
protected:
	afx_msg LRESULT OnExportSummaryList(WPARAM wParam, LPARAM lParam);
};
