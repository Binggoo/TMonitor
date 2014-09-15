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
	m_nSlotCount = 24;
	m_pMachine = NULL;
	m_nColumn = 4;

	m_Devices = NULL;
}

CPageDevice::~CPageDevice()
{
	if (m_Devices != NULL)
	{
		for (int i = 0; i < m_nSlotCount;i++)
		{
			m_Devices[i].DestroyWindow();
		}

		delete []m_Devices;
		m_Devices = NULL;
	}
}

void CPageDevice::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageDevice, CDialogEx)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_INIT_DEVICE, &CPageDevice::OnInitDevice)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_CHANGE_SLOT_COUNT, &CPageDevice::OnChangeSlotCount)
END_MESSAGE_MAP()


// CPageDevice 消息处理程序


BOOL CPageDevice::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	ASSERT(m_pMachine);

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


	IntialSingDevices();

	// 设置字体颜色
	static CFont font;
	font.CreatePointFont(120,_T("Arial"));

	GetDlgItem(IDC_TEXT_FUNCTION)->SetFont(&font);
	GetDlgItem(IDC_TEXT_PERCENT)->SetFont(&font);
	GetDlgItem(IDC_TEXT_SPEED)->SetFont(&font);
	GetDlgItem(IDC_TEXT_START_TIME)->SetFont(&font);
	GetDlgItem(IDC_TEXT_END_TIME)->SetFont(&font);
	GetDlgItem(IDC_GROUP_STATISTICS)->SetFont(&font);
	GetDlgItem(IDC_TEXT_TOTAL)->SetFont(&font);
	GetDlgItem(IDC_TEXT_EMPTY)->SetFont(&font);
	GetDlgItem(IDC_TEXT_ACTIVE)->SetFont(&font);
	GetDlgItem(IDC_TEXT_PASS)->SetFont(&font);
	GetDlgItem(IDC_TEXT_FAIL)->SetFont(&font);
	GetDlgItem(IDC_TEXT_SPEND_TIME)->SetFont(&font);
	

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

	CWnd *pWnd = NULL;
	pWnd = GetWindow(GW_CHILD);
	while (pWnd)
	{
		ChangeSize(pWnd,cx,cy);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}

	GetClientRect(&m_rect);

}

void CPageDevice::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString strText;

	CByteArray byteArray;
	m_pMachine->GetSlotNum(byteArray);
	int nSlots = m_pMachine->GetSlotCount();

	// Speed Percent
	CString strSpeed,strPercent;

	strText.LoadString(IDS_FORMAT_SPEED);

	if (m_pMachine->GetAvgSpeed() > 0)
	{
		strSpeed.Format(strText,m_pMachine->GetAvgSpeed());
		SetDlgItemText(IDC_TEXT_SPEED,strSpeed);
	}

	strText.LoadString(IDS_FORMAT_PERCENT);

	if (m_pMachine->GetAvgPercent() > 0)
	{
		strPercent.Format(strText,m_pMachine->GetAvgPercent());
		SetDlgItemText(IDC_TEXT_PERCENT,strPercent);
	}

	int nFail = 0;
	int nPass = 0;
	int nActive = 0;
	CString strActive,strPass,strFail;
	UINT nBitmap;

	if (m_pMachine->IsRunning())
	{
		int nSlowestSlot = m_pMachine->GetSlowestSlot();

		for (int i = 0; i < nSlots;i++)
		{
			int nSlot =  byteArray.GetAt(i);

			SLOT_INFO slotInfo = m_pMachine->GetSlotInfo(nSlot);

			if (slotInfo.bResult)
			{
				nActive++;

				
				if (nSlot == nSlowestSlot)
				{
					nBitmap = IDB_SD_YELLOW;
				}
				else
				{
					nBitmap = IDB_SD_NORMAL;
				}

				
			}
			else
			{
				nFail++;
				nBitmap = IDB_SD_RED;
			}

			m_Devices[nSlot].SetDeviceState(nBitmap,(ULONGLONG)slotInfo.ulCapacityMB * 1024 * 1024,slotInfo.dbSpeed,slotInfo.nPercent);
			
		}


	}
	else
	{
		KillTimer(nIDEvent);

		for (int i = 0; i < nSlots;i++)
		{
			int nSlot =  byteArray.GetAt(i);
			

			SLOT_INFO slotInfo = m_pMachine->GetSlotInfo(nSlot);

			if (slotInfo.bResult)
			{
				nBitmap = IDB_SD_GREEN;
				nPass++;
			}
			else
			{
				nBitmap = IDB_SD_RED;
				nFail++;
			}

			m_Devices[nSlot].SetDeviceState(nBitmap,(ULONGLONG)slotInfo.ulCapacityMB * 1024 * 1024,slotInfo.dbSpeed,slotInfo.nPercent);
		}

		CString strEndTime,strSpendTime;

		strText.LoadString(IDS_FORMAT_ENDTIME);
		strEndTime.Format(strText,m_pMachine->GetEndTime().Format(_T("%H:%M:%S")));

		CTimeSpan span = m_pMachine->GetEndTime() - m_pMachine->GetStartTime();
		strText.LoadString(IDS_FORMAT_SPENDTIME);
		strSpendTime.Format(strText,span.Format(_T("%H:%M:%S")));

		SetDlgItemText(IDC_TEXT_END_TIME,strEndTime);
		SetDlgItemText(IDC_TEXT_SPEND_TIME,strSpendTime);
	}

	strText.LoadString(IDS_FORMAT_ACTIVE);
	strActive.Format(strText,nActive);

	strText.LoadString(IDS_FORMAT_PASS);
	strPass.Format(strText,nPass);

	strText.LoadString(IDS_FORMAT_FAIL);
	strFail.Format(strText,nFail);


	SetDlgItemText(IDC_TEXT_ACTIVE,strActive);
	SetDlgItemText(IDC_TEXT_PASS,strPass);
	SetDlgItemText(IDC_TEXT_FAIL,strFail);

	CDialogEx::OnTimer(nIDEvent);
}

