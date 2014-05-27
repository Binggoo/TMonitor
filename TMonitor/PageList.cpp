// PageList.cpp : 实现文件
//

#include "stdafx.h"
#include "TMonitor.h"
#include "PageList.h"
#include "afxdialogex.h"
#include "MyExcel.h"
#include "Utils.h"


// CPageList 对话框

IMPLEMENT_DYNAMIC(CPageList, CDialogEx)

CPageList::CPageList(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPageList::IDD, pParent)
{
	m_pMachine = NULL;
}

CPageList::~CPageList()
{
}

void CPageList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SLOTS, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CPageList, CDialogEx)
	ON_WM_SIZE()
	ON_MESSAGE(WM_INIT_DEVICE, &CPageList::OnInitDevice)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CPageList::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CPageList::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CPageList::OnBnClickedButtonExport)
	ON_MESSAGE(WM_EXPORT_SUMMARY_LIST, &CPageList::OnExportSummaryList)
END_MESSAGE_MAP()

void CPageList::ChangeSize( CWnd *pWnd,int cx,int cy )
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


// CPageList 消息处理程序


BOOL CPageList::OnInitDialog()
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

	InitialListCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CPageList::OnSize(UINT nType, int cx, int cy)
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

void CPageList::SetMachineInfo( CMachineInfo *pMachine )
{
	m_pMachine = pMachine;
}


afx_msg LRESULT CPageList::OnInitDevice(WPARAM wParam, LPARAM lParam)
{
	KillTimer(1);

	SetTimer(1,1000,NULL);

	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(FALSE);

	Slot_List slotList;
	m_pMachine->GetSlotList(slotList);
	
	int nItem = m_ListCtrl.GetItemCount();

	if (nItem != 0)
	{
		m_ListCtrl.InsertItem(nItem,_T("  "));
		nItem++;
	}

	POSITION pos = slotList.GetHeadPosition();
	while (pos)
	{
		SLOT_INFO slotInfo = slotList.GetNext(pos);
		CString strItem;

		// Slot Num
		strItem.Format(_T("%d"),slotInfo.nSlotNum);
		m_ListCtrl.InsertItem(nItem,strItem);
		
		// Slot Type
		if (slotInfo.slotType == SlotType_SRC)
		{
			strItem = _T("M");
		}
		else
		{
			strItem = _T("T");
		}
		m_ListCtrl.SetItemText(nItem,1,strItem);

		// SN
		strItem = slotInfo.strSN;
		m_ListCtrl.SetItemText(nItem,2,strItem);

		// CapacitySize
		strItem.Format(_T("%ld"),slotInfo.ulCapacityMB);
		m_ListCtrl.SetItemText(nItem,3,strItem);

		// Function
		strItem = m_pMachine->GetFunction();
		m_ListCtrl.SetItemText(nItem,4,strItem);

		// Start Time
		strItem = m_pMachine->GetStartTime().Format(_T("%Y-%m-%d %H:%M:%S"));
		m_ListCtrl.SetItemText(nItem,5,strItem);

		// EndTime
		strItem = _T("---");
		m_ListCtrl.SetItemText(nItem,6,strItem);

		// Speed
		strItem.Format(_T("%.1f"),slotInfo.dbSpeed);
		m_ListCtrl.SetItemText(nItem,7,strItem);

		// Percent
		strItem.Format(_T("%d%%"),slotInfo.nPercent);
		m_ListCtrl.SetItemText(nItem,8,strItem);

		// Result
		strItem = _T("---");
		m_ListCtrl.SetItemText(nItem,9,strItem);

		nItem++;
	}

	int nCount = m_ListCtrl.GetItemCount();
	if (nCount > 0)
	{
		m_ListCtrl.EnsureVisible(nCount-1, FALSE);
	}

	return 0;
}

void CPageList::InitialListCtrl()
{
	CString strText;
	int nItem = 0;

	strText.LoadString(IDS_SLOT);
	m_ListCtrl.InsertColumn(nItem,strText,LVCFMT_LEFT,40);
	nItem++;

	strText.LoadString(IDS_TYPE);
	m_ListCtrl.InsertColumn(nItem,strText,LVCFMT_LEFT,40);
	nItem++;

	strText.LoadString(IDS_SN);
	m_ListCtrl.InsertColumn(nItem,strText,LVCFMT_LEFT,120);
	nItem++;

	strText.LoadString(IDS_SIZE_MB);
	m_ListCtrl.InsertColumn(nItem,strText,LVCFMT_LEFT,80);
	nItem++;

	strText.LoadString(IDS_FUNCTION);
	m_ListCtrl.InsertColumn(nItem,strText,LVCFMT_LEFT,120);
	nItem++;

	strText.LoadString(IDS_START_TIME);
	m_ListCtrl.InsertColumn(nItem,strText,LVCFMT_LEFT,130);
	nItem++;

	strText.LoadString(IDS_END_TIME);
	m_ListCtrl.InsertColumn(nItem,strText,LVCFMT_LEFT,130);
	nItem++;

	strText.LoadString(IDS_SPEED_MB_S);
	m_ListCtrl.InsertColumn(nItem,strText,LVCFMT_LEFT,90);
	nItem++;

	strText.LoadString(IDS_PERCENT);
	m_ListCtrl.InsertColumn(nItem,strText,LVCFMT_LEFT,60);
	nItem++;

	strText.LoadString(IDS_RESULT);
	m_ListCtrl.InsertColumn(nItem,strText,LVCFMT_LEFT,50);
	nItem++;

	m_ListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
}


BOOL CPageList::PreTranslateMessage(MSG* pMsg)
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


