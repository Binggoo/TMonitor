
// TMonitorDlg.cpp : implementation file
//
// v1.0.1.0 2013-12-10 �����ϵͳ���ڸı�ʱ���ܼ�ʱ���´��ڵ����⡣
// v1.0.2.0 2013-12-30 �޸ļ�¼LOG��ʽ��
// v1.0.3.0 2014-01-21 1.���յ�##STARTTASK::��־ʱ���Զ�����
//                     2.�޸��������뷽ʽ
//                     3.������������ʾ

#include "stdafx.h"
#include "TMonitor.h"
#include "TMonitorDlg.h"
#include "afxdialogex.h"

#pragma comment(lib,"version.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//�޸ı���
	CString strPath;
	GetModuleFileName(NULL,strPath.GetBuffer(MAX_PATH),MAX_PATH);
	strPath.ReleaseBuffer();

	CString strVersion = CTMonitorDlg::GetAppVersion(strPath);

	CString strTitle;
	strTitle += _T("About ");
	strTitle += PRJ_NAME + strVersion;
	SetWindowText(strTitle);

	strVersion = _T("Version:") + strVersion;

	SetDlgItemText(IDC_TEXT_VERSION,strVersion);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


// CTMonitorDlg dialog




CTMonitorDlg::CTMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTMonitorDlg::IDD, pParent)
	, m_bCheckAutoSave(FALSE)
	, m_bCheckDate(FALSE)
	, m_bCheckTime(FALSE)
	, m_bConnected(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_tabMain);
	DDX_Check(pDX, IDC_CHECK_AUTO_SAVE, m_bCheckAutoSave);
	DDX_Check(pDX, IDC_CHECK_DATE, m_bCheckDate);
	DDX_Check(pDX, IDC_CHECK_TIME, m_bCheckTime);
	DDX_Control(pDX, IDC_COMBO_COM, m_comboCom);
}

BEGIN_MESSAGE_MAP(CTMonitorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CTMonitorDlg::OnTcnSelchangeTabMain)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_AUTO_SAVE, &CTMonitorDlg::OnBnClickedCheckAutoSave)
	ON_EN_CHANGE(IDC_EDIT_PREFIX, &CTMonitorDlg::OnEnChangeEditPrefix)
	ON_BN_CLICKED(IDC_CHECK_DATE, &CTMonitorDlg::OnBnClickedCheckDate)
	ON_BN_CLICKED(IDC_CHECK_TIME, &CTMonitorDlg::OnBnClickedCheckTime)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER, &CTMonitorDlg::OnBnClickedButtonBrowser)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CTMonitorDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_AS, &CTMonitorDlg::OnBnClickedButtonSaveAs)
	ON_BN_CLICKED(IDC_BUTTON_LOCK, &CTMonitorDlg::OnBnClickedButtonLock)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CTMonitorDlg::OnBnClickedButtonConnect)
	ON_COMMAND(ID_ABOUT, &CTMonitorDlg::OnAbout)
	ON_CBN_DROPDOWN(IDC_COMBO_COM, &CTMonitorDlg::OnCbnDropdownComboCom)
	ON_MESSAGE(ON_COM_RECEIVE, &CTMonitorDlg::OnOnComReceive)
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTMonitorDlg message handlers

