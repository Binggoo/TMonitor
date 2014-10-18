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
	m_nSlotCount = 24;
	m_pMachine = NULL;
	m_nSlotPerRow = 4;
	m_nColums = 4;
	m_Devices = NULL;
}

CPageDevice::~CPageDevice()
{
	if (m_Devices != NULL)
	{
		for (UINT i = 0; i < m_nSlotCount;i++)
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


// CPageDevice ��Ϣ�������


BOOL CPageDevice::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	ASSERT(m_pMachine);

	//��ȡ�Ի���ԭʼ��С
	GetClientRect(&m_Rect);

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

	// ����������ɫ
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
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CPageDevice::ChangeSize( CWnd *pWnd,int cx,int cy,DWORD flag)
{
	if(pWnd)  //�ж��Ƿ�Ϊ�գ���Ϊ�Ի��򴴽�ʱ����ô˺���������ʱ�ؼ���δ����   
	{  
		CRect rectCtrl;   //��ȡ�ؼ��仯ǰ�Ĵ�С    
		pWnd->GetWindowRect(&rectCtrl);  
		ScreenToClient(&rectCtrl);//���ؼ���Сת��Ϊ�ڶԻ����е��������� 

		int iLeft = rectCtrl.left;
		int iTop = rectCtrl.top;
		int iWidth = rectCtrl.Width();
		int iHeight = rectCtrl.Height();

		// �ı�X����
		if ((flag & SIZE_MOVE_X) == SIZE_MOVE_X)
		{
			iLeft = iLeft * cx / m_Rect.Width();
		}

		// �ı�Y����
		if ((flag & SIZE_MOVE_Y) == SIZE_MOVE_Y)
		{
			iTop = iTop * cy / m_Rect.Height();
		}

		//�ı���
		if ((flag & SIZE_ELASTIC_X) == SIZE_ELASTIC_X)
		{
			iWidth = iWidth * cx / m_Rect.Width();
		}

		// �ı�߶�
		if ((flag & SIZE_ELASTIC_Y) == SIZE_ELASTIC_Y)
		{
			iHeight = iHeight * cy / m_Rect.Height();
		}

		//�ı���
		if ((flag & SIZE_ELASTIC_X_EX) == SIZE_ELASTIC_X_EX)
		{
			iWidth = cx - iLeft - 10;
		}

		// �ı�߶�
		if ((flag & SIZE_ELASTIC_Y_EX) == SIZE_ELASTIC_Y_EX)
		{
			iHeight = cy - iTop - 10;
		}

		pWnd->MoveWindow(iLeft,iTop,iWidth,iHeight);
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

	CWnd *pWnd = NULL;
	pWnd = GetWindow(GW_CHILD);
	while (pWnd)
	{
		DWORD dwFlag = SIZE_NO_CHANGE;

		switch (pWnd->GetDlgCtrlID())
		{
		case IDC_TEXT_FUNCTION:
		case IDC_TEXT_PERCENT:
		case IDC_TEXT_SPEED:
		case IDC_TEXT_START_TIME:
		case IDC_TEXT_END_TIME:
		case IDC_TEXT_SPEND_TIME:
		case IDC_TEXT_TOTAL:
		case IDC_TEXT_EMPTY:
		case IDC_TEXT_ACTIVE:
		case IDC_TEXT_PASS:
		case IDC_TEXT_FAIL:
			dwFlag = SIZE_MOVE_Y;
			break;

		case IDC_GROUP_STATISTICS:
			dwFlag = SIZE_MOVE_Y | SIZE_ELASTIC_Y;
			break;

		case IDC_DEVICE_FRAME:
			dwFlag = SIZE_ELASTIC_X_EX | SIZE_ELASTIC_Y_EX;
			break;

		}

		ChangeSize(pWnd,cx,cy,dwFlag);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}

	AdjustSingDevicesPos();

	GetClientRect(&m_Rect);

}

void CPageDevice::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

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

			PSLOT_INFO slotInfo = m_pMachine->GetSlotInfo(nSlot);

			if (slotInfo->bResult)
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

			if (nSlot < m_nSlotCount && m_Devices != NULL)
			{
				m_Devices[nSlot].SetDeviceState(nBitmap,(ULONGLONG)slotInfo->ulCapacityMB * 1024 * 1024,slotInfo->dbSpeed,slotInfo->nPercent,slotInfo->strSN);
			}
			
			
		}


	}
	else
	{
		KillTimer(nIDEvent);

		for (int i = 0; i < nSlots;i++)
		{
			int nSlot =  byteArray.GetAt(i);
			

			PSLOT_INFO slotInfo = m_pMachine->GetSlotInfo(nSlot);

			if (slotInfo->bResult)
			{
				nBitmap = IDB_SD_GREEN;
				nPass++;
			}
			else
			{
				nBitmap = IDB_SD_RED;
				nFail++;
			}

			if (nSlot < m_nSlotCount && m_Devices != NULL)
			{
				m_Devices[nSlot].SetDeviceState(nBitmap,(ULONGLONG)slotInfo->ulCapacityMB * 1024 * 1024,slotInfo->dbSpeed,slotInfo->nPercent,slotInfo->strSN);
			}
			
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


HBRUSH CPageDevice::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
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

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}


afx_msg LRESULT CPageDevice::OnChangeSlotCount(WPARAM wParam, LPARAM lParam)
{
	UINT nSlotCount = (UINT)wParam;
	UINT nSlotPerRow = (UINT)lParam;

	if (nSlotCount != m_nSlotCount || nSlotPerRow != m_nSlotPerRow)
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
		m_nSlotPerRow = nSlotPerRow;
	}

	if (m_nSlotCount <= 24)
	{
		m_nColums = 4;
	}
	else
	{
		m_nColums = 8;
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

	int rows = m_nSlotCount / m_nColums;

	if (m_nSlotCount % m_nColums)
	{
		rows++;
	}

	int nWidth = (rectFrame.Width()-2) / m_nColums;
	int nHeight = (rectFrame.Height()-2) / rows;

	// �ֳɼ���
	UINT nBlocks = m_nColums / m_nSlotPerRow;
	if (m_nColums % m_nSlotPerRow)
	{
		nBlocks++;
	}

	// ÿ����ʾ������
	UINT nSlotsPerBlock = m_nSlotCount / nBlocks;
	if (m_nSlotCount % nBlocks)
	{
		nSlotsPerBlock++;
	}

	for (UINT block = 0; block < nBlocks;block++)
	{
		for (UINT slot = 0; slot < nSlotsPerBlock;slot++)
		{
			int nRow = slot / m_nSlotPerRow;
			int nCol = slot % m_nSlotPerRow;

			int port = block * nSlotsPerBlock + slot;
			if (port >= m_nSlotCount)
			{
				break;
			}

			m_Devices[port].SetDevicePort(port);
			m_Devices[port].Create(IDD_DIALOG_SING_DEVICE,this);

			m_Devices[port].MoveWindow(rectFrame.left + 1 + block*m_nSlotPerRow*nWidth + nWidth * nCol,
				rectFrame.top + 1 + nHeight * nRow,
				nWidth,
				nHeight);

			m_Devices[port].ShowWindow(SW_SHOW);
		}
	}
	
}

void CPageDevice::ChangeDeviceStatus( int nSlot,UINT nBitmap ,CString strSN,ULONGLONG ullCapacity)
{
	if (nSlot < m_nSlotCount && m_Devices != NULL)
	{
		m_Devices[nSlot].SetBitmap(nBitmap,strSN,ullCapacity);
	}
	
}

void CPageDevice::Reset()
{
	for (int i = 0; i < m_nSlotCount;i++)
	{
		m_Devices[i].Intial();
	}
}

void CPageDevice::AdjustSingDevicesPos()
{
	if (m_Devices == NULL)
	{
		return;
	}

	if (GetDlgItem(IDC_DEVICE_FRAME) == NULL)
	{
		return;
	}

	CRect rectFrame;
	GetDlgItem(IDC_DEVICE_FRAME)->GetWindowRect(&rectFrame);
	ScreenToClient(&rectFrame);

	int rows = m_nSlotCount / m_nColums;

	if (m_nSlotCount % m_nColums)
	{
		rows++;
	}

	int nWidth = (rectFrame.Width()-2) / m_nColums;
	int nHeight = (rectFrame.Height()-2) / rows;

	// �ֳɼ���
	UINT nBlocks = m_nColums / m_nSlotPerRow;
	if (m_nColums % m_nSlotPerRow)
	{
		nBlocks++;
	}

	// ÿ����ʾ������
	UINT nSlotsPerBlock = m_nSlotCount / nBlocks;
	if (m_nSlotCount % nBlocks)
	{
		nSlotsPerBlock++;
	}

	for (UINT block = 0; block < nBlocks;block++)
	{
		for (UINT slot = 0; slot < nSlotsPerBlock;slot++)
		{
			int nRow = slot / m_nSlotPerRow;
			int nCol = slot % m_nSlotPerRow;

			int port = block * nSlotsPerBlock + slot;
			if (port >= m_nSlotCount)
			{
				break;
			}

			m_Devices[port].MoveWindow(rectFrame.left + 1 + block*m_nSlotPerRow*nWidth + nWidth * nCol,
				rectFrame.top + 1 + nHeight * nRow,
				nWidth,
				nHeight);

			m_Devices[port].ShowWindow(SW_SHOW);
		}
	}
}
