#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "TypeDef.h"


// CPageLog �Ի���

class CPageLog : public CDialogEx
{
	DECLARE_DYNAMIC(CPageLog)

public:
	CPageLog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPageLog();

// �Ի�������
	enum { IDD = IDD_DIALOG_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	CRect         m_rect;
	CEdit         m_editLog;
	CString       m_strAppPath;
	CString       m_strLogPath;
	BOOL          m_bAutoSave;
	HANDLE        m_hEvent;

	void ChangeSize(CWnd *pWnd, int cx, int cy);

public:
	virtual BOOL OnInitDialog();

	void SetLogPath(LPCTSTR lpstrPath);
	void SetAutoSave(BOOL bAutoCheck = TRUE);
	void AppendLogText(LPCTSTR lpstrBuffer,BOOL bAddTime = TRUE);
	void ClearLog();
	void SaveLog();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
