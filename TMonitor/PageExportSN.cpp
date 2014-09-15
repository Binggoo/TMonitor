// PageExportSN.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TMonitor.h"
#include "PageExportSN.h"
#include "afxdialogex.h"


// CPageExportSN �Ի���

IMPLEMENT_DYNAMIC(CPageExportSN, CDialogEx)

CPageExportSN::CPageExportSN(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPageExportSN::IDD, pParent)
	, m_strEditSN(_T(""))
	, m_strEditAlias(_T(""))
	, m_strEditMachineID(_T(""))
{
	m_pMySlotData = NULL;
}

CPageExportSN::~CPageExportSN()
{
}

void CPageExportSN::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START_TIME, m_StartTimeCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END_TIME, m_EndTimeCtrl);
	DDX_Text(pDX, IDC_EDIT_SN, m_strEditSN);
	DDX_Control(pDX, IDC_LIST_SN, m_ListSN);
	DDX_Text(pDX, IDC_EDIT_ALIAS, m_strEditAlias);
	DDX_Text(pDX, IDC_EDIT_MACHINE_ID, m_strEditMachineID);
}


BEGIN_MESSAGE_MAP(CPageExportSN, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CPageExportSN::OnBnClickedButtonQuery)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_SN, &CPageExportSN::OnBnClickedButtonExportSn)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CPageExportSN::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CPageExportSN::OnBnClickedButtonClear)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CPageExportSN ��Ϣ�������


BOOL CPageExportSN::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	ASSERT(m_pMySlotData);

	m_StartTimeCtrl.SetFormat(_T("yyyy-MM-dd HH:mm:ss"));
	m_EndTimeCtrl.SetFormat(_T("yyyy-MM-dd HH:mm:ss"));

	CTime timeBegin = CTime::GetCurrentTime();
	CTime timeEnd(3000,12,31,23,59,59);
	m_StartTimeCtrl.SetTime(&timeBegin);
	m_EndTimeCtrl.SetTime(&timeEnd);
	

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

	InitialListCtrl();

	// ����������ɫ
	static CFont font;
	font.CreatePointFont(120,_T("Arial"));

	GetDlgItem(IDC_SN_TOTAL)->SetFont(&font);
	GetDlgItem(IDC_SN_REPEAT)->SetFont(&font);
	GetDlgItem(IDC_SN_REAL)->SetFont(&font);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CPageExportSN::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CPageExportSN::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
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

void CPageExportSN::ChangeSize( CWnd *pWnd,int cx,int cy )
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

void CPageExportSN::InitialListCtrl()
{
	CString strText;
	int nItem = 0;

	strText.LoadString(IDS_ID);
	m_ListSN.InsertColumn(nItem,strText,LVCFMT_LEFT,50);
	nItem++;

	strText.LoadString(IDS_MACHINE_ID);
	m_ListSN.InsertColumn(nItem,strText,LVCFMT_LEFT,140);
	nItem++;

	strText.LoadString(IDS_ALIAS);
	m_ListSN.InsertColumn(nItem,strText,LVCFMT_LEFT,60);
	nItem++;

	strText.LoadString(IDS_SN);
	m_ListSN.InsertColumn(nItem,strText,LVCFMT_LEFT,120);
	nItem++;

	strText.LoadString(IDS_GETTIME);
	m_ListSN.InsertColumn(nItem,strText,LVCFMT_LEFT,130);
	nItem++;

	m_ListSN.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
}


void CPageExportSN::OnBnClickedButtonQuery()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	// �жϱ��Ƿ����
	if (!m_pMySlotData->IsTableExist(TS123_SN_TABLE_NAME))
	{
		return;
	}

	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(FALSE);
	UpdateData(TRUE);

	m_ListSN.DeleteAllItems();

	CString strQuery;
	CString strTmp;
	CTime startTime,endTime;
	m_StartTimeCtrl.GetTime(startTime);
	m_EndTimeCtrl.GetTime(endTime);

	strQuery.Format(_T("SELECT id,machineid,alias,sn,gettime FROM %s WHERE (gettime BETWEEN '%s' AND '%s') "),TS123_SN_TABLE_NAME
					   ,startTime.Format(_T("%Y-%m-%d %H:%M:%S"))
					   ,endTime.Format(_T("%Y-%m-%d %H:%M:%S")));

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

	strQuery += _T(";");

	m_pMySlotData->QueryData(strQuery,&m_ListSN);

	strQuery.Replace(_T("SELECT id,machineid,alias,sn,gettime"),_T("SELECT COUNT(DISTINCT sn)"));

	int nRealCount = m_pMySlotData->GetCount(strQuery);
	int nTotalCount = m_ListSN.GetItemCount();
	int nRepeatCount = nTotalCount - nRealCount;

	CString strText,strTotal,strRepeat,strReal;
	strText.LoadString(IDS_FORMAT_TOTAL);
	strTotal.Format(strText,nTotalCount);

	strText.LoadString(IDS_FORMAT_REPEAT);
	strRepeat.Format(strText,nRepeatCount);

	strText.LoadString(IDS_FORMAT_REAL);
	strReal.Format(strText,nRealCount);

	SetDlgItemText(IDC_SN_TOTAL,strTotal);
	SetDlgItemText(IDC_SN_REPEAT,strRepeat);
	SetDlgItemText(IDC_SN_REAL,strReal);

	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
}


