// PageLog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TMonitor.h"
#include "PageLog.h"
#include "afxdialogex.h"


// CPageLog �Ի���

IMPLEMENT_DYNAMIC(CPageLog, CDialogEx)

CPageLog::CPageLog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPageLog::IDD, pParent)
{
	m_bAutoSave = FALSE;
}

CPageLog::~CPageLog()
{
	if (m_hEvent != NULL)
	{
		::CloseHandle(m_hEvent);
	}
}

void CPageLog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
}


BEGIN_MESSAGE_MAP(CPageLog, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPageLog ��Ϣ�������


BOOL CPageLog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	// ��ȡ��ǰ·��
	CString strPath;
	GetModuleFileName(NULL,strPath.GetBuffer(MAX_PATH),MAX_PATH);
	strPath.ReleaseBuffer();

	m_strAppPath = strPath.Left(strPath.ReverseFind(_T('\\')));

	// ��ȡ����ԭʼ��С
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

	m_hEvent = ::CreateEvent(NULL,FALSE,TRUE,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CPageLog::AppendLogText( LPCTSTR lpstrBuffer,BOOL bAddTime/* = TRUE*/)
{
	WaitForSingleObject(m_hEvent,INFINITE);
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
	
	::SetEvent(m_hEvent);
}

void CPageLog::ChangeSize( CWnd *pWnd, int cx, int cy )
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


void CPageLog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������

	if (nType == SIZE_MINIMIZED)
	{
		return;
	}
	
	CWnd *pWnd = GetDlgItem(IDC_EDIT_LOG);
	ChangeSize(pWnd,cx,cy);

	GetClientRect(&m_rect);// ���仯��ĶԻ����С��Ϊ�ɴ�С
}

void CPageLog::ClearLog()
{
	m_editLog.SetWindowText(NULL);
}

void CPageLog::SaveLog()
{
	CString strText;
	strText.LoadString(IDS_LOG_FILTER);
	CString strFilter = strText;
	CFileDialog dlg(FALSE,_T("log"),NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilter);
	//dlg.m_ofn.lpstrInitialDir = m_strAppPath;

	if (IDOK == dlg.DoModal())
	{
		CString strPath;
		strPath = dlg.GetPathName();

		BOOL bOK = CopyFile(m_strLogPath,strPath,FALSE);

		if (GetFileAttributes(strPath) & FILE_ATTRIBUTE_HIDDEN)
		{
			SetFileAttributes(strPath,FILE_ATTRIBUTE_NORMAL);
		}
		
		CString strMsg,strTital;
		if (bOK)
		{
			CString strParameters;
			strParameters.Format(_T(" /select,%s"),strPath);

			ShellExecute(NULL,_T("open"),_T("Explorer.exe"),strParameters,NULL,SW_SHOW);
		}
		else
		{
			strMsg.LoadString(IDS_MSG_SAVE_FAILED);
			strTital.LoadString(IDS_MSG_TITAL_ERROR);
			MessageBox(strMsg,strTital,MB_OK | MB_ICONERROR);
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


BOOL CPageLog::PreTranslateMessage(MSG* pMsg)
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