BOOL CTMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_SerialPort.SetWnd(m_hWnd);

	//�޸ı���
	CString strPath;
	GetModuleFileName(NULL,strPath.GetBuffer(MAX_PATH),MAX_PATH);
	strPath.ReleaseBuffer();

	// ��ȡ����·��
	m_strAppPath = strPath.Left(strPath.ReverseFind(_T('\\')));  //������'\'

	CString strTempPath = m_strAppPath + TEMP_FILE;
	if (PathFileExists(strTempPath))
	{
		DeleteFile(strTempPath);
	}

	CString strVersion = GetAppVersion(strPath);

	CString strTitle = PRJ_NAME + strVersion;
	SetWindowText(strTitle);

	//ΪTab���2��ҳ��
	m_tabMain.InsertItem(0,_T("Log Message"));
	//m_tabMain.InsertItem(1,_T("Device Status"));

	//����2���Ի���
	m_PageLog.Create(IDD_DIALOG_LOG,&m_tabMain);
	//m_PageDevice.Create(IDD_DIALOG_DEVICE_STATUS,&m_tabMain);

	//��ʾ��ʼ����
	m_PageLog.ShowWindow(SW_SHOW);
	//m_PageDevice.ShowWindow(SW_HIDE);
	//m_nCurSelTab = 0;

	//����Ի���ҳ��ָ��
	//m_pDlg[0] = &m_PageLog;
	//m_pDlg[1] = &m_PageDevice;

	m_brush.CreateSolidBrush(RGB(102,205,170));

	// ��ʼ��AutoSave
	EnableAutoSaveCtrl(m_bCheckAutoSave);
	CTime time = CTime::GetCurrentTime();
	CString strDate = time.Format(_T("Date: %Y-%m-%d"));
	CString strTime = time.Format(_T("Time: %H-%M-%S")); 

	SetDlgItemText(IDC_CHECK_DATE,strDate);
	SetDlgItemText(IDC_CHECK_TIME,strTime);

	// ����
	CStringArray strArrayName,strArrayPort;	
	GetSystemPorts(strArrayName,strArrayPort);

	for (int i = 0;i<strArrayPort.GetCount();i++)
	{
		m_comboCom.AddString(strArrayPort.GetAt(i));
	}

	m_comboCom.SetCurSel(0);

	// ������ʱ��
	SetTimer(TIMER_TIME,1000,NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// void CTMonitorDlg::OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult)
// {
// 	// TODO: �ڴ���ӿؼ�֪ͨ����������
// 	m_pDlg[m_nCurSelTab]->ShowWindow(SW_HIDE);
// 	m_nCurSelTab = m_tabMain.GetCurSel();
// 	m_pDlg[m_nCurSelTab]->ShowWindow(SW_SHOW);
// 	*pResult = 0;
// }


HBRUSH CTMonitorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if(nCtlColor != CTLCOLOR_BTN && nCtlColor != CTLCOLOR_EDIT && nCtlColor != CTLCOLOR_LISTBOX)
	{
		pDC->SetBkMode(TRANSPARENT);
		return m_brush;
	}

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}


void CTMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nIDEvent)
	{
	case TIMER_TIME:
		CTime time = CTime::GetCurrentTime();
		CString strDate = time.Format(_T("Date: %Y-%m-%d"));
		CString strTime = time.Format(_T("Time: %H-%M-%S")); 

		SetDlgItemText(IDC_CHECK_DATE,strDate);
		SetDlgItemText(IDC_CHECK_TIME,strTime);

		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CTMonitorDlg::OnBnClickedCheckAutoSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();

	EnableAutoSaveCtrl(m_bCheckAutoSave);
}

void CTMonitorDlg::EnableAutoSaveCtrl( BOOL bEnable /*= TRUE*/ )
{
	GetDlgItem(IDC_TEXT_PREFIX)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_PREFIX)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_DATE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_TIME)->EnableWindow(bEnable);
	GetDlgItem(IDC_TEXT_FILE_NAME)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_LOCATION)->EnableWindow(bEnable);
	GetDlgItem(IDC_TEXT_LOCATION)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_BROWSER)->EnableWindow(bEnable);
}


void CTMonitorDlg::OnEnChangeEditPrefix()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateFileName();
}


void CTMonitorDlg::OnBnClickedCheckDate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();

	UpdateFileName();
}


void CTMonitorDlg::OnBnClickedCheckTime()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();

	UpdateFileName();
}

void CTMonitorDlg::UpdateFileName()
{
	CString strFileName,strPrefix;
	GetDlgItemText(IDC_EDIT_PREFIX,strPrefix);
	strPrefix.Trim();

	strFileName += _T("File Name: ");
	strFileName += strPrefix;

	CTime time = CTime::GetCurrentTime();
	CString strDate = time.Format(_T("Date: %Y-%m-%d"));
	CString strTime = time.Format(_T("Time: %H-%M-%S")); 

	if (m_bCheckDate)
	{
		strFileName += time.Format(_T("%Y%m%d"));
	}

	if (m_bCheckTime)
	{
		strFileName += time.Format(_T("%H%M%S"));
	}

	strFileName += _T(".log");
	SetDlgItemText(IDC_TEXT_FILE_NAME,strFileName);
}