void CPageExportSN::OnBnClickedButtonExportSn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CString strText;
	strText.LoadString(IDS_TEXT_FILTER);
	CString strFilter = strText;
	CFileDialog dlg(FALSE,_T("txt"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter,this);
	//dlg.m_ofn.lpstrInitialDir = m_strAppPath;

	if(dlg.DoModal() != IDOK) 
	{
		return; 
	}

	GetDlgItem(IDC_BUTTON_EXPORT_SN)->EnableWindow(FALSE);

	CString strStrFile = dlg.GetPathName();  //ѡ�񱣴�·������

	UpdateData(TRUE);

	CString strQuery;
	CString strTmp;
	CTime startTime,endTime;
	m_StartTimeCtrl.GetTime(startTime);
	m_EndTimeCtrl.GetTime(endTime);

	strQuery.Format(_T("SELECT COUNT(*) FROM %s WHERE (gettime BETWEEN '%s' AND '%s') "),TS123_SN_TABLE_NAME
		,startTime.Format(_T("%Y-%m-%d %H:%M:%S"))
		,endTime.Format(_T("%Y-%m-%d %H:%M:%S")));

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

	strQuery += _T(";");

	// ��ȡ����
	int nTotalCount = 0,nRepeatCount = 0,nRealCount = 0;
	nTotalCount= m_pMySlotData->GetCount(strQuery);

	if (nTotalCount < 1)
	{
		CString strMsg,strTital;
		strMsg.LoadString(IDS_MSG_NO_DATA);
		strTital.LoadString(IDS_MSG_TITAL_INFORMATION);
		MessageBox(strMsg,strTital,MB_ICONINFORMATION);
		GetDlgItem(IDC_BUTTON_EXPORT_SN)->EnableWindow(TRUE);
		return;
	}

	// ��ȡ���ظ���SN
	CStringArray strArrayBuffer;
	strQuery.Replace(_T("SELECT COUNT(*)"),_T("SELECT DISTINCT sn"));
	m_pMySlotData->QueryData(strQuery,&strArrayBuffer);
	nRealCount = strArrayBuffer.GetCount();
	nRepeatCount = nTotalCount - nRealCount;

	CFile file;
	file.Open(strStrFile,CFile::modeCreate | CFile::modeWrite);
	CString strBuffer;
	strBuffer.Format(_T("Phiyo TS-123 SN log file\r\n"));
	USES_CONVERSION;
	char *pBuffer = W2A(strBuffer);
	file.Write(pBuffer,strlen(pBuffer));

	strBuffer.Format(_T("File Create Time: %s\r\n"),CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S")));
	pBuffer = W2A(strBuffer);
	file.Write(pBuffer,strlen(pBuffer));

	strBuffer.Format(_T("Start Time: %s\tEnd Time: %s\r\n")
		,startTime.Format(_T("%Y-%m-%d %H:%M:%S"))
		,endTime.Format(_T("%Y-%m-%d %H:%M:%S")));
	pBuffer = W2A(strBuffer);
	file.Write(pBuffer,strlen(pBuffer));

	strBuffer.Format(_T("Total SN: %d\tRepeat SN: %d\tReal SN: %d\r\n"),nTotalCount,nRepeatCount,nRealCount);
	pBuffer = W2A(strBuffer);
	file.Write(pBuffer,strlen(pBuffer));

	for (int i = 0;i < nRealCount;i++)
	{
		strBuffer = strArrayBuffer.GetAt(i);
		strBuffer += _T("\r\n");

		pBuffer = W2A(strBuffer);
		file.Write(pBuffer,strlen(pBuffer));
	}

	file.Flush();
	file.Close();

	CString strParameters;
	strParameters.Format(_T(" /select,%s"),strStrFile);

	ShellExecute(NULL,_T("open"),_T("Explorer.exe"),strParameters,NULL,SW_SHOW);
	GetDlgItem(IDC_BUTTON_EXPORT_SN)->EnableWindow(TRUE);
}

void CPageExportSN::SetMySlotData( CMySlotData *pMySlotData )
{
	m_pMySlotData = pMySlotData;
}


void CPageExportSN::OnBnClickedButtonDelete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nCount = m_ListSN.GetItemCount();
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
		CString strId = m_ListSN.GetItemText(0,0);

		m_pMySlotData->DeleteData(strId,TS123_SN_TABLE_NAME);

		m_ListSN.DeleteItem(0);

		nCount = m_ListSN.GetItemCount();
	}		
}


void CPageExportSN::OnBnClickedButtonClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strMsg,strTital;
	strMsg.LoadString(IDS_MSG_CLEAN_TABLE);
	strTital.LoadString(IDS_MSG_TITAL_WARNING);
	if (IDNO == MessageBox(strMsg
		,strTital,MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING))
	{
		return;
	}

	m_ListSN.DeleteAllItems();
	m_pMySlotData->CleanupTable(TS123_SN_TABLE_NAME);
}


HBRUSH CPageExportSN::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if (pWnd->GetDlgCtrlID() == IDC_SN_TOTAL)
	{
		pDC->SetTextColor(RGB(30, 144, 255));
	}

	if (pWnd->GetDlgCtrlID() == IDC_SN_REAL)
	{
		pDC->SetTextColor(RGB(0, 139, 0));
	}

	if (pWnd->GetDlgCtrlID() == IDC_SN_REPEAT)
	{
		pDC->SetTextColor(RGB(148, 0, 211));
	}

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

void CPageExportSN::EnableControls( BOOL bEnable )
{
	// ����ɾ������
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(bEnable);
}
