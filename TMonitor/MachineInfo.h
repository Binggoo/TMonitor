#pragma once
#include "TypeDef.h"



class CMachineInfo
{
public:
	CMachineInfo(void);
	~CMachineInfo(void);

	void AddSlot(int nSlotNum,CString strSN,ULONG ulCapacitySizeMB);
	void AddSlot(int nSlotNum,CString strSN,ULONG ulCapacitySizeMB,double dbSpeed,int iPercent,BOOL bResult);
	void UpdateSlotSpeed(int nSlotNum,double dbSpeed);
	void UpdateSlotResult(int nSlotNum,BOOL bResult);
	void UpdateSlotPercent(int nSlotNum,int nPercent);
	void GetSlotList(Slot_List &slotList);
	void SetStartTime(CTime time);
	CTime GetStartTime();
	void SetEndTime(CTime time);
	CTime GetEndTime();
	void SetFunction(CString strFunction);
	CString GetFunction();
	double GetAvgSpeed();
	int GetAvgPercent();

	void SetMachineID(CString strMachineID);
	CString GetMachineID();

	void SetAlias(CString strAlias);
	CString GetAlias();

	int GetSlotCount();
	void GetSlotNum(CByteArray &slotArray);
	SLOT_INFO GetSlotInfo(int nSlotNum);
	void SetRunning(BOOL bRunning);
	BOOL IsRunning();

	int GetSlowestSlot();

	void Reset();
private:
	Slot_List m_SlotList;
	CTime m_StartTime;
	CTime m_EndTime;
	CString m_strFunction;
	CString m_strMachineID;
	CString m_strAlias;
	BOOL m_bRunning;

	CCriticalSection m_cs;
};

