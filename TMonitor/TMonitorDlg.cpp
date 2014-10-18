
// TMonitorDlg.cpp : implementation file
//
// v1.0.1.0 2013-12-10 解决当系统串口改变时不能及时更新串口的问题。
// v1.0.2.0 2013-12-30 修改记录LOG方式。
// v1.0.3.0 2014-01-21 1.当收到##STARTTASK::标志时，自动清屏
//                     2.修改设置密码方式
//                     3.界面可以最大化显示
// v2.0.0.0 2014-05-16 1.增加Device Status页面和Summary List页面
//          2014-05-17 2.增加Data Report页面
//          2014-05-20 3.增加上传SN时导出SN功能
// v2.0.1.0 2014-05-22 1.增加Export SN页面
//                     2.增加中文版
// v2.0.2.0 2014-05-23 1.增加禁止SN重复功能，一旦重复就会报错，并发出警报
// v2.0.3.0 2014-05-27 1.修改查询开始时间为最小时间，查询结束时间为最大时间。
// v2.0.4.0 2014-06-25 1.兼容TF-147机种
// v2.0.5.0 2014-06-26 1.修改查询开始时间为程序启动时间
//                     2.导出Excel表时，把MachineId和SN设置为文本格式
// v2.0.6.0 2014-09-15 1.修改界面显示，可以抓取卡的插入和拔出状态
//                     2.数据报表界面中，可以对查询结果进行排序，显示/隐藏项，序列号可编辑
// v2.0.7.0 2014-09-22 1.解决由于修改数据报表查询导致数据统计出错的问题。
// v2.0.8.0 2014-09-28 1.界面上显示卡的序列号
//                     2.每次开始执行拷贝的时候输出机器序列号、分位等信息。
//                     3.为了让显示的LOG更简洁，显示的LOG记录中不记录Speed、Percent信息。
// v2.0.9.0 2014-10-13 1.解决程序未用管理员权限打开，关闭程序时报参数错误的问题。
//                     2.当检测到设备是TS-147时，程序自动最大化显示。
//                     3.界面调整。
// v2.1.0.0 2014-10-15 1.界面语言可以在配置文件中配置，如果不配置则默认根据系统语言来选择
//                     2.新增机器型号选择功能，根据型号显示对应画面
// v2.1.1.0 2014-10-18 1.新增手动选择破特率功能，兼容波特率高的机种。

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

	// TODO:  在此添加额外的初始化
	//修改标题
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
	// 异常: OCX 属性页应返回 FALSE
}


// CTMonitorDlg dialog




CTMonitorDlg::CTMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTMonitorDlg::IDD, pParent)
	, m_bCheckAutoSave(FALSE)
	, m_bCheckDate(FALSE)
	, m_bCheckTime(FALSE)
	, m_bConnected(FALSE)
	, m_bLocked(FALSE)
	, m_bCheckForbidSN(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_PageDevice.SetMachineInfo(&m_Machine);
	m_PageList.SetMachineInfo(&m_Machine);
	m_PageReport.SetMySlotData(&m_SlotData);
	m_PageExportSN.SetMySlotData(&m_SlotData);
	m_nSlotCount = 24;
}

void CTMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_tabMain);
	DDX_Check(pDX, IDC_CHECK_AUTO_SAVE, m_bCheckAutoSave);
	DDX_Check(pDX, IDC_CHECK_DATE, m_bCheckDate);
	DDX_Check(pDX, IDC_CHECK_TIME, m_bCheckTime);
	DDX_Control(pDX, IDC_COMBO_COM, m_comboCom);
	DDX_Control(pDX, IDC_COMBO_MODULE, m_comboModel);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_comboBaudRate);
	DDX_Check(pDX, IDC_CHECK_DISABLE_SN_REPEAT, m_bCheckForbidSN);
}

