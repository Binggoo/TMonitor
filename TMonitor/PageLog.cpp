// PageLog.cpp : 实现文件
//

#include "stdafx.h"
#include "TMonitor.h"
#include "PageLog.h"
#include "afxdialogex.h"


// CPageLog 对话框

IMPLEMENT_DYNAMIC(CPageLog, CDialogEx)

CPageLog::CPageLog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPageLog::IDD, pParent)
{
	m_bAutoSave = FALSE;
}

CPageLog::~CPageLog()
{
}

void CPageLog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
}


BEGIN_MESSAGE_MAP(CPageLog, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPageLog 消息处理程序


BOOL CPageLog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// 获取当前路径
	CString strPath;
	GetModuleFileName(NULL,strPath.GetBuffer(MAX_PATH),MAX_PATH);
	strPath.ReleaseBuffer();

	m_strAppPath = strPath.Left(strPath.ReverseFind(_T('\\')));

	// 获取窗口原始大小
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

	AppendLogText(_T("Welcome !!!"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPageLog::AppendLogText( LPCTSTR lpstrBuffer,BOOL bAddTime/* = TRUE*/)
{
	CString strBuffer;

	if (bAddTime)
	{
		CTime time = CTime::GetCurrentTime();

		strBuffer = time.Format(_T("%Y-%m-%d %H:%M:%S --- "));
		strBuffer += CString(lpstrBuffer);
	}
	else
	{
		strBuffer = CString(_T(' '),24);
		strBuffer += CString(lpstrBuffer);
	}
	
	strBuffer += _T("\r\n");

	CString strText;
	m_editLog.GetWindowText(strText);
	strText += strBuffer;
	m_editLog.SetWindowText(strText);
	
	int nLineCount = m_editLog.GetLineCount();
	m_editLog.LineScroll(nLineCount,0);

	DWORD dwFlagsAndAttributes;
	
	if (m_bAutoSave)
	{
		dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	}
	else
	{
		dwFlagsAndAttributes = FILE_ATTRIBUTE_HIDDEN;
	}

	HANDLE hFile = ::CreateFile(m_strLogPath,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		dwFlagsAndAttributes,NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	SetFilePointer(hFile,0,0,FILE_END);

	DWORD dwSize = 0;
	::WriteFile(hFile,strBuffer,strBuffer.GetLength()*sizeof(TCHAR),&dwSize,NULL);
	::CloseHandle(hFile);
	
}

void CPageLog::ChangeSize( CWnd *pWnd, int cx, int cy )
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


void CPageLog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	if (nType == SIZE_MINIMIZED)
	{
		return;
	}
	
	CWnd *pWnd = GetDlgItem(IDC_EDIT_LOG);
	ChangeSize(pWnd,cx,cy);

	GetClientRect(&m_rect);// 将变化后的对话框大小设为旧大小
}

void CPageLog::ClearLog()
{
	m_editLog.SetWindowText(NULL);
}

void CPageLog::SaveLog()
{
	CString strFilter = _T("Log File(*.log)|*.log | All Files(*.*)|*.*||");
	CFileDialog dlg(FALSE,_T("log"),NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilter);
	dlg.m_ofn.lpstrInitialDir = m_strAppPath;

	if (IDOK == dlg.DoModal())
	{
		CString strPath;
		strPath = dlg.GetPathName();

		BOOL bOK = CopyFile(m_strLogPath,strPath,FALSE);

		if (GetFileAttributes(strPath) & FILE_ATTRIBUTE_HIDDEN)
		{
			SetFileAttributes(strPath,FILE_ATTRIBUTE_NORMAL);
		}
		
		if (bOK)
		{
			MessageBox(_T("Save log success"),_T("Success"));
		}
		else
		{
			MessageBox(_T("Save Log failed"),_T("Error"),MB_OK | MB_ICONERROR);
		}
	}
}

void CPageLog::SetLogPath( LPCTSTR lpstrPath )
{
	m_strLogPath = CString(lpstrPath);
}

void CPageLog::SetAutoSave( BOOL bAutoCheck /*= TRUE*/ )
{
	m_bAutoSave = bAutoCheck;
}
