// PageDataReport.cpp : 实现文件
//
// 2014-09-15 ListCtrl中新增排序、隐藏/显示、编辑功能

#include "stdafx.h"
#include "TMonitor.h"
#include "PageDataReport.h"
#include "afxdialogex.h"
#include "MyExcel.h"
#include "Utils.h"

#include "..\CGridListCtrlEx\CGridColumnTraitEdit.h"


// CPageDataReport 对话框

IMPLEMENT_DYNAMIC(CPageDataReport, CDialogEx)

CPageDataReport::CPageDataReport(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPageDataReport::IDD, pParent)
	, m_strEditSlot(_T(""))
	, m_strEditSN(_T(""))
	, m_strEditFunction(_T(""))
	, m_strEditResult(_T(""))
	, m_strEditMachineID(_T(""))
	, m_strEditAlias(_T(""))
{
	m_pMySlotData = NULL;
	m_bEnableDel = TRUE;
}

CPageDataReport::~CPageDataReport()
{
}

void CPageDataReport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START_TIME, m_StartTimeCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END_TIME, m_EndTimeCtrl);
	DDX_Text(pDX, IDC_EDIT_SLOT, m_strEditSlot);
	DDX_Text(pDX, IDC_EDIT_SN, m_strEditSN);
	DDX_Text(pDX, IDC_EDIT_FUNCTION, m_strEditFunction);
	DDX_Text(pDX, IDC_EDIT_RESULT, m_strEditResult);
	DDX_Control(pDX, IDC_LIST_QUERY, m_ListCtrlQuery);
	DDX_Text(pDX, IDC_EDIT_MACHINE_ID, m_strEditMachineID);
	DDX_Text(pDX, IDC_EDIT_ALIAS, m_strEditAlias);
}


BEGIN_MESSAGE_MAP(CPageDataReport, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CPageDataReport::OnBnClickedButtonQuery)
	ON_BN_CLICKED(IDC_BUTTON_QUERY_EXPORT, &CPageDataReport::OnBnClickedButtonQueryExport)
	ON_COMMAND(ID_RIGHTCLICK_DELETE_SELECTED, &CPageDataReport::OnRightclickDeleteSelected)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_QUERY, &CPageDataReport::OnNMRClickListQuery)
	ON_COMMAND(ID_RIGHTCLICK_DELETE_ALL, &CPageDataReport::OnRightclickDeleteAll)
	ON_COMMAND(ID_RIGHTCLICK_CLEAN_TABLE, &CPageDataReport::OnRightclickCleanTable)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CPageDataReport::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CPageDataReport::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CPageDataReport 消息处理程序


BOOL CPageDataReport::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	ASSERT(m_pMySlotData);

	m_StartTimeCtrl.SetFormat(_T("yyyy-MM-dd HH:mm:ss"));
	m_EndTimeCtrl.SetFormat(_T("yyyy-MM-dd HH:mm:ss"));

	CTime timeBegin = CTime::GetCurrentTime();
	CTime timeEnd(3000,12,31,23,59,59);
	m_StartTimeCtrl.SetTime(&timeBegin);
	m_EndTimeCtrl.SetTime(&timeEnd);

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

	// 设置字体颜色
	static CFont font;
	font.CreatePointFont(120,_T("Arial"));

	GetDlgItem(IDC_TEXT_REPORT_TOTAL)->SetFont(&font);
	GetDlgItem(IDC_TEXT_REPORT_PASS)->SetFont(&font);
	GetDlgItem(IDC_TEXT_REPORT_FAIL)->SetFont(&font);
	GetDlgItem(IDC_TEXT_REPORT_YIELD)->SetFont(&font);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CPageDataReport::OnSize(UINT nType, int cx, int cy)
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
		DWORD dwFlag = SIZE_NO_CHANGE;

		if (pWnd->GetDlgCtrlID() == IDC_LIST_QUERY)
		{
			dwFlag = SIZE_ELASTIC_X_EX | SIZE_ELASTIC_Y_EX;
		}
		else
		{
			dwFlag = SIZE_MOVE_X | SIZE_ELASTIC_X;
		}

		ChangeSize(pWnd,cx,cy,dwFlag);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}

	GetClientRect(&m_rect);
}