BEGIN_MESSAGE_MAP(CTMonitorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CTMonitorDlg::OnTcnSelchangeTabMain)
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
	ON_MESSAGE(WM_UPDATE_RESULT, &CTMonitorDlg::OnUpdateResult)
	ON_MESSAGE(WM_SN_REPEATE, &CTMonitorDlg::OnSnRepeate)
	ON_CBN_SELCHANGE(IDC_COMBO_MODULE, &CTMonitorDlg::OnCbnSelchangeComboModule)
	ON_CBN_EDITCHANGE(IDC_COMBO_BAUDRATE, &CTMonitorDlg::OnCbnEditchangeComboBaudrate)
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

	//修改标题
	CString strPath;
	GetModuleFileName(NULL,strPath.GetBuffer(MAX_PATH),MAX_PATH);
	strPath.ReleaseBuffer();

	// 获取程序路径
	m_strAppPath = strPath.Left(strPath.ReverseFind(_T('\\')));  //不包含'\'

	CString strTempPath = m_strAppPath + TEMP_FILE;
	if (PathFileExists(strTempPath))
	{
		DeleteFile(strTempPath);
	}

	CString strVersion = GetAppVersion(strPath);

	CString strTitle = PRJ_NAME + strVersion;
	SetWindowText(strTitle);

	// 读配置文件
	m_Ini.SetPathName(m_strAppPath + CONFIG_FILE);
	m_bCheckAutoSave = m_Ini.GetBool(_T("LOG Setting"),_T("EN_AUTO_SAVE"),FALSE);
	m_bCheckDate = m_Ini.GetBool(_T("LOG Setting"),_T("EN_DATE"),FALSE);
	m_bCheckTime = m_Ini.GetBool(_T("LOG Setting"),_T("EN_TIME"),FALSE);
	CString strPrefix = m_Ini.GetString(_T("LOG Setting"),_T("PREFIX"));
	strPrefix.Trim();
	CString strLocation = m_Ini.GetString(_T("LOG Setting"),_T("LOCATION"));
	strLocation.Trim();
	CString strSerialPort = m_Ini.GetString(_T("ComPort Setting"),_T("SERIAL_PORT"));
	strSerialPort.Trim();
	DWORD dwBaudRate = m_Ini.GetUInt(_T("ComPort Setting"),_T("BAUDRATE"),9600);
	BOOL bAutoConnect = m_Ini.GetBool(_T("ComPort Setting"),_T("EN_AUTO_CONNECT"),FALSE);
	CString strDBPath = m_Ini.GetString(_T("DB Setting"),_T("DB_PATH"),_T("C:\\PHIYO"));
	strDBPath.TrimRight(_T('\\'));
	m_SlotData.InitialDatabase(strDBPath);

	CString strAlias = m_Ini.GetString(_T("Machine Info"),_T("ALIAS"));
	m_bCheckForbidSN = m_Ini.GetBool(_T("Machine Info"),_T("EN_FORBID_SN_REPEAT"),FALSE);

	SetDlgItemText(IDC_EDIT_PREFIX,strPrefix);
	SetDlgItemText(IDC_EDIT_LOCATION,strLocation);
	SetDlgItemText(IDC_EDIT_ALIAS,strAlias);
	
	UpdateData(FALSE);

	//为Tab添加3个页面
	CString strText;

	strText.LoadString(IDS_PAGE_LOG_MESSAGE);
	m_tabMain.InsertItem(0,strText);

	strText.LoadString(IDS_PAGE_DEVICE_STATUS);
	m_tabMain.InsertItem(1,strText);

	strText.LoadString(IDS_PAGE_SUMMARY_LIST);
	m_tabMain.InsertItem(2,strText);

	strText.LoadString(IDS_PAGE_DATA_REPORT);
	m_tabMain.InsertItem(3,strText);

	strText.LoadString(IDS_PAGE_EXPORT_SN);
	m_tabMain.InsertItem(4,strText);

	//创建3个对话框
	m_PageLog.Create(IDD_DIALOG_LOG,&m_tabMain);
	m_PageDevice.Create(IDD_DIALOG_DEVICE_STATUS,&m_tabMain);
	m_PageList.Create(IDD_DIALOG_DEVICE_LIST,&m_tabMain);
	m_PageReport.Create(IDD_DIALOG_DATA_REPORT,&m_tabMain);
	m_PageExportSN.Create(IDD_DIALOG_EXPORT_SN,&m_tabMain);

	//显示初始界面
	m_PageLog.ShowWindow(SW_HIDE);
	m_PageDevice.ShowWindow(SW_SHOW);
	m_PageList.ShowWindow(SW_HIDE);
	m_PageReport.ShowWindow(SW_HIDE);
	m_PageExportSN.ShowWindow(SW_HIDE);
	m_nCurSelTab = 1;

	m_tabMain.SetCurSel(m_nCurSelTab);

	//保存对话框页面指针
	m_pDlg[0] = &m_PageLog;
	m_pDlg[1] = &m_PageDevice;
	m_pDlg[2] = &m_PageList;
	m_pDlg[3] = &m_PageReport;
	m_pDlg[4] = &m_PageExportSN;

	m_brush.CreateSolidBrush(RGB(102,205,170));

	// 初始化AutoSave
	EnableAutoSaveCtrl(m_bCheckAutoSave);
	CTime time = CTime::GetCurrentTime();

	strText.LoadString(IDS_FORMAT_DATE);
	CString strDate = time.Format(strText);

	strText.LoadString(IDS_FORMAT_TIME);
	CString strTime = time.Format(strText); 

	SetDlgItemText(IDC_CHECK_DATE,strDate);
	SetDlgItemText(IDC_CHECK_TIME,strTime);

	// 串口
	CStringArray strArrayName,strArrayPort;	
	GetSystemPorts(strArrayName,strArrayPort);

	int nSelectIndex = 0;
	int nCount = strArrayPort.GetCount();
	for (int i = 0;i<nCount;i++)
	{
		CString strPort = strArrayPort.GetAt(i);
		if (strPort.CompareNoCase(strSerialPort) == 0)
		{
			nSelectIndex = i;
		}
		m_comboCom.AddString(strPort);
	}

	m_comboCom.SetCurSel(nSelectIndex);

	int nItem = m_comboModel.AddString(_T("TS-123"));
	m_comboModel.SetItemData(nItem,TS_123);
	
	nItem = m_comboModel.AddString(_T("TS-147"));
	m_comboModel.SetItemData(nItem,TS_147);

	nItem = m_comboModel.AddString(_T("TF-147"));
	m_comboModel.SetItemData(nItem,TF_147);

	m_comboModel.SetCurSel(0);
	m_nSlotPerRow = 4;
	m_nSlotCount = 24;

	m_comboBaudRate.AddString(_T("9600"));
	m_comboBaudRate.AddString(_T("115200"));
	SetDlgItemInt(IDC_COMBO_BAUDRATE,dwBaudRate);


	// 开启定时器
	SetTimer(TIMER_TIME,1000,NULL);

	m_hEvent = CreateEvent(NULL,FALSE,TRUE,NULL);

	if (bAutoConnect)
	{
		OnBnClickedButtonConnect();
	}

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



void CTMonitorDlg::OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	m_pDlg[m_nCurSelTab]->ShowWindow(SW_HIDE);
	m_nCurSelTab = m_tabMain.GetCurSel();
	m_pDlg[m_nCurSelTab]->ShowWindow(SW_SHOW);
	*pResult = 0;
}


HBRUSH CTMonitorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if(nCtlColor != CTLCOLOR_BTN && nCtlColor != CTLCOLOR_EDIT && nCtlColor != CTLCOLOR_LISTBOX)
	{
		pDC->SetBkMode(TRANSPARENT);
		return m_brush;
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CTMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString strText;
	switch (nIDEvent)
	{
	case TIMER_TIME:
		CTime time = CTime::GetCurrentTime();
		strText.LoadString(IDS_FORMAT_DATE);
		CString strDate = time.Format(strText);

		strText.LoadString(IDS_FORMAT_TIME);
		CString strTime = time.Format(strText); 

		SetDlgItemText(IDC_CHECK_DATE,strDate);
		SetDlgItemText(IDC_CHECK_TIME,strTime);

		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CTMonitorDlg::OnBnClickedCheckAutoSave()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateFileName();
}


void CTMonitorDlg::OnBnClickedCheckDate()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	UpdateFileName();
}


