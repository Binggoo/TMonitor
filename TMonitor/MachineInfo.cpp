#include "StdAfx.h"
#include "MachineInfo.h"


CMachineInfo::CMachineInfo(void)
{
	m_bRunning = FALSE;
	m_pSlotList = new Slot_List;
}


CMachineInfo::~CMachineInfo(void)
{
	Reset();
	delete m_pSlotList;
}

void CMachineInfo::AddSlot( int nSlotNum,CString strSN,ULONG ulCapacitySizeMB )
{
	PSLOT_INFO slotInfo = new SLOT_INFO();
	slotInfo->nSlotNum = nSlotNum;
	if (nSlotNum == 0)
	{
		slotInfo->slotType = SlotType_SRC;
	}
	else
	{
		slotInfo->slotType = SlotType_TRG;
	}
	slotInfo->slotState = SlotState_Online;

	slotInfo->strSN = strSN;
	slotInfo->ulCapacityMB = ulCapacitySizeMB;
	slotInfo->dbSpeed = 0.0;
	slotInfo->nPercent = 0;
	slotInfo->bResult = TRUE;

	m_pSlotList->AddTail(slotInfo);
}

void CMachineInfo::AddSlot( int nSlotNum,CString strSN,ULONG ulCapacitySizeMB,double dbSpeed,int iPercent,BOOL bResult )
{
	PSLOT_INFO slotInfo = new SLOT_INFO();
	slotInfo->nSlotNum = nSlotNum;
	if (nSlotNum == 0)
	{
		slotInfo->slotType = SlotType_SRC;
	}
	else
	{
		slotInfo->slotType = SlotType_TRG;
	}
	slotInfo->slotState = SlotState_Online;

	slotInfo->strSN = strSN;
	slotInfo->ulCapacityMB = ulCapacitySizeMB;
	slotInfo->dbSpeed = dbSpeed;
	slotInfo->nPercent = iPercent;
	slotInfo->bResult = bResult;

	m_pSlotList->AddTail(slotInfo);
}

void CMachineInfo::UpdateSlotSpeed( int nSlotNum,double dbSpeed )
{
	m_cs.Lock();
	POSITION pos = m_pSlotList->GetHeadPosition();
	while (pos)
	{
		SLOT_INFO *pSlot = m_pSlotList->GetNext(pos);

		if (pSlot->nSlotNum == nSlotNum)
		{
			pSlot->dbSpeed = dbSpeed;
			break;
		}
	}
	m_cs.Unlock();
}

void CMachineInfo::UpdateSlotResult( int nSlotNum,BOOL bResult )
{
	m_cs.Lock();
	POSITION pos = m_pSlotList->GetHeadPosition();
	while (pos)
	{
		SLOT_INFO *pSlot = m_pSlotList->GetNext(pos);

		if (pSlot->nSlotNum == nSlotNum)
		{
			pSlot->bResult = bResult;
			pSlot->slotState = SlotState_Stop;

			if (bResult)
			{
				pSlot->nPercent = 100;
			}
			break;
		}
	}
	m_cs.Unlock();
}

void CMachineInfo::UpdateSlotPercent( int nSlotNum,int nPercent )
{
	m_cs.Lock();
	POSITION pos = m_pSlotList->GetHeadPosition();
	while (pos)
	{
		SLOT_INFO *pSlot = m_pSlotList->GetNext(pos);

		if (pSlot->nSlotNum == nSlotNum)
		{
			pSlot->nPercent = nPercent;
			break;
		}
	}
	m_cs.Unlock();
}


Slot_List *CMachineInfo::GetSlotList( )
{
	return m_pSlotList;
}

void CMachineInfo::SetStartTime( CTime time )
{
	m_bRunning = TRUE;
	m_StartTime = time;
}

CTime CMachineInfo::GetStartTime()
{
	return m_StartTime;
}

void CMachineInfo::SetEndTime( CTime time )
{
	m_bRunning = FALSE;
	m_EndTime = time;
}

CTime CMachineInfo::GetEndTime()
{
	return m_EndTime;
}

void CMachineInfo::SetFunction( CString strFunction )
{
	m_strFunction = strFunction;
}

CString CMachineInfo::GetFunction()
{
	return m_strFunction;
}

