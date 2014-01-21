// PageDevice.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TMonitor.h"
#include "PageDevice.h"
#include "afxdialogex.h"


// CPageDevice �Ի���

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


// CPageDevice ��Ϣ�������


BOOL CPageDevice::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	//��ȡ�Ի���ԭʼ��С
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

	//��ʼ��ListCtrl
	m_ListCtrl.InsertItem(0,_T("Master"),SD_EMPTY);
	for (UINT i = 1;i < m_nBoardNum * PER_BOARD_SLOTS;i++)
	{
		CString strText;
		strText.Format(_T("SD-%d"),i);
		m_ListCtrl.InsertItem(i,strText,SD_EMPTY);
		
	}

	// ��ʼ��Slot״̬
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CPageDevice::ChangeSize( CWnd *pWnd,int cx,int cy )
{
	if(pWnd)  //�ж��Ƿ�Ϊ�գ���Ϊ�Ի��򴴽�ʱ����ô˺���������ʱ�ؼ���δ����   
	{  
		CRect rect;   //��ȡ�ؼ��仯ǰ�Ĵ�С    
		pWnd->GetWindowRect(&rect);  
		ScreenToClient(&rect);//���ؼ���Сת��Ϊ�ڶԻ����е���������  

		//    cx/m_rect.Width()Ϊ�Ի����ں���ı仯����  
		rect.left=rect.left*cx/m_rect.Width();//�����ؼ���С  
		rect.right=rect.right*cx/m_rect.Width();  
		rect.top=rect.top*cy/m_rect.Height();  
		rect.bottom=rect.bottom*cy/m_rect.Height();  
		pWnd->MoveWindow(rect);//���ÿؼ���С  
	}  
}


void CPageDevice::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnTimer(nIDEvent);
}