void CTMonitorDlg::OnBnClickedCheckTime()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	UpdateFileName();
}

void CTMonitorDlg::UpdateFileName()
{
	CString strFileName,strPrefix;
	GetDlgItemText(IDC_EDIT_PREFIX,strPrefix);
	strPrefix.Trim();

	CString strText;
	strText.LoadString(IDS_FILE_NAME);
	strFileName +=strText;
	strFileName += strPrefix;

	CTime time = CTime::GetCurrentTime();
	strText.LoadString(IDS_FORMAT_DATE);
	CString strDate = time.Format(strText);

	strText.LoadString(IDS_FORMAT_TIME);
	CString strTime = time.Format(strText); 

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
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO broInfo = {0};
	TCHAR      szDisName[MAX_PATH] = {0};

	UINT    ulFalgs = BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN
		| BIF_BROWSEFORCOMPUTER | BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;

	broInfo.hwndOwner = this->m_hWnd;
	broInfo.pidlRoot  = NULL;
	broInfo.pszDisplayName = szDisName;
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
	// TODO: 在此添加控件通知处理程序代码
	m_PageLog.ClearLog();
}


void CTMonitorDlg::OnBnClickedButtonSaveAs()
{
	// TODO: 在此添加控件通知处理程序代码
	m_PageLog.SaveLog();
}


void CTMonitorDlg::OnBnClickedButtonLock()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPassword;
	GetDlgItemText(IDC_EDIT_PASSWORD,strPassword);
	strPassword.Trim();

	CString strText;

	if (!m_bLocked)
	{
		if (strPassword.IsEmpty())
		{
			m_strPassWord = DEFAULT_PASSWORD;
		}
		else
		{
			m_strPassWord = strPassword;
		}

		m_bLocked = TRUE;

		SetDlgItemText(IDC_EDIT_PASSWORD,NULL);

		strText.LoadString(IDS_UNLOCK);
		SetDlgItemText(IDC_BUTTON_LOCK,strText);

		EnableAllSettingCtrl(FALSE);
	}
	else
	{
		if (strPassword == m_strPassWord || strPassword == DEFAULT_PASSWORD)
		{
			strText.LoadString(IDS_LOCK);
			SetDlgItemText(IDC_BUTTON_LOCK,strText);

			EnableAllSettingCtrl(TRUE);

			m_bLocked = FALSE;
		}
		else
		{
			CString strMsg,strTitle;
			strMsg.LoadString(IDS_MSG_PASSWORD_ERROR);
			strTitle.LoadString(IDS_MSG_TITAL_ERROR);

			MessageBox(strMsg,strTitle,MB_OK| MB_ICONERROR);
		}
		SetDlgItemText(IDC_EDIT_PASSWORD,NULL);
	}
}

void CTMonitorDlg::EnableAllSettingCtrl( BOOL bEnable /*= TRUE*/ )
{
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_SAVE_AS)->EnableWindow(bEnable);

	if (m_bConnected)
	{
		GetDlgItem(IDC_COMBO_COM)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ALIAS)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_DISABLE_SN_REPEAT)->EnableWindow(FALSE);
		EnableAutoSaveCtrl(FALSE);
		m_comboBaudRate.EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_COM)->EnableWindow(bEnable);
		GetDlgItem(IDC_EDIT_ALIAS)->EnableWindow(bEnable);
		GetDlgItem(IDC_CHECK_DISABLE_SN_REPEAT)->EnableWindow(bEnable);
		EnableAutoSaveCtrl(((CButton*)GetDlgItem(IDC_CHECK_AUTO_SAVE))->GetCheck());
		m_comboBaudRate.EnableWindow(bEnable);
	}
	
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(bEnable);

	m_PageReport.EnableControls(bEnable);
	m_PageExportSN.EnableControls(bEnable);
	m_PageList.EnableControls(bEnable);
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
	// TODO: 在此添加控件通知处理程序代码

	CString strText;
	if (!m_bConnected)
	{
		CString strComName ;
		m_comboCom.GetWindowText(strComName);
		strComName.Trim();
		CString strPortNum = strComName.Right(strComName.GetLength()-3);

		DWORD dwPortNum = _ttoi(strPortNum);
		CString strBaudTate;
		m_comboBaudRate.GetWindowText(strBaudTate);
		DWORD dwBaudRate = _ttoi(strBaudTate);

		// 默认9600,8,n,1
		if (!m_SerialPort.Open(dwPortNum,dwBaudRate))
		{
			AfxMessageBox(GetErrorMessage(m_SerialPort.GetErrorCode()));
			return;
		}

		UpdateData(TRUE);
		EnableAutoSaveCtrl(FALSE);
		GetDlgItem(IDC_CHECK_AUTO_SAVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ALIAS)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_DISABLE_SN_REPEAT)->EnableWindow(FALSE);
		GetDlgItemText(IDC_EDIT_ALIAS,m_strAlias);

		m_comboCom.EnableWindow(FALSE);
		m_comboBaudRate.EnableWindow(FALSE);

		strText.LoadString(IDS_DISCONNECT);
		SetDlgItemText(IDC_BUTTON_CONNECT,strText);

		m_PageLog.SetAutoSave(m_bCheckAutoSave);

		CString strLogPath;
		if (m_bCheckAutoSave)
		{
			CString strLogFileName;
			GetDlgItemText(IDC_TEXT_FILE_NAME,strLogFileName);

			strText.LoadString(IDS_FILE_NAME);
			strLogFileName.TrimLeft(strText);
			strLogFileName.Trim();

			GetDlgItemText(IDC_EDIT_LOCATION,strLogPath);
			strLogPath.Trim();
			strLogPath.TrimRight(_T('\\'));

			if (!PathFileExists(strLogPath))
			{
				SHCreateDirectoryEx(NULL,strLogPath,NULL);
			}

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
		strText.LoadString(IDS_CONNECT);
		SetDlgItemText(IDC_BUTTON_CONNECT,strText);
		EnableAutoSaveCtrl(m_bCheckAutoSave);
		GetDlgItem(IDC_CHECK_AUTO_SAVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_ALIAS)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_DISABLE_SN_REPEAT)->EnableWindow(TRUE);
		m_comboCom.EnableWindow(TRUE);
		m_comboBaudRate.EnableWindow(TRUE);
	}
}