int CMachineInfo::GetSlotCount()
{
	return m_pSlotList->GetCount();
}

void CMachineInfo::GetSlotNum( CByteArray &slotArray )
{
	slotArray.RemoveAll();

	POSITION pos = m_pSlotList->GetHeadPosition();
	while (pos)
	{
		PSLOT_INFO slotInfo = m_pSlotList->GetNext(pos);

		if (slotInfo)
		{
			slotArray.Add((BYTE)slotInfo->nSlotNum);
		}	
	}
}

PSLOT_INFO CMachineInfo::GetSlotInfo( int nSlotNum )
{
	POSITION pos = m_pSlotList->GetHeadPosition();

	PSLOT_INFO slotInfo = NULL;
	while (pos)
	{
		slotInfo = m_pSlotList->GetNext(pos);
		
		if (slotInfo)
		{
			if (slotInfo->nSlotNum == nSlotNum)
			{
				return slotInfo;
			}
		}
		
	}

	return NULL;
}

void CMachineInfo::Reset()
{
	POSITION pos = m_pSlotList->GetHeadPosition();

	while (pos)
	{
		PSLOT_INFO slotInfo = m_pSlotList->GetNext(pos);

		if (slotInfo != NULL)
		{
			delete slotInfo;
			slotInfo = NULL;
		}
	}

	m_pSlotList->RemoveAll();
	m_strFunction = _T("");
	m_StartTime = 0;
	m_EndTime = 0;
	m_bRunning = FALSE;
	m_strMachineID = _T("");
	m_strAlias = _T("");
}

void CMachineInfo::SetRunning( BOOL bRunning )
{
	m_bRunning = bRunning;
}

BOOL CMachineInfo::IsRunning()
{
	return m_bRunning;
}

double CMachineInfo::GetAvgSpeed()
{
	m_cs.Lock();
	POSITION pos = m_pSlotList->GetHeadPosition();
	double dbSpeed = 0.0;
	int nPass = 0;
	while (pos)
	{
		PSLOT_INFO slotInfo = m_pSlotList->GetNext(pos);

		if (slotInfo->nSlotNum == 0)
		{
			continue;
		}

		if (slotInfo->bResult)
		{
			dbSpeed += slotInfo->dbSpeed;

			nPass++;
		}
		
	}
	m_cs.Unlock();

	if (nPass == 0)
	{
		return 0;
	}
	else
	{
		return dbSpeed/nPass;
	}
}

int CMachineInfo::GetAvgPercent()
{
	m_cs.Lock();
	POSITION pos = m_pSlotList->GetHeadPosition();
	int nPercent  = 0;
	int nPass = 0;
	while (pos)
	{
		PSLOT_INFO slotInfo = m_pSlotList->GetNext(pos);

		if (slotInfo->nSlotNum == 0)
		{
			continue;
		}

		if (slotInfo->bResult)
		{
			nPercent += slotInfo->nPercent;
			nPass++;
		}
		
	}
	m_cs.Unlock();

	if (nPass == 0)
	{
		return 0;
	}
	else
	{
		return nPercent/nPass;
	}
}

int CMachineInfo::GetSlowestSlot()
{
	if (m_pSlotList->GetCount() == 0)
	{
		return -1;
	}

	PSLOT_INFO pHead = m_pSlotList->GetHead();

	int nSlowestSlot = pHead->nSlotNum;
	double dbSpeed = m_pSlotList->GetHead()->dbSpeed;
	POSITION pos = m_pSlotList->GetHeadPosition();
	while (pos)
	{
		PSLOT_INFO slotInfo = m_pSlotList->GetNext(pos);

		if (slotInfo->dbSpeed < dbSpeed)
		{
			nSlowestSlot = slotInfo->nSlotNum;
		}
	}

	return nSlowestSlot;
}

void CMachineInfo::SetMachineID( CString strMachineID )
{
	m_strMachineID = strMachineID;
}

CString CMachineInfo::GetMachineID()
{
	return m_strMachineID;
}

void CMachineInfo::SetAlias( CString strAlias)
{
	m_strAlias = strAlias;
}

CString CMachineInfo::GetAlias()
{
	return m_strAlias;
}
