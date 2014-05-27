// AlarmDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TMonitor.h"
#include "AlarmDlg.h"
#include "afxdialogex.h"


// CAlarmDlg �Ի���

IMPLEMENT_DYNAMIC(CAlarmDlg, CDialogEx)

CAlarmDlg::CAlarmDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAlarmDlg::IDD, pParent)
{
	m_pSlotIn = NULL;
	m_bAlarm = TRUE;
}

CAlarmDlg::~CAlarmDlg()
{
}

void CAlarmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAlarmDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CAlarmDlg ��Ϣ�������


BOOL CAlarmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	ASSERT(m_pSlotIn);

	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_LAYERED);
	
	SetLayeredWindowAttributes(RGB(255,255,255),200,LWA_ALPHA);
		
	m_font.CreatePointFont(200,_T("Arial"));

	GetDlgItem(IDC_TEXT_MSG)->SetFont(&m_font);

	SetDlgItemText(IDC_TEXT_MSG,m_strMsg);

	AfxBeginThread((AFX_THREADPROC)AlarmThreadProc,this);

	AfxBeginThread((AFX_THREADPROC)ListenThreadProc,this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CAlarmDlg::PreTranslateMessage(MSG* pMsg)
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

void CAlarmDlg::SetSlotIn( SLOT_IN *pSlotIn )
{
	m_pSlotIn = pSlotIn;
}

void CAlarmDlg::SetSlotMessage( int iSlot,CString strMsg )
{
	m_strMsg = strMsg;
	m_iSlot = iSlot;
}

DWORD WINAPI CAlarmDlg::AlarmThreadProc( LPVOID lpParm )
{
	CAlarmDlg *pDlg = (CAlarmDlg *)lpParm;

	pDlg->Alarm();

	return 0;
}

DWORD WINAPI CAlarmDlg::ListenThreadProc( LPVOID lpParm )
{
	CAlarmDlg *pDlg = (CAlarmDlg *)lpParm;

	pDlg->Listen();

	return 0;
}

void CAlarmDlg::Alarm()
{
	while (m_bAlarm)
	{
		Beep(2000,500);
	}
}

void CAlarmDlg::Listen()
{
	while (m_bAlarm)
	{
		BOOL bIn;
		if (m_pSlotIn->Lookup(m_iSlot,bIn))
		{
			if (!bIn)
			{
				m_bAlarm = FALSE;
				m_pSlotIn->RemoveKey(m_iSlot);
				
				SetTimer(1,100,NULL);

				break;
			}
		}

		Sleep(50);
	}
}


HBRUSH CAlarmDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if (pWnd->GetDlgCtrlID() == IDC_TEXT_MSG)
	{
		pDC->SetTextColor(RGB(255,0,0));
	}

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}


void CAlarmDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	KillTimer(nIDEvent);
	EndDialog(0);

	CDialogEx::OnTimer(nIDEvent);
}

BOOL CAlarmDlg::DestroyWindow()
{
	// TODO: �ڴ����ר�ô����/����û���
	m_bAlarm = FALSE;
	Sleep(500);
	return CDialogEx::DestroyWindow();
}