void CTMonitorDlg::OnAbout()
{
	// TODO: 在此添加命令处理程序代码
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
	int iPos = 0; //分割符位置
	int iNums = 0; //分割符的总数
	CString strTemp = str;
	CString strRight;
	//先计算子字符串的数量
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
	if (iNums == 0) //没有找到分割符
	{
		//子字符串数就是字符串本身
		iSubStrs = 1; 
		return NULL;
	}
	//子字符串数组
	iSubStrs = iNums + 1; //子串的数量 = 分割符数量 + 1
	CString* pStrSplit;
	pStrSplit = new CString[iSubStrs];
	strTemp = str;
	CString strLeft;
	for (int i = 0; i < iNums; i++)
	{
		iPos = strTemp.Find(split);
		//左子串
		strLeft = strTemp.Left(iPos);
		//右子串
		strRight = strTemp.Mid(iPos + 1, strTemp.GetLength());
		strTemp = strRight;
		pStrSplit[i] = strLeft;
	}
	pStrSplit[iNums] = strTemp;
	return pStrSplit;
}


void CTMonitorDlg::OnCbnDropdownComboCom()
{
	// TODO: 在此添加控件通知处理程序代码
	// 串口
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
	if (m_SerialPort.GetInBufferLength() > 0)
	{
		AfxBeginThread((AFX_THREADPROC)ComReceiveThreadProc,this);
	}
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString strTempPath = m_strAppPath + TEMP_FILE;
	if (PathFileExists(strTempPath))
	{
		DeleteFile(strTempPath);
	}

	// 写配置文件
	UpdateData(TRUE);
	m_Ini.WriteBool(_T("LOG Setting"),_T("EN_AUTO_SAVE"),m_bCheckAutoSave);
	m_Ini.WriteBool(_T("LOG Setting"),_T("EN_DATE"),m_bCheckDate);
	m_Ini.WriteBool(_T("LOG Setting"),_T("EN_TIME"),m_bCheckTime);
	CString strPrefix,strLocation,strSerialPort,strAlias;
	GetDlgItemText(IDC_EDIT_PREFIX,strPrefix);
	GetDlgItemText(IDC_EDIT_LOCATION,strLocation);

	if (m_comboCom.GetCurSel() != -1)
	{
		m_comboCom.GetLBText(m_comboCom.GetCurSel(),strSerialPort);
		m_Ini.WriteString(_T("ComPort Setting"),_T("SERIAL_PORT"),strSerialPort);
	}

	CString strBaudRate;
	m_comboBaudRate.GetWindowText(strBaudRate);
	m_Ini.WriteString(_T("ComPort Setting"),_T("BAUDRATE"),strBaudRate);
	
	m_Ini.WriteString(_T("LOG Setting"),_T("PREFIX"),strPrefix);
	m_Ini.WriteString(_T("LOG Setting"),_T("LOCATION"),strLocation);

	GetDlgItemText(IDC_EDIT_ALIAS,strAlias);
	m_Ini.WriteString(_T("Machine Info"),_T("ALIAS"),strAlias);
	m_Ini.WriteBool(_T("Machine Info"),_T("EN_FORBID_SN_REPEAT"),m_bCheckForbidSN);

// 	// 提示是否需要保存Summary List
// 	if (IDYES == MessageBox(_T("You are exiting the program now !\r\nDo you want to save the summary list to excel ?"),_T("Tips"),MB_YESNO | MB_DEFBUTTON1 | MB_ICONINFORMATION))
// 	{
// 		::SendMessage(m_PageList.GetSafeHwnd(),WM_EXPORT_SUMMARY_LIST,0,0);
// 	}

	CDialogEx::OnClose();
}


BOOL CTMonitorDlg::PreTranslateMessage(MSG* pMsg)
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


void CTMonitorDlg::OnSize(UINT nType, int cx, int cy)
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
		DWORD dwFlag = 0;
		if (pWnd->GetDlgCtrlID() == IDC_TAB_MAIN)
		{
			dwFlag = SIZE_ELASTIC_X_EX | SIZE_ELASTIC_Y_EX;
		}
		else if (pWnd->GetDlgCtrlID() == IDC_STATIC)
		{
			dwFlag = SIZE_MOVE_Y | SIZE_ELASTIC_Y;
		}
		else
		{
			dwFlag = SIZE_MOVE_Y;
		}


		ChangeSize(pWnd,cx,cy,dwFlag);

		if (pWnd->GetDlgCtrlID() == IDC_TAB_MAIN)
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
			m_PageDevice.MoveWindow(rectTab);
			m_PageList.MoveWindow(rectTab);
			m_PageReport.MoveWindow(rectTab);
			m_PageExportSN.MoveWindow(rectTab);
		}
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
	GetClientRect(&m_Rect);
}

