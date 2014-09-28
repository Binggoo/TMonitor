#ifndef _TYPE_DEF_H
#define _TYPE_DEF_H

typedef enum _ENUM_SLOT_TYPE
{
	SlotType_SRC,
	SlotType_TRG
}SlotType;

typedef enum _ENUM_SLOT_STATE
{
	SlotState_Online,
	SlotState_Offline,
	SlotState_Stop
}SlotState;

typedef struct _STRUCT_SLOT_INFO
{
	int      nSlotNum;
	SlotType slotType;
	SlotState slotState;
	CString   strSN;
	ULONG    ulCapacityMB;
	double   dbSpeed;
	int      nPercent;
	BOOL     bResult;

	_STRUCT_SLOT_INFO()
	{
		nSlotNum = 0;
		slotType = SlotType_SRC;
		slotState = SlotState_Offline;
		strSN = _T("");
		ulCapacityMB = 0;
		dbSpeed = 0.0;
		nPercent = 0;
		bResult = FALSE;
	}

}SLOT_INFO,*PSLOT_INFO;

typedef CList<PSLOT_INFO,PSLOT_INFO> Slot_List;

#endif