void CPageDevice::SetMachineInfo( CMachineInfo *pMachine )
{
	m_pMachine = pMachine;
}


afx_msg LRESULT CPageDevice::OnInitDevice(WPARAM wParam, LPARAM lParam)
{
	CString strText;

	KillTimer(1);
	SetTimer(1,1000,NULL);

	CString strFunction,strStartTime,strEndTime,strSpendTime,strSpeed,strPercent;
	strText.LoadString(IDS_FORMAT_FUNCTION);
	strFunction.Format(strText,m_pMachine->GetFunction());

	strText.LoadString(IDS_FORMAT_STARTTIME);
	strStartTime.Format(strText,m_pMachine->GetStartTime().Format(_T("%H:%M:%S")));

	strText.LoadString(IDS_FORMAT_ENDTIME);
	strEndTime.Format(strText,_T(""));

	strText.LoadString(IDS_FORMAT_SPENDTIME);
	strSpendTime.Format(strText,_T(""));

	strText.LoadString(IDS_FORMAT_SPEED);
	strSpeed.Format(strText,0.0);

	strText.LoadString(IDS_FORMAT_PERCENT);
	strPercent.Format(strText,0);

	SetDlgItemText(IDC_TEXT_FUNCTION,strFunction);
	SetDlgItemText(IDC_TEXT_START_TIME,strStartTime);
	SetDlgItemText(IDC_TEXT_END_TIME,strEndTime);
	SetDlgItemText(IDC_TEXT_SPEND_TIME,strSpendTime);
	SetDlgItemText(IDC_TEXT_SPEED,strSpeed);
	SetDlgItemText(IDC_TEXT_PERCENT,strPercent);

	CString strTotal,strEmpty,strActive,strPass,strFail;

	int nSlotsNums = m_pMachine->GetSlotCount();

	strText.LoadString(IDS_FORMAT_TOTAL);
	strTotal.Format(strText,m_nSlotCount);

	strText.LoadString(IDS_FORMAT_EMPTY);
	strEmpty.Format(strText,m_nSlotCount - nSlotsNums);

	strText.LoadString(IDS_FORMAT_ACTIVE);
	strActive.Format(strText,nSlotsNums);

	strText.LoadString(IDS_FORMAT_PASS);
	strPass.Format(strText,0);

	strText.LoadString(IDS_FORMAT_FAIL);
	strFail.Format(strText,0);

	SetDlgItemText(IDC_TEXT_TOTAL,strTotal);
	SetDlgItemText(IDC_TEXT_EMPTY,strEmpty);
	SetDlgItemText(IDC_TEXT_ACTIVE,strActive);
	SetDlgItemText(IDC_TEXT_PASS,strPass);
	SetDlgItemText(IDC_TEXT_FAIL,strFail);

	return 0;
}


BOOL CPageDevice::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


HBRUSH CPageDevice::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetTextColor(RGB(0, 191, 255));
	}

	if (pWnd->GetDlgCtrlID() == IDC_TEXT_FAIL)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}

	if (pWnd->GetDlgCtrlID() == IDC_TEXT_PASS)
	{
		pDC->SetTextColor(RGB(0, 139, 0));
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


afx_msg LRESULT CPageDevice::OnChangeSlotCount(WPARAM wParam, LPARAM lParam)
{
	int nSlotCount = (UINT)wParam;

	if (nSlotCount != m_nSlotCount)
	{
		if (m_Devices != NULL)
		{
			for (int i = 0; i < m_nSlotCount;i++)
			{
				m_Devices[i].DestroyWindow();
			}

			delete []m_Devices;
			m_Devices = NULL;
		}

		m_nSlotCount = nSlotCount;
	}

	if (m_nSlotCount <= 24)
	{
		m_nColumn = 4;
	}
	else
	{
		m_nColumn = 8;	
	}

	IntialSingDevices();
	
	return 0;
}

void CPageDevice::IntialSingDevices()
{
	m_Devices = new CSingDevice[m_nSlotCount];

	CRect rectFrame;
	GetDlgItem(IDC_DEVICE_FRAME)->GetWindowRect(&rectFrame);
	ScreenToClient(&rectFrame);

	int rows = m_nSlotCount / m_nColumn;

	if (m_nSlotCount % m_nColumn)
	{
		rows++;
	}

	int nWidth = (rectFrame.Width()-2) / m_nColumn;
	int nHeight = (rectFrame.Height()-2) / rows;

	for (int i = 0; i < m_nSlotCount;i++)
	{
		int nRow = i / m_nColumn;
		int nCol = i % m_nColumn;

		m_Devices[i].SetDevicePort(i);

		m_Devices[i].Create(IDD_DIALOG_SING_DEVICE,this);

		m_Devices[i].MoveWindow(rectFrame.left + 1 + nWidth * nCol,
			rectFrame.top + 1 + nHeight * nRow,
			nWidth,
			nHeight);

		m_Devices[i].ShowWindow(SW_SHOW);
	}
}

void CPageDevice::ChangeDeviceStatus( int nSlot,UINT nBitmap )
{
	m_Devices[nSlot].SetBitmap(nBitmap);
}

void CPageDevice::Reset()
{
	for (int i = 0; i < m_nSlotCount;i++)
	{
		m_Devices[i].Intial();
	}
}