void CPageDataReport::ChangeSize( CWnd *pWnd,int cx,int cy ,DWORD flag)
{
	if(pWnd)  //判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建   
	{  
		CRect rectCtrl;   //获取控件变化前的大小    
		pWnd->GetWindowRect(&rectCtrl);  
		ScreenToClient(&rectCtrl);//将控件大小转换为在对话框中的区域坐标 

		int iLeft = rectCtrl.left;
		int iTop = rectCtrl.top;
		int iWidth = rectCtrl.Width();
		int iHeight = rectCtrl.Height();

		// 改变X坐标
		if ((flag & SIZE_MOVE_X) == SIZE_MOVE_X)
		{
			iLeft = iLeft * cx / m_rect.Width();
		}

		// 改变Y坐标
		if ((flag & SIZE_MOVE_Y) == SIZE_MOVE_Y)
		{
			iTop = iTop * cy / m_rect.Height();
		}

		//改变宽度
		if ((flag & SIZE_ELASTIC_X) == SIZE_ELASTIC_X)
		{
			iWidth = iWidth * cx / m_rect.Width();
		}

		// 改变高度
		if ((flag & SIZE_ELASTIC_Y) == SIZE_ELASTIC_Y)
		{
			iHeight = iHeight * cy / m_rect.Height();
		}

		//改变宽度
		if ((flag & SIZE_ELASTIC_X_EX) == SIZE_ELASTIC_X_EX)
		{
			iWidth = cx - iLeft - 10;
		}

		// 改变高度
		if ((flag & SIZE_ELASTIC_Y_EX) == SIZE_ELASTIC_Y_EX)
		{
			iHeight = cy - iTop - 10;
		}

		pWnd->MoveWindow(iLeft,iTop,iWidth,iHeight);
	}  
}

void CPageDataReport::InitialListCtrl()
{
	CGridColumnTrait* pTrait = new CGridColumnTraitEdit;

	// Create Columns
	m_ListCtrlQuery.InsertHiddenLabelColumn();	// Requires one never uses column 0

	CString strText;
	int nItem = 0;
	strText.LoadString(IDS_ID);
//	m_ListCtrlQuery.InsertColumn(nItem,strText,LVCFMT_LEFT,50);
	m_ListCtrlQuery.InsertColumnTrait(nItem+1,strText,LVCFMT_LEFT,50,nItem);
	nItem++;

	strText.LoadString(IDS_SLOT);
//	m_ListCtrlQuery.InsertColumn(nItem,strText,LVCFMT_LEFT,40);
	m_ListCtrlQuery.InsertColumnTrait(nItem+1,strText,LVCFMT_LEFT,50,nItem);
	nItem++;

	strText.LoadString(IDS_TYPE);
//	m_ListCtrlQuery.InsertColumn(nItem,strText,LVCFMT_LEFT,40);
	m_ListCtrlQuery.InsertColumnTrait(nItem+1,strText,LVCFMT_LEFT,50,nItem);
	nItem++;

	strText.LoadString(IDS_MACHINE_ID);
//	m_ListCtrlQuery.InsertColumn(nItem,strText,LVCFMT_LEFT,140);
	m_ListCtrlQuery.InsertColumnTrait(nItem+1,strText,LVCFMT_LEFT,140,nItem);
	nItem++;

	strText.LoadString(IDS_ALIAS);
//	m_ListCtrlQuery.InsertColumn(nItem,strText,LVCFMT_LEFT,60);
	m_ListCtrlQuery.InsertColumnTrait(nItem+1,strText,LVCFMT_LEFT,60,nItem);
	nItem++;

	strText.LoadString(IDS_SN);
//	m_ListCtrlQuery.InsertColumn(nItem,strText,LVCFMT_LEFT,100);
	m_ListCtrlQuery.InsertColumnTrait(nItem+1,strText,LVCFMT_LEFT,100,nItem,pTrait);
	nItem++;

	strText.LoadString(IDS_SIZE_MB);
//	m_ListCtrlQuery.InsertColumn(nItem,strText,LVCFMT_LEFT,80);
	m_ListCtrlQuery.InsertColumnTrait(nItem+1,strText,LVCFMT_LEFT,80,nItem);
	nItem++;

	strText.LoadString(IDS_FUNCTION);
//	m_ListCtrlQuery.InsertColumn(nItem,strText,LVCFMT_LEFT,100);
	m_ListCtrlQuery.InsertColumnTrait(nItem+1,strText,LVCFMT_LEFT,100,nItem);
	nItem++;

	strText.LoadString(IDS_START_TIME);
//	m_ListCtrlQuery.InsertColumn(nItem,strText,LVCFMT_LEFT,130);
	m_ListCtrlQuery.InsertColumnTrait(nItem+1,strText,LVCFMT_LEFT,140,nItem);
	nItem++;

	strText.LoadString(IDS_END_TIME);
//	m_ListCtrlQuery.InsertColumn(nItem,strText,LVCFMT_LEFT,130);
	m_ListCtrlQuery.InsertColumnTrait(nItem+1,strText,LVCFMT_LEFT,140,nItem);
	nItem++;

	strText.LoadString(IDS_SPEED_MB_S);
//	m_ListCtrlQuery.InsertColumn(nItem,strText,LVCFMT_LEFT,80);
	m_ListCtrlQuery.InsertColumnTrait(nItem+1,strText,LVCFMT_LEFT,100,nItem);
	nItem++;

	strText.LoadString(IDS_RESULT);
//	m_ListCtrlQuery.InsertColumn(nItem,strText,LVCFMT_LEFT,50);
	m_ListCtrlQuery.InsertColumnTrait(nItem+1,strText,LVCFMT_LEFT,50,nItem);
	nItem++;

	m_ListCtrlQuery.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
}