void CTMonitorDlg::ChangeSize( CWnd *pWnd, int cx, int cy,DWORD flag)
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
			iLeft = iLeft * cx / m_Rect.Width();
		}

		// 改变Y坐标
		if ((flag & SIZE_MOVE_Y) == SIZE_MOVE_Y)
		{
			iTop = iTop * cy / m_Rect.Height();
		}

		//改变宽度
		if ((flag & SIZE_ELASTIC_X) == SIZE_ELASTIC_X)
		{
			iWidth = iWidth * cx / m_Rect.Width();
		}

		// 改变高度
		if ((flag & SIZE_ELASTIC_Y) == SIZE_ELASTIC_Y)
		{
			iHeight = iHeight * cy / m_Rect.Height();
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

DWORD WINAPI CTMonitorDlg::ComReceiveThreadProc( LPVOID lpParm )
{
	CTMonitorDlg *pDlg = (CTMonitorDlg *)lpParm;
	pDlg->OnReceive();
	return 0;
}

void CTMonitorDlg::OnReceive()
{
	WaitForSingleObject(m_hEvent,INFINITE);
	DWORD dwBufferLength = m_SerialPort.GetInBufferLength()+1;

	char *pBuffer = new char[dwBufferLength];
	memset(pBuffer,0,dwBufferLength);
	m_SerialPort.Read(pBuffer,dwBufferLength);

	USES_CONVERSION;
	m_strReadLine += CString(A2W(pBuffer));	

	static bool bSlog = false;
	static bool bSpeed = false;
	static bool bSummary = false;
	static bool bSN = false;
	static bool bBootStart = false;

	if (m_strReadLine.Find('\n') != -1)
	{
		int nStrNums = 0;
		CString* pStr;
		pStr = SplitString(m_strReadLine,'\n',nStrNums);

		// 如果最后一个不是换行符，说明后面还有下一条的部分信息
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
			CString strMessage = pStr[i].Trim();
			if (!strMessage.IsEmpty())
			{
				// Speed 和 Percent不保存

				if (strMessage.Find(_T("#Start speed:")) == -1 
					&& strMessage.Find(_T("Speed(")) == -1
					&& strMessage.Find(_T("Percent")) == -1)
				{
					m_PageLog.AppendLogText(strMessage);
				}

				if (strMessage.Find(_T("#BOOTSTART::")) != -1)
				{
					bBootStart = true;
					continue;
				}

				//if (bBootStart)
				{
					int nPos1 = 0,nPos2 = 0;

					nPos1 = strMessage.Find(_T("##MAHINE_NAME:"));
					if (nPos1 != -1)
					{
						nPos1 += 14;
						nPos2 = strMessage.GetLength();
						m_strMachineName = strMessage.Right(nPos2-nPos1);
						m_strMachineName.Trim();
						continue;
					}

					nPos1 = strMessage.Find(_T("##MAHINE_ID:"));
					if (nPos1 != -1)
					{
						nPos1 += 12;
						nPos2 = strMessage.GetLength();
						m_strMachineID= strMessage.Right(nPos2-nPos1);
						m_strMachineID.Trim();

						CString strText,strMachineID;
						strText.LoadString(IDS_FORMAT_MACHINE_ID);
						strMachineID.Format(strText,m_strMachineID);

						SetDlgItemText(IDC_TEXT_MACHINE_ID,strMachineID);

						m_Machine.SetMachineID(m_strMachineID);
						continue;
					}

					nPos1 = strMessage.Find(_T("##FW_VER:"));
					if (nPos1 != -1)
					{
						nPos1 += 9;
						nPos2 = strMessage.GetLength();
						m_strFirmware= strMessage.Right(nPos2-nPos1);
						m_strFirmware.Trim();

						CString strText,strFirmware;
						strText.LoadString(IDS_FORMAT_FIRMWARE);
						strFirmware.Format(strText,m_strFirmware);

						SetDlgItemText(IDC_TEXT_FIRMWARE,strFirmware);
						continue;
					}

					nPos1 = strMessage.Find(_T("##BLD:"));
					if (nPos1 != -1)
					{
						nPos1 += 6;
						nPos2 = strMessage.GetLength();
						m_strBLD = strMessage.Right(nPos2-nPos1);
						m_strBLD.Trim();

						CString strText,strBLD;
						strText.LoadString(IDS_FORMAT_BLD);
						strBLD.Format(strText,m_strBLD);

						SetDlgItemText(IDC_TEXT_BLD,strBLD);
						continue;
					}

					nPos1 = strMessage.Find(_T("##SLOT_NUM:"));
					if (nPos1 != -1)
					{
						nPos1 += 11;
						nPos2 = strMessage.GetLength();
						CString strNum = strMessage.Right(nPos2-nPos1);
						strNum.Trim();
						int nSlotCount = _ttoi(strNum);

						if (nSlotCount == 0)
						{
							nSlotCount = m_nSlotCount;
						}

						if (nSlotCount % 4)
						{
							nSlotCount++;
						}
						
						if (nSlotCount != m_nSlotCount)
						{
							m_nSlotCount = nSlotCount;
							::PostMessage(m_PageDevice.GetSafeHwnd(),WM_CHANGE_SLOT_COUNT,(WPARAM)m_nSlotCount,(LPARAM)m_nSlotPerRow);

							if (nSlotCount >= 48)
							{
								DWORD dwStyle = GetStyle();

								if ((dwStyle & WS_MAXIMIZE) != WS_MAXIMIZE)
								{
									ShowWindow(SW_MAXIMIZE);
								}
							}
						}
						
						continue;
					}
				}

				// 复位
				if (strMessage.Find(_T("##RESET")) != -1)
				{
					m_PageDevice.Reset();
				}

				// 清除上一轮信息
				if (strMessage.Find(_T("##STARTTASK::")) != -1)
				{
					m_PageLog.ClearLog();
					m_Machine.Reset();

					m_Machine.SetStartTime(CTime::GetCurrentTime());
					m_Machine.SetAlias(m_strAlias);
					m_Machine.SetMachineID(m_strMachineID);

					bSlog = false;
					bSpeed = false;
					bSummary = false;
					bSN = false;
					continue;
				}

				// 收集SN ##Upload SN
				if (strMessage.Find(_T("##Upload SN start")) != -1)
				{
					bSN = true;
					continue;
				}
				// 收集SN ##Upload SN
				if (strMessage.Find(_T("##Upload SN end")) != -1)
				{
					bSN = false;
					continue;
				}

				if (bSN)
				{
					int nSlotNum = 0;
					int nCount = 0;
					CString strSN,strCapacity;
					ULONG ulCapacitySizeMB = 0;
					int nPos1 = 0,nPos2 = 0;

					//##S/N(Slot1 Count1):24906EEB Capacity: 1913MB
					//##S/N(Slot1 Count1):24906EEB
					nPos1 = strMessage.Find(_T("##S/N(Slot"));
					if (nPos1 != -1)
					{
						nPos1 += 10;
						nPos2 = strMessage.Find(_T("Count"),nPos1);

						CString strSlotNum = strMessage.Mid(nPos1,nPos2-nPos1);
						strSlotNum.Trim();
						nSlotNum = _ttoi(strSlotNum);

						nPos1 = nPos2 + 5;
						nPos2 = strMessage.Find(_T("):"),nPos1);
						CString strCount = strMessage.Mid(nPos1,nPos2 - nPos1);
						strCount.Trim();
						nCount = _ttoi(strCount);

						// 有没有Capacity
						if (strMessage.Find(_T("Capacity:"),nPos2) == -1)
						{
							// 获取SN
							nPos1 = nPos2 + 2;
							nPos2 = strMessage.GetLength();
							strSN = strMessage.Right(nPos2-nPos1);
							strSN.Trim();
						}
						else
						{
							// 获取SN
							nPos1 = nPos2+2;
							nPos2 = strMessage.Find(_T("Capacity:"),nPos2);
							strSN = strMessage.Mid(nPos1,nPos2-nPos1);
							strSN.Trim();

							// 获取Capacity
							nPos1 = nPos2 + 9;
							nPos2 = strMessage.Find(_T("MB"));
							strCapacity = strMessage.Mid(nPos1,nPos2 - nPos1);
							strCapacity.Trim();
							ulCapacitySizeMB = (ULONG)_ttol(strCapacity);
						}

						if (m_bCheckForbidSN)
						{
							CString strSQL;
							strSQL.Format(_T("SELECT COUNT(*) FROM %s Where sn='%s';"),TS123_SN_TABLE_NAME,strSN);
							int nSNCount = m_SlotData.GetCount(strSQL);

							if (nSNCount != 0)
							{
								// 报错，黄灯
								m_PageDevice.ChangeDeviceStatus(nSlotNum,IDB_SD_YELLOW,strSN,(ULONGLONG)ulCapacitySizeMB * 1024 *1024);
								m_SlotIn.SetAt(nSlotNum,TRUE);
								PostMessage(WM_SN_REPEATE,(WPARAM)nSlotNum,(LPARAM)SD_YELLOW);
							}
							else
							{
								// 绿灯
								m_PageDevice.ChangeDeviceStatus(nSlotNum,IDB_SD_GREEN,strSN,(ULONGLONG)ulCapacitySizeMB * 1024 *1024);

								DB_SN_INFO dbSN;
								dbSN.strMachineID = m_strMachineID;
								dbSN.strAlias = m_strAlias;
								dbSN.strSN = strSN;
								dbSN.GetTime = CTime::GetCurrentTime();
								m_SlotData.AddSN(dbSN);
							}
						}
						else
						{
							// 绿灯
							m_PageDevice.ChangeDeviceStatus(nSlotNum,IDB_SD_GREEN,strSN,(ULONGLONG)ulCapacitySizeMB * 1024 *1024);

							DB_SN_INFO dbSN;
							dbSN.strMachineID = m_strMachineID;
							dbSN.strAlias = m_strAlias;
							dbSN.strSN = strSN;
							dbSN.GetTime = CTime::GetCurrentTime();
							m_SlotData.AddSN(dbSN);
						}
						
						continue;
					}

					//##Remove card:1
					nPos1 = strMessage.Find(_T("##Remove card:"));
					if (nPos1 != -1)
					{
						nPos1 += 14;
						nPos2 = strMessage.GetLength();

						CString strSlotNum = strMessage.Right(nPos2-nPos1).Trim();
						nSlotNum = _ttoi(strSlotNum);

						// 灭灯
						m_PageDevice.ChangeDeviceStatus(nSlotNum,IDB_SD_GRAY,strSN,0);

						m_SlotIn.SetAt(nSlotNum,FALSE);

						continue;
					}

					//##Bad card:1
					nPos1 = strMessage.Find(_T("##Bad card:"));
					if (nPos1 != -1)
					{
						nPos1 += 11;
						nPos2 = strMessage.GetLength();

						CString strSlotNum = strMessage.Right(nPos2-nPos1).Trim();
						nSlotNum = _ttoi(strSlotNum);

						// 红灯
						m_PageDevice.ChangeDeviceStatus(nSlotNum,IDB_SD_RED,strSN,0);
						m_SlotIn.SetAt(nSlotNum,TRUE);

						PostMessage(WM_SN_REPEATE,(WPARAM)nSlotNum,(LPARAM)SD_RED);

						continue;
					}

					continue;
				}
				else
				{
					CString strSN,strCapacity,strSlotNum;
					ULONG ulCapacitySizeMB = 0;
					int nSlotNum = 0;
					int nPos1 = 0,nPos2 = 0;
					// 检测插卡
					// ##Insert card:1
					if (strMessage.Find(_T("##Insert card")) != -1 )
					{
						nPos1 = strMessage.Find(_T("##Insert card:"));
						nPos2 = strMessage.Find(_T(":"),nPos1);

						if (nPos2 != -1)
						{
							strSlotNum = strMessage.Mid(nPos2 + 1);

							strSlotNum.Trim();

							nSlotNum = _ttoi(strSlotNum);

							m_PageDevice.ChangeDeviceStatus(nSlotNum,IDB_SD_NORMAL,strSN,0);
						}
					}

					//##Card( 1) S/N:24906EEB Capacity: 1913MB
					if (strMessage.Find(_T("##Card")) != -1)
					{
						// 获取SlotNum
						nPos1 = strMessage.Find(_T("(")) + 1;
						nPos2 = strMessage.Find(_T(")"));

						strSlotNum = strMessage.Mid(nPos1,nPos2-nPos1);
						strSlotNum.Trim();

						nSlotNum = _ttoi(strSlotNum);
						
						// 获取SN
						nPos1 = strMessage.Find(_T("S/N:"),nPos2);
						nPos2 = strMessage.Find(_T("Capacity:"),nPos2);
						if (nPos1 != -1 && nPos2 != -1)
						{
							nPos1 += 4;
							strSN = strMessage.Mid(nPos1,nPos2 - nPos1);
							strSN.Trim();
						}

						// 获取Capacity
						if (nPos2 != -1)
						{
							nPos1 = nPos2 + 9;
							nPos2 = strMessage.Find(_T("MB"));
							strCapacity = strMessage.Mid(nPos1,nPos2 - nPos1);
							strCapacity.Trim();
							ulCapacitySizeMB = (ULONG)_ttol(strCapacity);
						}

						m_PageDevice.ChangeDeviceStatus(nSlotNum,IDB_SD_NORMAL,strSN,(ULONGLONG)ulCapacitySizeMB*1024*1024);
					}

					// 检测拔卡
					// ##Remove card:1
					if (strMessage.Find(_T("##Remove card")) != -1)
					{
						nPos1 = strMessage.Find(_T("##Remove card:"));
						nPos2 = strMessage.Find(_T(":"),nPos1);

						if (nPos2 != -1)
						{
							strSlotNum = strMessage.Mid(nPos2 + 1);

							strSlotNum.Trim();

							nSlotNum = _ttoi(strSlotNum);

							m_PageDevice.ChangeDeviceStatus(nSlotNum,IDB_SD_GRAY,strSN,0);
						}
					}
				}


				// ###SLOG_S::开始记录SLOT ###SLOG_E::结束记录SLOT，立即输出
				if (strMessage.Find(_T("###SLOG_S::")) != -1)
				{
					bSlog = true;
					continue;
				}

				if (strMessage.Find(_T("###SLOG_E::")) != -1)
				{
					bSlog = false;
					bSpeed = true;

					// 输出每个Slot的状态
					::PostMessage(m_pDlg[1]->GetSafeHwnd(),WM_INIT_DEVICE,0,0);
					::PostMessage(m_pDlg[2]->GetSafeHwnd(),WM_INIT_DEVICE,0,0);
					continue;
				}

				// 记录每个Slot的SlotNum、SN、CapacitySize信息
				if (bSlog)
				{
					bool isSRC = false,isTRG = false;

					int nPos1 = 0,nPos2 = 0;

					nPos1 = strMessage.Find(_T("MACHINEID::"));
					if (nPos1 != -1)
					{
						nPos1 += 10;
						nPos2 = strMessage.GetLength();
						m_strMachineID = strMessage.Right(nPos2-nPos1);
						m_strMachineID.Trim();
						GetDlgItemText(IDC_EDIT_PREFIX,m_strAlias);

						m_Machine.SetMachineID(m_strMachineID);
						m_Machine.SetAlias(m_strAlias);

						continue;
					}

					nPos1 = strMessage.Find(_T("FUNCTION::"));
					if (nPos1 != -1)
					{
						nPos1 += 10;
						nPos2 = strMessage.GetLength();
						CString strFunction = strMessage.Right(nPos2-nPos1);
						strFunction.Trim();

						m_Machine.SetFunction(strFunction);

						continue;
					}

					nPos1 = strMessage.Find(_T("SRC("));

					if (nPos1 != -1)
					{
						isSRC = true;
					}
					else
					{
						nPos1 = strMessage.Find(_T("TRG("));
						if (nPos1 != -1)
						{
							isTRG = true;
						}
					}

					if (isSRC || isTRG)
					{
						int nSlotNum = 0;
						CString strSN;
						ULONG ulCapacitySizeMB = 0;

						// 获取SlotNum
						nPos1 += 4; // 跳过SRC( 或者TRG(
						nPos2 = strMessage.Find(_T(')'),nPos1);
						CString strSlotNum = strMessage.Mid(nPos1,nPos2-nPos1);
						strSlotNum.Trim();
						nSlotNum = _ttoi(strSlotNum);

						//获取S/N
						nPos1 = strMessage.Find(_T("S/N:"));
						nPos1 += 4;
						nPos2 = strMessage.Find(_T("Capacity:"),nPos1);
						strSN = strMessage.Mid(nPos1,nPos2-nPos1);
						strSN.Trim();

						//获取Capacity;
						nPos1 = nPos2 + 9;
						nPos2 = strMessage.Find(_T("MB"),nPos1);
						CString strCapacity = strMessage.Mid(nPos1,nPos2-nPos1);
						strCapacity.Trim();
						ulCapacitySizeMB = (ULONG)_ttol(strCapacity);

						m_Machine.AddSlot(nSlotNum,strSN,ulCapacitySizeMB);

						continue;
					}

					continue;
				}

				//  ###SUMMARY_S::结束速度
				if (strMessage.Find(_T("###SUMMARY_S::")) != -1)
				{
					bSpeed = false;
					bSummary = true;

					continue;
				}

				if (bSpeed)
				{
					// #Start Speed: 统计一次平均速度
					if (strMessage.Find(_T("#Start speed:")) != -1)
					{
						continue;
					}

					int nSlotNum = 0;
					double dbSpeed = 0.0;
					int nPercent = 0;
					int nPos1 = 0,nPos2 = 0;

					nPos1 = strMessage.Find(_T("Percent("));
					if (nPos1 != -1)
					{
						nPos1 += 8;
						nPos2 = strMessage.Find(_T("):"),nPos1);
						CString strSlotNum = strMessage.Mid(nPos1,nPos2-nPos1);
						strSlotNum.Trim();
						nSlotNum = _ttoi(strSlotNum);

						nPos1 = nPos2 + 2;
						nPos2 = strMessage.GetLength();
						CString strPercent = strMessage.Right(nPos2-nPos1);
						strPercent.Trim();

						nPercent = _ttoi(strPercent);

						m_Machine.UpdateSlotPercent(nSlotNum,nPercent);

						continue;

					}

					nPos1 = strMessage.Find(_T("Speed("));

					if (nPos1 != -1)
					{
						nPos1 += 6;
						nPos2 = strMessage.Find(_T("):"),nPos1);
						CString strSlotNum = strMessage.Mid(nPos1,nPos2-nPos1);
						strSlotNum.Trim();
						nSlotNum = _ttoi(strSlotNum);

						nPos1 = nPos2 + 2;
						nPos2 = strMessage.GetLength();
						CString strSpeed = strMessage.Right(nPos2-nPos1);
						strSpeed.Trim();

						dbSpeed = _ttof(strSpeed) / 2. / 1024.;

						m_Machine.UpdateSlotSpeed(nSlotNum,dbSpeed);

						continue;
					}

					// 过程中出错
					nPos1 = strMessage.Find(_T("##ERROR:("));
					if (nPos1 != -1)
					{
						nPos1 += 9;
						nPos2 = strMessage.Find(_T(")"),nPos1);
						CString strSlotNum = strMessage.Mid(nPos1,nPos2-nPos1);
						strSlotNum.Trim();
						nSlotNum = _ttoi(strSlotNum);

						m_Machine.UpdateSlotResult(nSlotNum,FALSE);
						continue;
					}

					continue;
				}

				// ###SUMMARY_E:: 统计结束
				if (strMessage.Find(_T("###SUMMARY_E::")) != -1)
				{
					bSummary = false;

					m_Machine.SetEndTime(CTime::GetCurrentTime());
					// 显示最终结果
					PostMessage(WM_UPDATE_RESULT);

					continue;
				}

				if (bSummary)
				{
					int nSlotNum = 0;
					BOOL bResult = FALSE;
					int nPos1 = 0,nPos2 = 0;

					nPos1 = strMessage.Find(_T("TARGET"));
					if (nPos1 != -1)
					{
						nPos1 += 6;
						nPos2 = strMessage.Find(_T("=>("));
						CString strSlotNum = strMessage.Mid(nPos1,nPos2-nPos1);
						strSlotNum.Trim();
						nSlotNum = _ttoi(strSlotNum);

						nPos1 = nPos2 + 3;
						CString strResult = strMessage.Mid(nPos1,4);

						if (strResult.CompareNoCase(_T("PASS")) == 0)
						{
							m_Machine.UpdateSlotResult(0,TRUE);
							bResult = TRUE;
						}
						else
						{
							bResult = FALSE;
						}

						m_Machine.UpdateSlotResult(nSlotNum,bResult);

						continue;
					}

					continue;
				}

			}


		}
	}
	delete []pBuffer;

	::SetEvent(m_hEvent);
}


