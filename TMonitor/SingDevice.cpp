// SingDevice.cpp : 实现文件
//

#include "stdafx.h"
#include "TMonitor.h"
#include "SingDevice.h"
#include "afxdialogex.h"
#include "Utils.h"


// CSingDevice 对话框

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


// CSingDevice 消息处理程序


BOOL CSingDevice::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
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
	// 异常: OCX 属性页应返回 FALSE
}

void CSingDevice::ChangeSize( CWnd *pWnd,int cx, int cy)
{
	if (pWnd)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
	
		//rect.left=rect.left*cx/m_Rect.Width();
		rect.right=rect.right*cx/m_Rect.Width();
		rect.top=rect.top*cy/m_Rect.Height();
		rect.bottom=rect.bottom*cy/m_Rect.Height();

		pWnd->MoveWindow(rect);
	}
}


void CSingDevice::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CWnd *pWnd = GetWindow(GW_CHILD);
	while (pWnd)
	{
		if (pWnd->GetDlgCtrlID() != IDC_PIC_DEVICE)
		{
			ChangeSize(pWnd,cx,cy);
		}
		
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
	SetDlgItemText(IDC_TEXT_SN,_T(""));

	m_Progress.SetPos(0);

	Invalidate(TRUE);
}

void CSingDevice::SetDeviceState( UINT nBitmap,ULONGLONG ullCapacity,double dbSpeed,int percent,CString strSN )
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

	SetDlgItemText(IDC_TEXT_SN,strSN);
}

void CSingDevice::SetBitmap( UINT nBitmap ,CString strSN,ULONGLONG ullCapacity)
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

	SetDlgItemText(IDC_TEXT_SPEED,_T(""));

	if (ullCapacity == 0)
	{
		SetDlgItemText(IDC_TEXT_SIZE,_T(""));
	}
	else
	{
		SetDlgItemText(IDC_TEXT_SIZE,CUtils::AdjustFileSize(ullCapacity));
	}


	SetDlgItemText(IDC_TEXT_SN,strSN);

	m_Progress.SetPos(0);
}