void CPageDataReport::OnBnClickedButtonQuery()
{
	// TODO: 在此添加控件通知处理程序代码

	// 判断表是否存在
	if (!m_pMySlotData->IsTableExist(TS123_TABLE_NAME))
	{
		return;
	}

	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(FALSE);
	UpdateData(TRUE);

	m_ListCtrlQuery.DeleteAllItems();

	CString strQuery;
	CString strTmp;
	CTime startTime,endTime;
	m_StartTimeCtrl.GetTime(startTime);
	m_EndTimeCtrl.GetTime(endTime);

	DB_SLOT *dbSlots = NULL;

	strQuery.Format(_T("SELECT id,slot,type,machineid,alias,sn,size,function,starttime,endtime,speed,result FROM %s WHERE (starttime BETWEEN '%s' AND '%s' \
					   OR endtime BETWEEN '%s' AND '%s') "),TS123_TABLE_NAME
					   ,startTime.Format(_T("%Y-%m-%d %H:%M:%S"))
					   ,endTime.Format(_T("%Y-%m-%d %H:%M:%S"))
					   ,startTime.Format(_T("%Y-%m-%d %H:%M:%S"))
					   ,endTime.Format(_T("%Y-%m-%d %H:%M:%S")));
	
	if (!m_strEditSlot.IsEmpty())
	{
		if (m_strEditSlot.Left(1) == _T("!"))
		{
			m_strEditSlot.TrimLeft(_T("!"));
			
			if (m_strEditSlot.Left(1) == _T("%") || m_strEditSlot.Right(1) == _T("%"))
			{
				strTmp.Format(_T("AND NOT slot LIKE '%s' "),m_strEditSlot);
			}
			else
			{
				strTmp.Format(_T("AND NOT slot='%s' "),m_strEditSlot);
			}
		}
		else if (m_strEditSlot.Left(1) == _T("%") || m_strEditSlot.Right(1) == _T("%"))
		{
			strTmp.Format(_T("AND slot LIKE '%s' "),m_strEditSlot);
		}
		else 
		{
			strTmp.Format(_T("AND slot='%s' "),m_strEditSlot);
		}

		strQuery += strTmp;
	}

	if (!m_strEditMachineID.IsEmpty())
	{
		if (m_strEditMachineID.Left(1) == _T("!"))
		{
			m_strEditMachineID.TrimLeft(_T("!"));
			
			if (m_strEditMachineID.Left(1) == _T("%") || m_strEditMachineID.Right(1) == _T("%"))
			{
				strTmp.Format(_T("AND NOT machineid LIKE '%s' "),m_strEditMachineID);
			}
			else
			{
				strTmp.Format(_T("AND NOT machineid='%s' "),m_strEditMachineID);
			}
		}
		else if (m_strEditMachineID.Left(1) == _T("%") || m_strEditMachineID.Right(1) == _T("%"))
		{
			strTmp.Format(_T("AND machineid LIKE '%s' "),m_strEditMachineID);
		}
		else
		{
			strTmp.Format(_T("AND machineid='%s' "),m_strEditMachineID);
		}

		strQuery += strTmp;
	}

	if (!m_strEditAlias.IsEmpty())
	{
		if (m_strEditAlias.Left(1) == _T("!"))
		{
			m_strEditAlias.TrimLeft(_T("!"));
			
			if (m_strEditAlias.Left(1) == _T("%") || m_strEditAlias.Right(1) == _T("%"))
			{
				strTmp.Format(_T("AND NOT alias LIKE '%s' "),m_strEditAlias);
			}
			else
			{
				strTmp.Format(_T("AND NOT alias='%s' "),m_strEditAlias);
			}
		}
		else if (m_strEditAlias.Left(1) == _T("%") || m_strEditAlias.Right(1) == _T("%"))
		{
			strTmp.Format(_T("AND alias LIKE '%s' "),m_strEditAlias);
		}
		else 
		{
			strTmp.Format(_T("AND alias='%s' "),m_strEditAlias);
		}

		strQuery += strTmp;
	}

	if (!m_strEditSN.IsEmpty())
	{
		if (m_strEditSN.Left(1) == _T("!"))
		{
			m_strEditSN.TrimLeft(_T("!"));
			
			if (m_strEditSN.Left(1) == _T("%") || m_strEditSN.Right(1) == _T("%"))
			{
				strTmp.Format(_T("AND NOT sn LIKE '%s' "),m_strEditSN);
			}
			else
			{
				strTmp.Format(_T("AND NOT sn='%s' "),m_strEditSN);
			}
		}
		else if (m_strEditSN.Left(1) == _T("%") || m_strEditSN.Right(1) == _T("%"))
		{
			strTmp.Format(_T("AND sn LIKE '%s' "),m_strEditSN);
		}
		else 
		{
			strTmp.Format(_T("AND sn='%s' "),m_strEditSN);
		}

		strQuery += strTmp;
	}

	if (!m_strEditFunction.IsEmpty())
	{
		if (m_strEditFunction.Left(1) == _T("!"))
		{
			m_strEditFunction.Trim(_T("!"));
			
			if (m_strEditFunction.Left(1) == _T("%") || m_strEditFunction.Right(1) == _T("%"))
			{
				strTmp.Format(_T("AND NOT function LIKE '%s' "),m_strEditFunction);
			}
			else
			{
				strTmp.Format(_T("AND NOT function='%s' "),m_strEditFunction);
			}
		}
		else if (m_strEditFunction.Left(1) == _T("%") || m_strEditFunction.Right(1) == _T("%"))
		{
			strTmp.Format(_T("AND function LIKE '%s' "),m_strEditFunction);
		}
		else 
		{
			strTmp.Format(_T("AND function='%s' "),m_strEditFunction);
		}

		strQuery += strTmp;
	}

	if (!m_strEditResult.IsEmpty())
	{
		if (m_strEditResult.Left(1) == _T("!"))
		{
			m_strEditResult.Trim(_T("!"));
			
			if (m_strEditResult.Left(1) == _T("%") || m_strEditResult.Right(1) == _T("%"))
			{
				strTmp.Format(_T("AND NOT result LIKE '%s' "),m_strEditResult);
			}
			else
			{
				strTmp.Format(_T("AND NOT result='%s' "),m_strEditResult);
			}
		}
		else if (m_strEditResult.Left(1) == _T("%") || m_strEditResult.Right(1) == _T("%"))
		{
			strTmp.Format(_T("AND result LIKE '%s' "),m_strEditResult);
		}
		else 
		{
			strTmp.Format(_T("AND result='%s' "),m_strEditResult);
		}

		strQuery += strTmp;
	}

	strQuery += _T(";");

	m_pMySlotData->QueryData(strQuery,&m_ListCtrlQuery,1);

	int nRows = m_ListCtrlQuery.GetItemCount();
	int nPass = 0,nFail = 0;

	for (int row = 0; row < nRows;row++)
	{
		CString strItem = m_ListCtrlQuery.GetItemText(row,COL_RESULT);

		if (strItem.CompareNoCase(_T("PASS")) == 0)
		{
			nPass++;
		}
		else
		{
			nFail++;
		}
	}

	CString strTotal,strPass,strFail,strYield;
	CString strText;
	strText.LoadString(IDS_FORMAT_TOTAL);
	strTotal.Format(strText,nRows);

	strText.LoadString(IDS_FORMAT_PASS);
	strPass.Format(strText,nPass);

	strText.LoadString(IDS_FORMAT_FAIL);
	strFail.Format(strText,nFail);

	strText.LoadString(IDS_FORMAT_YIELD);
	if (nRows != 0)
	{
		strYield.Format(strText,(double)100 * nPass/nRows);
	}
	else
	{
		strYield.Format(strText,0.0);
	}

	SetDlgItemText(IDC_TEXT_REPORT_TOTAL,strTotal);
	SetDlgItemText(IDC_TEXT_REPORT_PASS,strPass);
	SetDlgItemText(IDC_TEXT_REPORT_FAIL,strFail);
	SetDlgItemText(IDC_TEXT_REPORT_YIELD,strYield);

	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
}