afx_msg LRESULT CTMonitorDlg::OnUpdateResult(WPARAM wParam, LPARAM lParam)
{
	DB_SLOT dbSlot;
	dbSlot.strFunction = m_Machine.GetFunction();
	dbSlot.StartTime = m_Machine.GetStartTime();
	dbSlot.EndTime = m_Machine.GetEndTime();
	dbSlot.strMachineID = m_Machine.GetMachineID();
	dbSlot.strAlias = m_Machine.GetAlias();

	Slot_List *slotList;
	slotList = m_Machine.GetSlotList();
	POSITION pos = slotList->GetHeadPosition();
	while (pos)
	{
		PSLOT_INFO slotInfo = slotList->GetNext(pos);

		if (slotInfo)
		{
			dbSlot.iSlot = slotInfo->nSlotNum;
			dbSlot.strType = (slotInfo->slotType == SlotType_SRC) ? _T("M") : _T("T");
			dbSlot.strSN = slotInfo->strSN;
			dbSlot.ulSizeMB = slotInfo->ulCapacityMB;
			dbSlot.dbSpeed = slotInfo->dbSpeed;
			dbSlot.iPercent = slotInfo->nPercent;
			dbSlot.strResult = (slotInfo->bResult) ? _T("PASS") : _T("FAIL");

			m_SlotData.AddSlotData(dbSlot);
		}
		
	}

	return 0;
}