void CTMonitorDlg::OnBnClickedButtonBrowser()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO broInfo = {0};
	TCHAR      szDisName[MAX_PATH] = {0};

	CString strTitle = _T("Please select save log loaction : ");
	UINT    ulFalgs = BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN
		| BIF_BROWSEFORCOMPUTER | BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;

	broInfo.hwndOwner = this->m_hWnd;
	broInfo.pidlRoot  = NULL;
	broInfo.pszDisplayName = szDisName;
	broInfo.lpszTitle = strTitle;
	broInfo.ulFlags   = ulFalgs;
	broInfo.lpfn      = NULL;
	broInfo.lParam    = NULL;
	broInfo.iImage    = IDR_MAINFRAME;

	LPITEMIDLIST pIDList = SHBrowseForFolder(&broInfo);
	if (pIDList != NULL)
	{
		memset(szDisName, 0, sizeof(szDisName));
		SHGetPathFromIDList(pIDList, szDisName);
		SetDlgItemText(IDC_EDIT_LOCATION,szDisName);

		LPMALLOC pMalloc;
		//Retrieve a pointer to the shell's IMalloc interface
		if (SUCCEEDED(SHGetMalloc(&pMalloc)))
		{
			// free the PIDL that SHBrowseForFolder returned to us.
			pMalloc->Free(pIDList);
			// release the shell's IMalloc interface
			(void)pMalloc->Release();
		}
	}
}


void CTMonitorDlg::OnBnClickedButtonClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_PageLog.ClearLog();
}


void CTMonitorDlg::OnBnClickedButtonSaveAs()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_PageLog.SaveLog();
}


void CTMonitorDlg::OnBnClickedButtonLock()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strBtnText,strPassword;
	GetDlgItemText(IDC_BUTTON_LOCK,strBtnText);
	GetDlgItemText(IDC_EDIT_PASSWORD,strPassword);
	strPassword.Trim();

	if (strBtnText == _T("Lock"))
	{
		if (strPassword.IsEmpty())
		{
			m_strPassWord = DEFAULT_PASSWORD;
		}
		else
		{
			m_strPassWord = strPassword;
		}

		SetDlgItemText(IDC_EDIT_PASSWORD,NULL);
		SetDlgItemText(IDC_BUTTON_LOCK,_T("Unlock"));
		EnableAllSettingCtrl(FALSE);
	}
	else
	{
		if (strPassword == m_strPassWord || strPassword == DEFAULT_PASSWORD)
		{
			SetDlgItemText(IDC_BUTTON_LOCK,_T("Lock"));
			EnableAllSettingCtrl(TRUE);
		}
		else
		{
			MessageBox(_T("Password error"),_T("Error"),MB_OK| MB_ICONERROR);
		}
		SetDlgItemText(IDC_EDIT_PASSWORD,NULL);
	}
}

void CTMonitorDlg::EnableAllSettingCtrl( BOOL bEnable /*= TRUE*/ )
{
	GetDlgItem(IDC_CHECK_AUTO_SAVE)->EnableWindow(bEnable);
	EnableAutoSaveCtrl(((CButton*)GetDlgItem(IDC_CHECK_AUTO_SAVE))->GetCheck());
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_SAVE_AS)->EnableWindow(bEnable);
	GetDlgItem(IDC_TEXT_SERIAL_PORT)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_COM)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(bEnable);
}

