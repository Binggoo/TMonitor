// SingDevice.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TMonitor.h"
#include "SingDevice.h"
#include "afxdialogex.h"
#include "Utils.h"


// CSingDevice �Ի���

IMPLEMENT_DYNAMIC(CSingDevice, CDialogEx)

CSingDevice::CSingDevice(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSingDevice::IDD, pParent)
{
	m_nPortNum = 0;
}

CSingDevice::~CSingDevice()
{
}

void CSingDevice::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_DEVICE, m_PicDevice);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}


BEGIN_MESSAGE_MAP(CSingDevice, CDialogEx)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSingDevice ��Ϣ��������


BOOL CSingDevice::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ����Ӷ���ĳ�ʼ��
	CString strPortName;
	if (m_nPortNum == 0)
	{
		strPortName = _T("Master");
	}
	else
	{
		strPortName.Format(_T("T-%d"),m_nPortNum);		
	}

	SetDlgItemText(IDC_GROUP_DEVICE,strPortName);

	m_Progress.ShowPercent(FALSE);
	m_Progress.SetRange32(0,100);

	Intial();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CSingDevice::ChangeSize( CWnd *pWnd,int cx, int cy )
{
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.left=rect.left*cx/m_Rect.Width();
		rect.right=rect.right*cx/m_Rect.Width();
		rect.top=rect.top*cy/m_Rect.Height();
		rect.bottom=rect.bottom*cy/m_Rect.Height();
		pWnd->MoveWindow(rect);
	}
}


void CSingDevice::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴�������Ϣ�����������
	CWnd *pWnd = GetWindow(GW_CHILD);
	while (pWnd)
	{
		ChangeSize(pWnd,cx,cy);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}

	GetClientRect(&m_Rect);
}

void CSingDevice::SetDevicePort( int nPortNum )
{
	m_nPortNum = nPortNum;
}

void CSingDevice::Intial()
{
	if (m_Bitmap.m_hObject != NULL)
	{
		m_Bitmap.DeleteObject();
	}

	m_nBitmap = IDB_SD_GRAY;

	m_Bitmap.LoadBitmap(m_nBitmap);
	m_PicDevice.SetBitmap(m_Bitmap);

	SetDlgItemText(IDC_TEXT_SPEED,_T(""));
	SetDlgItemText(IDC_TEXT_SIZE,_T(""));

	m_Progress.SetPos(0);

	Invalidate(TRUE);
}

void CSingDevice::SetDeviceState( UINT nBitmap,ULONGLONG ullCapacity,double dbSpeed,int percent )
{
	if (m_nBitmap != nBitmap)
	{
		if (m_Bitmap.m_hObject != NULL)
		{
			m_Bitmap.DeleteObject();
		}

		m_nBitmap = nBitmap;

		m_Bitmap.LoadBitmap(m_nBitmap);
		m_PicDevice.SetBitmap(m_Bitmap);
	}

	SetDlgItemText(IDC_TEXT_SIZE,CUtils::AdjustFileSize(ullCapacity));
	CString strSpeed;
	strSpeed.Format(_T("%dMB/s"),(int)dbSpeed);
	SetDlgItemText(IDC_TEXT_SPEED,strSpeed);
	m_Progress.SetPos(percent);
}

void CSingDevice::SetBitmap( UINT nBitmap )
{
	if (m_nBitmap != nBitmap)
	{
		if (m_Bitmap.m_hObject != NULL)
		{
			m_Bitmap.DeleteObject();
		}

		m_nBitmap = nBitmap;

		m_Bitmap.LoadBitmap(m_nBitmap);
		m_PicDevice.SetBitmap(m_Bitmap);

		SetDlgItemText(IDC_TEXT_SPEED,_T(""));
		SetDlgItemText(IDC_TEXT_SIZE,_T(""));

		m_Progress.SetPos(0);
	}
}
