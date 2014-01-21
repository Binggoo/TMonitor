// PageDevice.cpp : 实现文件
//

#include "stdafx.h"
#include "TMonitor.h"
#include "PageDevice.h"
#include "afxdialogex.h"


// CPageDevice 对话框

IMPLEMENT_DYNAMIC(CPageDevice, CDialogEx)

CPageDevice::CPageDevice(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPageDevice::IDD, pParent)
{
	m_nBoardNum = 6;
}

CPageDevice::~CPageDevice()
{
}

void CPageDevice::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEVICE_STATUS, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CPageDevice, CDialogEx)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPageDevice 消息处理程序


BOOL CPageDevice::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//获取对话框原始大小
	GetClientRect(&m_rect);

	CRect rectClient;
	this->GetOwner()->GetClientRect(&rectClient);
	CTabCtrl *pTab = (CTabCtrl *)this->GetOwner();

	CRect rcHead;
	pTab->GetItemRect(0,&rcHead);
	rectClient.top += rcHead.Height() + 2;
	rectClient.left += 2;
	rectClient.right -= 2;
	rectClient.bottom -= 2;
	MoveWindow(rectClient);

	m_ImageList.Create(64,64,ILC_COLOR32 | ILC_MASK,1,0);

	for (int i = 0;i < 3;i++)
	{
		m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_SD_EMPTY+i));
	}
	m_ListCtrl.SetImageList(&m_ImageList,LVSIL_NORMAL);

	m_ListCtrl.SetIconSpacing(80,90);

	//初始化ListCtrl
	m_ListCtrl.InsertItem(0,_T("Master"),SD_EMPTY);
	for (UINT i = 1;i < m_nBoardNum * PER_BOARD_SLOTS;i++)
	{
		CString strText;
		strText.Format(_T("SD-%d"),i);
		m_ListCtrl.InsertItem(i,strText,SD_EMPTY);
		
	}

	// 初始化Slot状态
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPageDevice::ChangeSize( CWnd *pWnd,int cx,int cy )
{
	if(pWnd)  //判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建   
	{  
		CRect rect;   //获取控件变化前的大小    
		pWnd->GetWindowRect(&rect);  
		ScreenToClient(&rect);//将控件大小转换为在对话框中的区域坐标  

		//    cx/m_rect.Width()为对话框在横向的变化比例  
		rect.left=rect.left*cx/m_rect.Width();//调整控件大小  
		rect.right=rect.right*cx/m_rect.Width();  
		rect.top=rect.top*cy/m_rect.Height();  
		rect.bottom=rect.bottom*cy/m_rect.Height();  
		pWnd->MoveWindow(rect);//设置控件大小  
	}  
}


void CPageDevice::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (nType == SIZE_MINIMIZED)
	{
		return;
	}
	CWnd *pWnd[9];

	pWnd[0] = GetDlgItem(IDC_LIST_DEVICE_STATUS);
	pWnd[1] = GetDlgItem(IDC_TEXT_MODE);
	pWnd[2] = GetDlgItem(IDC_TEXT_STATE);
	pWnd[3] = GetDlgItem(IDC_TEXT_TOTAL);
	pWnd[4] = GetDlgItem(IDC_TEXT_EMPTY);
	pWnd[5] = GetDlgItem(IDC_TEXT_ACTIVE);
	pWnd[6] = GetDlgItem(IDC_TEXT_PASS);
	pWnd[7] = GetDlgItem(IDC_TEXT_FAIL);
	pWnd[8] =GetDlgItem(IDC_GROUP_STATISTICS);

	for (int i = 0;i<9;i++)
	{
		ChangeSize(pWnd[i],cx,cy);
	}

	GetClientRect(&m_rect);



}

void CPageDevice::SetDeviceNumber( UINT nDeviceNum )
{
	m_nBoardNum = nDeviceNum;
}


void CPageDevice::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);
}
