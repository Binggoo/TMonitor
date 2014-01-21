#pragma once

//�����ܵ������͵����ڵ���Ϣ
#define ON_COM_RECEIVE WM_USER + 618  //  WPARAM �˿ں�

class CSerialPort
{
public:
	CSerialPort(bool bAutoBeginThread = true, DWORD dwIOMode = FILE_FLAG_OVERLAPPED);
	virtual ~CSerialPort(void);

protected:
	volatile DWORD m_dwPort;  //���ں�
	volatile HANDLE m_hCommHandle;//���ھ��
	TCHAR m_szCommStr[20];

	DWORD m_dwErrorCode;

	DCB m_DCB;     //�����ʣ�ֹͣλ����
	COMMTIMEOUTS m_CO;     //��ʱ�ṹ

	DWORD m_dwIOMode; // 0 ͬ��  Ĭ�� FILE_FLAG_OVERLAPPED �ص�I/O �첽
	OVERLAPPED m_ReadOverlapped, m_WriteOverlapped; // �ص�I/O

	//�߳���
	volatile HANDLE m_hThreadHandle; //�����߳�
	volatile HWND m_hNotifyWnd; // ֪ͨ����
	volatile DWORD m_dwNotifyNum;//���ܶ����ֽ�(>_dwNotifyNum)����֪ͨ��Ϣ
	volatile bool m_bRunFlag; //�߳�����ѭ����־
	bool m_bAutoBeginThread;//Open() �Զ� BeginThread();
	OVERLAPPED m_WaitOverlapped; //WaitCommEvent use


public:
	//�򿪴��� ȱʡ 9600, 8, n, 1
	bool Open(DWORD dwPort);

	//�򿪴��� ȱʡ baud_rate, 8, n, 1
	bool Open(DWORD dwPort, DWORD dwBaudRate);

	//�򿪴���, ʹ������"9600, 8, n, 1"�������ַ������ô���
	bool Open(DWORD dwPort, TCHAR *szSetStr);

	//�жϴ����ǻ��
	bool IsOpen();

	//��ô��ھ��
	HANDLE GetHandle();

	//���ô��ڲ����������ʣ�ֹͣλ���� ֧�������ַ��� "9600, 8, n, 1"
	bool SetState(TCHAR *szSetStr);

	//���ô��ڲ����������ʣ�ֹͣλ
	bool SetState(DWORD dwBaudRate, BYTE byByteSize = 8, BYTE byParity = NOPARITY, BYTE byStopBits = ONESTOPBIT);

	//���ô��ڵ�I/O��������С
	bool SetBufferSize(DWORD dwInputSize, DWORD dwOutputSize);

	//��ȡ���� dwBufferLength - 1 ���ַ��� szBuffer ����ʵ�ʶ������ַ���
	DWORD Read(char *szBuffer, DWORD dwBufferLength, DWORD dwWaitTime = 20);

	//д���� szBuffer
	DWORD Write(char *szBuffer, DWORD dwBufferLength);

	//д���� szBuffer
	DWORD Write(char *szBuffer);

	//ǿ��ͬ��д
	DWORD WriteSync(char *szBuffer, DWORD dwBufferLength);

	//д���� szBuffer ���������ʽ�ַ���
	DWORD Write(char *szBuffer, DWORD dwBufferLength, char * szFormat, ...);

	//��ȡ���ջ����������ݴ�С
	DWORD GetInBufferLength();

	//�رմ���
	virtual void Close();

	//�趨����֪ͨ, �����ַ���Сֵ
	void SetNotifyNum(int iNum);

	//����Ϣ�Ĵ��ھ��
	void SetWnd(HWND hWnd);

	// ��ȡErrorCode
	DWORD GetErrorCode();

	//�����߳̿��� �������߳�
	bool BeginThread();

	//�߳��Ƿ�����
	bool IsThreadRunning();

	//����߳̾��
	HANDLE GetThread();

	//��ͣ�����߳�
	bool SuspendThread();

	//�ָ������߳�
	bool ResumeThread();

	//��ֹ�߳�
	bool EndThread(DWORD dwWaitTime = 100);

protected:
	//�߳��յ���Ϣ�Զ�����, �細�ھ����Ч, �ͳ���Ϣ, �������ڱ��
	virtual void OnReceive();

	//��ʼ��
	void Init();

	void UnInit();

	//�󶨴���
	void BindCommPort(DWORD dwPort);

	//�򿪴���
	virtual bool OpenCommPort();

	//���ô���
	virtual bool SetupPort();

private:
	//�����߳�
	static DWORD WINAPI CommThreadProc(LPVOID lpPara);
};