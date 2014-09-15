#pragma once
#include "CppSQLite3U.h"
#pragma comment(lib,"sqlite3.lib")

#define DB_NAME _T("\\phiyo.db")
#define TS123_TABLE_NAME     _T("table_ts123")
#define TS123_SN_TABLE_NAME  _T("table_ts123_sn")

typedef struct _STRUCT_DB_SLOT_INFO
{
	int iID;
	int iSlot;
	CString strType;
	CString strMachineID;
	CString strAlias;
	CString strSN;
	ULONG   ulSizeMB;
	CString strFunction;
	CTime   StartTime;
	CTime   EndTime;
	double   dbSpeed;
	int      iPercent;
	CString  strResult;
}DB_SLOT,*PDB_SLOT;

typedef CList<DB_SLOT,DB_SLOT&> DB_SLOTS;

typedef struct _STRUCT_DB_SN_INFO
{
	int iID;
	CString strMachineID;
	CString strAlias;
	CString strSN;
	CTime GetTime;
}DB_SN_INFO,*PDB_SN_INFO;

typedef CList<DB_SN_INFO,DB_SN_INFO&> DB_SNS;

typedef enum _ENUM_DB_SLOT_COLUMN
{
	Column_ID = 0,
	Column_Slot,
	Column_Type,
	Column_MachineID,
	Column_Prefix,
	Column_SN,
	Column_Size,
	Column_Function,
	Column_StartTime,
	Column_EndTime,
	Column_Speed,
	Column_Percent,
	Column_Result
}DB_SLOT_COLUMN;

class CMySlotData
{
public:
	CMySlotData(void);
	~CMySlotData(void);

	BOOL InitialDatabase(CString strDBPath);

	BOOL AddSlotData(DB_SLOT dbSlot);
	BOOL AddSN(DB_SN_INFO dbSN);

	BOOL QueryData(CString strDML,CListCtrl *pList,int offset = 0);

	BOOL DeleteData(int nID,CString strTableName);

	BOOL DeleteData(CString strID,CString strTableName);

	BOOL CleanupTable(CString strTableName);

	int GetCount(CString strDML);

	BOOL QueryData(CString strDML,CStringArray *pArray);

	BOOL IsTableExist(CString strTableName);

public:
	static CppSQLite3DB	s_db;		//数据库对象

};