void CPageDataReport::OnBnClickedButtonQueryExport()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strText;
	strText.LoadString(IDS_EXCEL_FILTER);

	CString strFilter = strText;
	
	CFileDialog dlg(FALSE,_T("xlsx"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter,this);

	if(dlg.DoModal() != IDOK) 
	{
		return; 
	}

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
		return;
	}

	MyAlignment Xalign;
	MyNumberFormat numberFormat;

	//对齐方式
	Xalign.HorizontalAlignment = xlLeft;
	Xalign.VerticalAlignment = xlTop;

	strText.LoadString(IDS_PAGE_DATA_REPORT);
	myExcel.AddSheet(strText);

	myExcel.SetAlignment(Xalign);

	// 获取表头
	LVCOLUMN lvcol = {0};
	TCHAR szBuffer[256] = {NULL};
	lvcol.mask = LVCF_TEXT;
	lvcol.pszText = szBuffer;
	lvcol.cchTextMax = 256;
	int nCol = 1; // CGridListCtrlEx第一列是隐藏不用的
	while (m_ListCtrlQuery.GetColumn(nCol,&lvcol))
	{
		myExcel.SetItemText(1,nCol,lvcol.pszText);
		nCol++;
	}

	int nRows = m_ListCtrlQuery.GetItemCount();
	int nCols = m_ListCtrlQuery.GetHeaderCtrl()->GetItemCount();
	for (int row = 0; row < nRows;row++)
	{
		for (int col = 1;col < nCols;col++)
		{
			if (col == COL_SN || col == COL_MACHINE_ID)
			{
				CString strStart;
				strStart.Format(_T("%c%d"),_T('A') + col - 1,row + 2);

				myExcel.GetRange(strStart,strStart);

				numberFormat.GetText();

				myExcel.SetNumberFormat(numberFormat);

				myExcel.AutoRange();
			}

			myExcel.SetItemText(row+2,col,m_ListCtrlQuery.GetItemText(row,col));
		}
	}

	myExcel.AutoRange();
	myExcel.AutoColFit();

	myExcel.SaveAs(strStrFile);
	myExcel.Exit();

	CString strParameters;
	strParameters.Format(_T(" /select,%s"),strStrFile);

	ShellExecute(NULL,_T("open"),_T("Explorer.exe"),strParameters,NULL,SW_SHOW);
}

