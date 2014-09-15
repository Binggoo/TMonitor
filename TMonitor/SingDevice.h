#pragma once
#include "ProgressCtrlEx.h"

// CSingDevice �Ի���

class CSingDevice : public CDialogEx
{
	DECLARE_DYNAMIC(CSingDevice)

public:
	CSingDevice(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSingDevice();

// �Ի�������
	enum { IDD = IDD_DIALOG_SING_DEVICE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void SetDevicePort(int nPortNum);
	void Intial();
	void SetDeviceState(UINT nBitmap,ULONGLONG ullCapacity,double dbSpeed,int percent);
	void SetBitmap(UINT nBitmap);

private:
	CStatic m_PicDevice;
	CProgressCtrlEx m_Progress;
	CBitmap m_Bitmap;
	CRect   m_Rect;

	int m_nPortNum;
	UINT m_nBitmap;

	void ChangeSize( CWnd *pWnd,int cx, int cy );
	
};
