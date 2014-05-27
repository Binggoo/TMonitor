#pragma once


// CAlarmDlg �Ի���
typedef CMap<int,int,BOOL,BOOL> SLOT_IN;

class CAlarmDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAlarmDlg)

public:
	CAlarmDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAlarmDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ALARM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void SetSlotIn(SLOT_IN *pSlotIn);
	void SetSlotMessage(int iSlot,CString strMsg);

private:
	SLOT_IN *m_pSlotIn;
	CString m_strMsg;
	BOOL    m_bAlarm;
	int     m_iSlot;
	CFont   m_font;

	static DWORD WINAPI AlarmThreadProc(LPVOID lpParm);
	static DWORD WINAPI ListenThreadProc(LPVOID lpParm);

	void Alarm();
	void Listen();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL DestroyWindow();
};
