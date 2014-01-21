#pragma once
#include "afxcmn.h"


// CPageDevice �Ի���
#define PER_BOARD_SLOTS 4

class CPageDevice : public CDialogEx
{
	DECLARE_DYNAMIC(CPageDevice)

public:
	CPageDevice(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageDevice();

// �Ի�������
	enum { IDD = IDD_DIALOG_DEVICE_STATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	CImageList m_ImageList;
	CListCtrl  m_ListCtrl;
	CRect      m_rect;
	UINT       m_nBoardNum;

	enum
	{
		SD_EMPTY = 0,
		SD_RED,
		SD_GREEN
	};

	void ChangeSize(CWnd *pWnd,int cx,int cy);

public:
	virtual BOOL OnInitDialog();

	void SetDeviceNumber(UINT nSlotNum);

	void ChangeDeviceStatus();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