void CPageList::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	Slot_List slotList;
	m_pMachine->GetSlotList(slotList);

	POSITION pos = slotList.GetHeadPosition();
	int nItem = m_ListCtrl.GetItemCount() - m_pMachine->GetSlotCount();

	if (m_pMachine->IsRunning())
	{
		while (pos)
		{
			SLOT_INFO slotInfo = slotList.GetNext(pos);
			CString strItem;

			if (slotInfo.bResult)
			{
				// Speed	
				strItem.Format(_T("%.1f MB/s"),slotInfo.dbSpeed);
				m_ListCtrl.SetItemText(nItem,7,strItem);

				// Percent
				strItem.Format(_T("%d%%"),slotInfo.nPercent);
				m_ListCtrl.SetItemText(nItem,8,strItem);
			}
			else
			{
				// Result
				m_ListCtrl.SetItemText(nItem,9,_T("FAIL"));
			}	

			nItem++;
		}
	}
	else
	{
		KillTimer(nIDEvent);
		GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(TRUE);

		while (pos)
		{
			SLOT_INFO slotInfo = slotList.GetNext(pos);
			CString strItem;


			// EndTime
			strItem = m_pMachine->GetEndTime().Format(_T("%Y-%m-%d %H:%M:%S"));
			m_ListCtrl.SetItemText(nItem,6,strItem);

			// Speed	
			strItem.Format(_T("%.1f"),slotInfo.dbSpeed);
			m_ListCtrl.SetItemText(nItem,7,strItem);

			// Percent
			strItem.Format(_T("%d%%"),slotInfo.nPercent);
			m_ListCtrl.SetItemText(nItem,8,strItem);

			if (slotInfo.bResult)
			{
				m_ListCtrl.SetItemText(nItem,9,_T("PASS"));
			}
			else
			{
				// Result
				m_ListCtrl.SetItemText(nItem,9,_T("FAIL"));
			}	

			nItem++;
		}
	}

	// 滚动到最后
	int nCount = m_ListCtrl.GetItemCount();
	if (nCount > 0)
	{
		m_ListCtrl.EnsureVisible(nCount-1, FALSE);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CPageList::OnBnClickedButtonClear()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ListCtrl.DeleteAllItems();
}


void CPageList::OnBnClickedButtonSearch()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ListCtrl.ResetColors();

	CString strSearch;
	GetDlgItemText(IDC_EDIT_SEARCH,strSearch);
	
	strSearch.Trim();

	if (strSearch.IsEmpty())
	{
		return;
	}

	GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);

	strSearch.MakeLower();
	int nRows = m_ListCtrl.GetItemCount();
	int nCols = m_ListCtrl.GetHeaderCtrl()->GetItemCount();

	for (int row = 0;row < nRows;row++ )
	{
		for (int col = 0;col < nCols;col++)
		{
			CString strItem = m_ListCtrl.GetItemText(row,col).MakeLower();

			if (strItem.Find(strSearch) != -1)
			{
				m_ListCtrl.SetCellColor(RGB(255,255,0),row,col);
				m_ListCtrl.EnsureVisible(row,FALSE);
			}
		}
	}

	GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(TRUE);
}


void CPageList::OnBnClickedButtonExport()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strText;
	strText.LoadString(IDS_EXCEL_FILTER);
	CString strFilter = strText;
	CFileDialog dlg(FALSE,_T("xls"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter,this);
	if(dlg.DoModal() != IDOK) 
	{
		return; 
	}

	GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(FALSE);

	CString strStrFile = dlg.GetPathName();  //选择保存路径名称

	if(::PathFileExists(strStrFile)) 
	{
		DeleteFile(strStrFile); 
	}

	CMyExcel myExcel;

	if (!myExcel.Open())
	{
		CString strMsg,strTital;
		strMsg.LoadString(IDS_MSG_OPEN_EXCEL_ERROR);
		strTital.LoadString(IDS_MSG_TITAL_ERROR);
		MessageBox(strMsg,strTital,MB_ICONERROR);
		GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(TRUE);
		return;
	}

	MyAlignment Xalign;

	//对齐方式
	Xalign.HorizontalAlignment = xlLeft;
	Xalign.VerticalAlignment = xlTop;

	strText.LoadString(IDS_PAGE_SUMMARY_LIST);
	myExcel.AddSheet(strText);

	myExcel.SetAlignment(Xalign);

	// 获取表头
	LVCOLUMN lvcol = {0};
	TCHAR szBuffer[256] = {NULL};
	lvcol.mask = LVCF_TEXT;
	lvcol.pszText = szBuffer;
	lvcol.cchTextMax = 256;
	int nCol = 0;
	while (m_ListCtrl.GetColumn(nCol,&lvcol))
	{
		myExcel.SetItemText(1,nCol+1,lvcol.pszText);
		nCol++;
	}

	int nRows = m_ListCtrl.GetItemCount();
	int nCols = m_ListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int row = 0; row < nRows;row++)
	{
		for (int col = 0;col < nCols;col++)
		{
			myExcel.SetItemText(row+2,col+1,m_ListCtrl.GetItemText(row,col));
		}
	}

	myExcel.AutoRange();
	myExcel.AutoColFit();

	myExcel.SaveAs(strStrFile);
	myExcel.Exit();

	CString strParameters;
	strParameters.Format(_T(" /select,%s"),strStrFile);

	ShellExecute(NULL,_T("open"),_T("Explorer.exe"),strParameters,NULL,SW_SHOW);

	GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(TRUE);
}


afx_msg LRESULT CPageList::OnExportSummaryList(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedButtonExport();
	return 0;
}

void CPageList::EnableControls( BOOL bEnable )
{
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(bEnable);
}