int CTMonitorDlg::GetSystemPorts( CStringArray &strArrayName, CStringArray &strArrayPort )
{
	CRegKey RegKey;   
	int nCount = 0;   

	if(RegKey.Open(HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm")) == ERROR_SUCCESS)   
	{   
		while(true)   
		{   
			TCHAR szValueName[MAX_PATH] = {NULL};   
			BYTE  byValueData[MAX_PATH] = {NULL};   
			DWORD dwValueSize = MAX_PATH;   
			DWORD dwDataSize = MAX_PATH;   
			DWORD nType;   

			if(::RegEnumValue(HKEY(RegKey), nCount, szValueName, &dwValueSize, NULL, &nType, byValueData, &dwDataSize) == ERROR_NO_MORE_ITEMS)   
			{   
				break;
			}   

			strArrayName.Add(szValueName);
			strArrayPort.Add((LPCTSTR)byValueData);

			nCount++;   
		}   
	}   

	return nCount;   
}


void CTMonitorDlg::OnBnClickedButtonConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strConnect;
	GetDlgItemText(IDC_BUTTON_CONNECT,strConnect);
	if (strConnect == _T("Connect"))
	{
		CString strComName ;
		m_comboCom.GetWindowText(strComName);
		strComName.Trim();
		CString strPortNum = strComName.Right(strComName.GetLength()-3);

		DWORD dwPortNum = _ttoi(strPortNum);

		// Ĭ��9600,8,n,1
		if (!m_SerialPort.Open(dwPortNum))
		{
			AfxMessageBox(GetErrorMessage(m_SerialPort.GetErrorCode()));
			return;
		}

		EnableAutoSaveCtrl(FALSE);
		GetDlgItem(IDC_CHECK_AUTO_SAVE)->EnableWindow(FALSE);

		SetDlgItemText(IDC_BUTTON_CONNECT,_T("Disconnect"));

		m_PageLog.SetAutoSave(m_bCheckAutoSave);

		CString strLogPath;
		if (m_bCheckAutoSave)
		{
			CString strLogFileName;
			GetDlgItemText(IDC_TEXT_FILE_NAME,strLogFileName);
			strLogFileName.TrimLeft(_T("File Name:"));
			strLogFileName.Trim();

			GetDlgItemText(IDC_EDIT_LOCATION,strLogPath);
			strLogPath.Trim();
			strLogPath.TrimRight(_T('\\'));
			strLogPath += _T("\\") + strLogFileName;
		}
		else
		{
			strLogPath = m_strAppPath + TEMP_FILE;
		}

		m_PageLog.SetLogPath(strLogPath);

		m_bConnected = TRUE;

	}
	else
	{
		m_bConnected = FALSE;
		m_SerialPort.Close();
		SetDlgItemText(IDC_BUTTON_CONNECT,_T("Connect"));
		EnableAutoSaveCtrl(m_bCheckAutoSave);
		GetDlgItem(IDC_CHECK_AUTO_SAVE)->EnableWindow(TRUE);
	}
}

void CTMonitorDlg::OnAbout()
{
	// TODO: �ڴ���������������
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

CString CTMonitorDlg::GetAppVersion( CString strPath )
{
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo = NULL;
	CString strVersion ;
	dwVerInfoSize = GetFileVersionInfoSize(strPath, &dwVerHnd);

	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it:
		HANDLE			hMem;
		LPVOID			lpvMem;
		unsigned int	uInfoSize = 0;

		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = GlobalLock(hMem);
		GetFileVersionInfo(strPath, dwVerHnd, dwVerInfoSize, lpvMem);

		::VerQueryValue(lpvMem, (LPTSTR)_T("\\"), (void**)&pFileInfo, &uInfoSize);

		WORD wProdVersion[4];

		// Product version from the FILEVERSION of the version info resource 
		wProdVersion[0] = HIWORD(pFileInfo->dwProductVersionMS); 
		wProdVersion[1] = LOWORD(pFileInfo->dwProductVersionMS);
		wProdVersion[2] = HIWORD(pFileInfo->dwProductVersionLS);
		wProdVersion[3] = LOWORD(pFileInfo->dwProductVersionLS); 

		strVersion.Format(_T("%d.%d.%d.%d"),wProdVersion[0],
			wProdVersion[1],wProdVersion[2],wProdVersion[3]);

		GlobalUnlock(hMem);
		GlobalFree(hMem);

	}

	return strVersion;
}

CString * CTMonitorDlg::SplitString( CString str, char split, int& iSubStrs )
{
	int iPos = 0; //�ָ��λ��
	int iNums = 0; //�ָ��������
	CString strTemp = str;
	CString strRight;
	//�ȼ������ַ���������
	while (iPos != -1)
	{
		iPos = strTemp.Find(split);
		if (iPos == -1)
		{
			break;
		}
		strRight = strTemp.Mid(iPos + 1, str.GetLength());
		strTemp = strRight;
		iNums++;
	}
	if (iNums == 0) //û���ҵ��ָ��
	{
		//���ַ����������ַ�������
		iSubStrs = 1; 
		return NULL;
	}
	//���ַ�������
	iSubStrs = iNums + 1; //�Ӵ������� = �ָ������ + 1
	CString* pStrSplit;
	pStrSplit = new CString[iSubStrs];
	strTemp = str;
	CString strLeft;
	for (int i = 0; i < iNums; i++)
	{
		iPos = strTemp.Find(split);
		//���Ӵ�
		strLeft = strTemp.Left(iPos);
		//���Ӵ�
		strRight = strTemp.Mid(iPos + 1, strTemp.GetLength());
		strTemp = strRight;
		pStrSplit[i] = strLeft;
	}
	pStrSplit[iNums] = strTemp;
	return pStrSplit;
}


void CTMonitorDlg::OnCbnDropdownComboCom()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// ����
	CStringArray strArrayName,strArrayPort;	
	GetSystemPorts(strArrayName,strArrayPort);

	int nPortCount = m_comboCom.GetCount();
	if (nPortCount != strArrayPort.GetCount())
	{
		m_comboCom.ResetContent();
		for (int i = 0;i<strArrayPort.GetCount();i++)
		{
			m_comboCom.AddString(strArrayPort.GetAt(i));
		}
	}
	else
	{
		BOOL bDifferent = FALSE;
		for (int i = 0;i<strArrayPort.GetCount();i++)
		{
			CString strItem;
			m_comboCom.GetLBText(i,strItem);
			if (strItem.Compare(strArrayPort.GetAt(i)) != 0)
			{
				bDifferent = TRUE;
				break;
			}
		}

		if (bDifferent)
		{
			m_comboCom.ResetContent();
			for (int i = 0;i<strArrayPort.GetCount();i++)
			{
				m_comboCom.AddString(strArrayPort.GetAt(i));
			}
		}
	}
}