afx_msg LRESULT CTMonitorDlg::OnSnRepeate(WPARAM wParam, LPARAM lParam)
{
	int iSlot = (int)wParam;
	SLOT_COLOR color = (SLOT_COLOR)lParam;

	CString strMsg,strText;

	switch (color)
	{
	case SD_YELLOW:
		strText.LoadString(IDS_FORMAT_MSG_SN_REPEAT);
		break;

	case SD_RED:
		strText.LoadString(IDS_FORMAT_MSG_BAD_CARD);
	}
	
	strMsg.Format(strText,iSlot);

	m_pDlg[m_nCurSelTab]->ShowWindow(SW_HIDE);
	m_nCurSelTab = 1;
	m_pDlg[m_nCurSelTab]->ShowWindow(SW_SHOW);
	

	CAlarmDlg alarm;
	alarm.SetSlotIn(&m_SlotIn);
	alarm.SetSlotMessage(iSlot,strMsg);
	alarm.DoModal();

	return 0;
}


void CTMonitorDlg::OnCbnSelchangeComboModule()
{
	// TODO: 在此添加控件通知处理程序代码
	int nSelectIndex = m_comboModel.GetCurSel();

	if (nSelectIndex == -1)
	{
		return;
	}

	DWORD dwData = m_comboModel.GetItemData(nSelectIndex);
	UINT nSlotCount = HIWORD(dwData);
	UINT nSlotPerRow = LOWORD(dwData);

	if (nSlotCount != m_nSlotCount || nSlotPerRow != m_nSlotPerRow)
	{
		m_nSlotCount = nSlotCount;
		m_nSlotPerRow = nSlotPerRow;
		::PostMessage(m_PageDevice.GetSafeHwnd(),WM_CHANGE_SLOT_COUNT,(WPARAM)m_nSlotCount,(LPARAM)m_nSlotPerRow);

		if (nSlotCount >= 48)
		{
			DWORD dwStyle = GetStyle();

			if ((dwStyle & WS_MAXIMIZE) != WS_MAXIMIZE)
			{
				ShowWindow(SW_MAXIMIZE);
			}
		}
	}
	
}


void CTMonitorDlg::OnCbnEditchangeComboBaudrate()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strBaudRate;
	m_comboBaudRate.GetWindowText(strBaudRate);

	strBaudRate.Trim();

	if (strBaudRate.IsEmpty())
	{
		return;
	}

	TCHAR ch = strBaudRate.GetAt(strBaudRate.GetLength()-1);

	if (!_istdigit(ch))
	{
		m_comboBaudRate.SetWindowText(_T(""));
	}
}