void CPageDataReport::SetMySlotData( CMySlotData *pMySlotData )
{
	m_pMySlotData = pMySlotData;
}


void CPageDataReport::OnRightclickDeleteSelected()
{
	// TODO: 在此添加命令处理程序代码

	POSITION pos = m_ListCtrlQuery.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		TRACE0("No items were selected!\n");
	}
	else
	{
		CString strMsg,strTital;
		strMsg.LoadString(IDS_MSG_DELETE_SELECT);
		strTital.LoadString(IDS_MSG_TITAL_WARNING);
		if (IDNO == MessageBox(strMsg
			,strTital,MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING))
		{
			return;
		}

		while (pos)
		{
			int nItem = m_ListCtrlQuery.GetNextSelectedItem(pos);
			
			// you could do your own processing on nItem here

			CString strId = m_ListCtrlQuery.GetItemText(nItem,0);

			m_pMySlotData->DeleteData(strId,TS123_TABLE_NAME);

			m_ListCtrlQuery.DeleteItem(nItem);

			pos = m_ListCtrlQuery.GetFirstSelectedItemPosition();
		}
	}

}


void CPageDataReport::OnNMRClickListQuery(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	if (m_bEnableDel)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );

		CMenu menu;
		VERIFY( menu.LoadMenu( IDR_MENU_DATA_REPORT ) );
		CMenu* popup = menu.GetSubMenu(0);
		ASSERT( popup != NULL );
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );

	}
	*pResult = 0;
}