afx_msg LRESULT CTMonitorDlg::OnOnComReceive(WPARAM wParam, LPARAM lParam)
{
	DWORD dwBufferLength = m_SerialPort.GetInBufferLength()+1;
	char *pBuffer = new char[dwBufferLength];
	memset(pBuffer,0,dwBufferLength);
	m_SerialPort.Read(pBuffer,dwBufferLength);

	USES_CONVERSION;
	m_strReadLine += CString(A2W(pBuffer));	

	if (m_strReadLine.Find('\n') != -1)
	{
		int nStrNums = 0;
		CString* pStr;
		pStr = SplitString(m_strReadLine,'\n',nStrNums);

		// ������һ�����ǻ��з���˵�����滹����һ���Ĳ�����Ϣ
		if (m_strReadLine.ReverseFind('\n') != m_strReadLine.GetLength()-1)
		{
			nStrNums--;
			m_strReadLine = pStr[nStrNums];
		}
		else
		{
			m_strReadLine.Empty();
		}

		for (int i = 0; i<nStrNums;i++)
		{
			pStr[i].Trim();
			if (!pStr[i].IsEmpty())
			{
				if (pStr[i].Find(_T("##STARTTASK::")) != -1)
				{
					m_PageLog.ClearLog();
				}
				m_PageLog.AppendLogText(pStr[i]);
			}					
		}
	}
	delete []pBuffer;
	return 0;
}

CString CTMonitorDlg::GetErrorMessage( DWORD dwErrorCode )
{
	LPTSTR lpMsgBuffer;
	CString strErrorMsg;
	FormatMessage(  
		FORMAT_MESSAGE_ALLOCATE_BUFFER |   
		FORMAT_MESSAGE_FROM_SYSTEM |  
		FORMAT_MESSAGE_IGNORE_INSERTS,  
		NULL,  
		dwErrorCode,  
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  
		(LPTSTR) &lpMsgBuffer,  
		0, NULL ); 

	strErrorMsg = CString(lpMsgBuffer);
	LocalFree(lpMsgBuffer);

	return strErrorMsg;
}


void CTMonitorDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CString strTempPath = m_strAppPath + TEMP_FILE;
	if (PathFileExists(strTempPath))
	{
		DeleteFile(strTempPath);
	}
	CDialogEx::OnClose();
}


BOOL CTMonitorDlg::PreTranslateMessage(MSG* pMsg)
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


void CTMonitorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������

	CWnd *pWnd = NULL;
	pWnd = GetWindow(GW_CHILD);
	while (pWnd)
	{
		ChangeSize(pWnd,cx,cy);

		if (pWnd->GetDlgCtrlID() == m_tabMain.GetDlgCtrlID())
		{
			CRect rectTab;
			m_tabMain.GetClientRect(&rectTab);

			CRect rcHead;
			m_tabMain.GetItemRect(0,&rcHead);
			rectTab.top += rcHead.Height() + 2;
			rectTab.left += 2;
			rectTab.right -= 2;
			rectTab.bottom -= 2;
			m_PageLog.MoveWindow(rectTab);
		}
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
	GetClientRect(&m_Rect);
}

void CTMonitorDlg::ChangeSize( CWnd *pWnd, int cx, int cy )
{
	if(pWnd)  //�ж��Ƿ�Ϊ�գ���Ϊ�Ի��򴴽�ʱ����ô˺���������ʱ�ؼ���δ����   
	{  
		CRect rect;   //��ȡ�ؼ��仯ǰ�Ĵ�С    
		pWnd->GetWindowRect(&rect);  
		ScreenToClient(&rect);//���ؼ���Сת��Ϊ�ڶԻ����е���������  

		//    cx/m_rect.Width()Ϊ�Ի����ں���ı仯����  
		rect.left=rect.left*cx/m_Rect.Width();//�����ؼ���С  
		rect.right=rect.right*cx/m_Rect.Width();  
		rect.top=rect.top*cy/m_Rect.Height();  
		rect.bottom=rect.bottom*cy/m_Rect.Height();  
		pWnd->MoveWindow(rect);//���ÿؼ���С 
	}  
}