void CPageDataReport::OnRightclickDeleteAll()
{
	// TODO: 在此添加命令处理程序代码
	// TODO: 在此添加命令处理程序代码
	int nCount = m_ListCtrlQuery.GetItemCount();
	if (nCount < 1)
	{
		return;
	}

	CString strMsg,strTital;
	strMsg.LoadString(IDS_MSG_DELETE_ALL);
	strTital.LoadString(IDS_MSG_TITAL_WARNING);
	if (IDNO == MessageBox(strMsg
		,strTital,MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING))
	{
		return;
	}

	while (nCount)
	{
		CString strId = m_ListCtrlQuery.GetItemText(0,0);

		m_pMySlotData->DeleteData(strId,TS123_TABLE_NAME);

		m_ListCtrlQuery.DeleteItem(0);

		nCount = m_ListCtrlQuery.GetItemCount();
	}		
}
	


void CPageDataReport::OnRightclickCleanTable()
{
	// TODO: 在此添加命令处理程序代码
	CString strMsg,strTital;
	strMsg.LoadString(IDS_MSG_CLEAN_TABLE);
	strTital.LoadString(IDS_MSG_TITAL_WARNING);
	if (IDNO == MessageBox(strMsg
		,strTital,MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING))
	{
		return;
	}

	m_ListCtrlQuery.DeleteAllItems();
	m_pMySlotData->CleanupTable(TS123_TABLE_NAME);
}


HBRUSH CPageDataReport::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_TEXT_REPORT_TOTAL)
	{
		pDC->SetTextColor(RGB(30, 144, 255));
	}

	if (pWnd->GetDlgCtrlID() == IDC_TEXT_REPORT_PASS)
	{
		pDC->SetTextColor(RGB(0, 139, 0));
	}

	if (pWnd->GetDlgCtrlID() == IDC_TEXT_REPORT_FAIL)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}

	if (pWnd->GetDlgCtrlID() == IDC_TEXT_REPORT_YIELD)
	{
		pDC->SetTextColor(RGB(148, 0, 211));
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


BOOL CPageDataReport::PreTranslateMessage(MSG* pMsg)
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


void CPageDataReport::OnBnClickedButtonDelete()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCount = m_ListCtrlQuery.GetItemCount();
	if (nCount < 1)
	{
		return;
	}

	CString strMsg,strTital;
	strMsg.LoadString(IDS_MSG_DELETE_ALL);
	strTital.LoadString(IDS_MSG_TITAL_WARNING);
	if (IDNO == MessageBox(strMsg
		,strTital,MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING))
	{
		return;
	}

	while (nCount)
	{
		CString strId = m_ListCtrlQuery.GetItemText(0,0);

		m_pMySlotData->DeleteData(strId,TS123_TABLE_NAME);

		m_ListCtrlQuery.DeleteItem(0);

		nCount = m_ListCtrlQuery.GetItemCount();
	}		
}


void CPageDataReport::OnBnClickedButtonClear()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strMsg,strTital;
	strMsg.LoadString(IDS_MSG_CLEAN_TABLE);
	strTital.LoadString(IDS_MSG_TITAL_WARNING);
	if (IDNO == MessageBox(strMsg
		,strTital,MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING))
	{
		return;
	}

	m_ListCtrlQuery.DeleteAllItems();
	m_pMySlotData->CleanupTable(TS123_TABLE_NAME);
}

void CPageDataReport::EnableControls( BOOL bEnable )
{
	// 禁用删除功能
	m_bEnableDel = bEnable;
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(bEnable);
}
