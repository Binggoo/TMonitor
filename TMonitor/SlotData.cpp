#include "StdAfx.h"
#include "SlotData.h"
#include "Utils.h"
#include <shlobj.h>

CppSQLite3DB CMySlotData::s_db;

CMySlotData::CMySlotData(void)
{
}


CMySlotData::~CMySlotData(void)
{
}

BOOL CMySlotData::InitialDatabase(CString strDBPath)
{
	DbgPrintA(("SQLite Header Version: %s",CppSQLite3DB::SQLiteVersion()));

	if (!PathFileExists(strDBPath))
	{
		SHCreateDirectoryEx(NULL,strDBPath,NULL);
	}

	strDBPath += DB_NAME;

	DbgPrint((_T("Customer Database File: %s"),strDBPath));

	try
	{
		s_db.open(strDBPath);
		if (!s_db.tableExists(TS123_TABLE_NAME)) //如果 basic 表不存在
		{
			CString strDML;
			strDML.Format(_T("CREATE TABLE IF NOT EXISTS %s\
						  (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
						  slot INTEGER NOT NULL,\
						  type TEXT NOT NULL,\
						  machineid TEXT,\
						  alias TEXT,\
						  sn TEXT, \
						  size INTEGER,\
						  function TEXT NOT NULL,\
						  starttime DATETIME,\
						  endtime DATETIME,\
						  speed REAL,\
						  percent INTEGER,\
						  result TEXT);"),TS123_TABLE_NAME);
			if (s_db.execDML(strDML) != SQLITE_OK) //执行成功
			{
				DbgPrint((_T("create %s table %s"),TS123_TABLE_NAME,s_db.tableExists(TS123_TABLE_NAME) ? "successfully" : "failed"));
				return FALSE;
			}else
			{
				DbgPrint((_T("create %s table succfully"),TS123_TABLE_NAME));
			}
		}

		if (!s_db.tableExists(TS123_SN_TABLE_NAME)) //如果 basic 表不存在
		{
			CString strDML;
			strDML.Format(_T("CREATE TABLE IF NOT EXISTS %s\
							 (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
							 machineid TEXT,\
							 alias TEXT,\
							 sn TEXT NOT NULL,\
							 gettime DATETIME NOT NULL);"),TS123_SN_TABLE_NAME);
			if (s_db.execDML(strDML) != SQLITE_OK) //执行成功
			{
				DbgPrint((_T("create %s table %s"),TS123_SN_TABLE_NAME,s_db.tableExists(TS123_SN_TABLE_NAME) ? "successfully" : "failed"));
				return FALSE;
			}else
			{
				DbgPrint((_T("create %s table succfully"),TS123_SN_TABLE_NAME));
			}
		}
	}catch (CppSQLite3Exception &e)
	{
		DbgPrint((_T("Create table error,exception %s"),e.errorMessage()));
		return FALSE;
	}

	return TRUE;
}

BOOL CMySlotData::AddSlotData( DB_SLOT dbSlot )
{
	try
	{
		CString strDML;

		if (!s_db.tableExists(TS123_TABLE_NAME)) //如果 basic 表不存在
		{
			strDML.Format(_T("CREATE TABLE IF NOT EXISTS %s\
							 (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
							 slot INTEGER NOT NULL,\
							 type TEXT NOT NULL,\
							 machineid TEXT,\
							 alias TEXT,\
							 sn TEXT, \
							 size INTEGER,\
							 function TEXT NOT NULL,\
							 starttime DATETIME,\
							 endtime DATETIME,\
							 speed REAL,\
							 percent INTEGER,\
							 result TEXT);"),TS123_TABLE_NAME);
			if (s_db.execDML(strDML) != SQLITE_OK) //执行成功
			{
				DbgPrint((_T("create %s table %s"),TS123_TABLE_NAME,s_db.tableExists(TS123_TABLE_NAME) ? "successfully" : "failed"));
				return FALSE;
			}
			else
			{
				DbgPrint((_T("create %s table succfully"),TS123_TABLE_NAME));
			}
		}

		strDML.Format(_T("INSERT INTO %s (slot,type,machineid,alias,sn,size,function,starttime,endtime,speed,percent,result)\
						 VALUES ('%d','%s','%s','%s','%s','%d','%s','%s','%s','%.1f','%d','%s');"),TS123_TABLE_NAME
						 ,dbSlot.iSlot,dbSlot.strType,dbSlot.strMachineID,dbSlot.strAlias,dbSlot.strSN,dbSlot.ulSizeMB
						 ,dbSlot.strFunction,dbSlot.StartTime.Format(_T("%Y-%m-%d %H:%M:%S")),dbSlot.EndTime.Format(_T("%Y-%m-%d %H:%M:%S"))
						 ,dbSlot.dbSpeed,dbSlot.iPercent,dbSlot.strResult);

		if (s_db.execDML(strDML) != SQLITE_OK)
		{
			DbgPrint((_T("AddSlotData failed.")));
			return FALSE;
		}
		
	}
	catch (CppSQLite3Exception* e)
	{
		DbgPrint((_T("AddSlotData error,exception %s."),e->errorMessage()));
		return FALSE;
	}

	return TRUE;
}

BOOL CMySlotData::QueryData( CString strDML,CListCtrl *pList ,int offset/* = 0*/)
{
	if (strDML.IsEmpty() || pList == NULL)
	{
		return FALSE;
	}

	try
	{
		CppSQLite3Query query = s_db.execQuery(strDML);
		int nCols = query.numFields();

		int nRow = 0;
		while (!query.eof())
		{
			for (int col= 0; col < nCols;col++)
			{
				CString strItem = query.getStringField(col);

				if (col == 0)
				{
					pList->InsertItem(nRow+offset,strItem);

					if (offset != 0)
					{
						pList->SetItemText(nRow,col + offset,strItem);
					}
				}
				else
				{
					pList->SetItemText(nRow,col + offset,strItem);
				}
			}
			query.nextRow();
			nRow++;
		}

		query.finalize();
	}
	catch (CppSQLite3Exception *e)
	{
		DbgPrint((_T("QuerySlotData error,exception %s."),e->errorMessage()));
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMySlotData::QueryData( CString strDML,CStringArray *pArray )
{
	if (strDML.IsEmpty() || pArray == NULL)
	{
		return FALSE;
	}

	try
	{
		CppSQLite3Query query = s_db.execQuery(strDML);
		int nCols = query.numFields();

		int nRow = 0;
		while (!query.eof())
		{
			CString strItem = query.getStringField(0);

			pArray->Add(strItem);
			
			query.nextRow();
			nRow++;
		}

		query.finalize();
	}
	catch (CppSQLite3Exception *e)
	{
		DbgPrint((_T("QuerySlotData error,exception %s."),e->errorMessage()));
		return FALSE;
	}

	return TRUE;
}

BOOL CMySlotData::DeleteData( int nID,CString strTableName )
{
	try
	{
		CString strDML;
		strDML.Format(_T("DELETE FROM %s WHERE id='%d';"),strTableName,nID);

		if (s_db.execDML(strDML) != SQLITE_OK)
		{
			DbgPrint((_T("DeleteSlotData failed.")));
			return FALSE;
		}

	}
	catch (CppSQLite3Exception* e)
	{
		DbgPrint((_T("DeleteSlotData error,exception %s."),e->errorMessage()));
		return FALSE;
	}

	return TRUE;
}

BOOL CMySlotData::DeleteData( CString strID,CString strTableName )
{
	try
	{
		CString strDML;
		strDML.Format(_T("DELETE FROM %s WHERE id='%s';"),strTableName,strID);

		if (s_db.execDML(strDML) != SQLITE_OK)
		{
			DbgPrint((_T("DeleteSlotData failed.")));
			return FALSE;
		}

	}
	catch (CppSQLite3Exception* e)
	{
		DbgPrint((_T("DeleteSlotData error,exception %s."),e->errorMessage()));
		return FALSE;
	}

	return TRUE;
}

BOOL CMySlotData::CleanupTable(CString strTableName)
{
	try
	{
		CString strDML;
		strDML.Format(_T("DROP TABLE %s;"),strTableName);

		if (s_db.execDML(strDML) != SQLITE_OK)
		{
			DbgPrint((_T("CleanupSlotData failed.")));
			return FALSE;
		}

	}
	catch (CppSQLite3Exception* e)
	{
		DbgPrint((_T("CleanupSlotData error,exception %s."),e->errorMessage()));
		return FALSE;
	}

	return TRUE;
}

BOOL CMySlotData::AddSN( DB_SN_INFO dbSN )
{
	try
	{
		CString strDML;

		if (!s_db.tableExists(TS123_SN_TABLE_NAME)) //如果 basic 表不存在
		{
			strDML.Format(_T("CREATE TABLE IF NOT EXISTS %s\
							 (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
							 machineid TEXT,\
							 alias TEXT,\
							 sn TEXT NOT NULL,\
							 gettime DATETIME NOT NULL);"),TS123_SN_TABLE_NAME);
			if (s_db.execDML(strDML) != SQLITE_OK) //执行成功
			{
				DbgPrint((_T("create %s table %s"),TS123_SN_TABLE_NAME,s_db.tableExists(TS123_SN_TABLE_NAME) ? "successfully" : "failed"));
				return FALSE;
			}
			else
			{
				DbgPrint((_T("create %s table succfully"),TS123_SN_TABLE_NAME));
			}
		}

		strDML.Format(_T("INSERT INTO %s (machineid,alias,sn,gettime) VALUES ('%s','%s','%s','%s');")
			,TS123_SN_TABLE_NAME,dbSN.strMachineID,dbSN.strAlias,dbSN.strSN,
			dbSN.GetTime.Format(_T("%Y-%m-%d %H:%M:%S")));

		if (s_db.execDML(strDML) != SQLITE_OK)
		{
			DbgPrint((_T("AddSN failed.")));
			return FALSE;
		}

	}
	catch (CppSQLite3Exception* e)
	{
		DbgPrint((_T("AddSN error,exception %s."),e->errorMessage()));
		return FALSE;
	}

	return TRUE;
}

int CMySlotData::GetCount( CString strDML )
{
	int nCount = 0;
	try
	{
		nCount = s_db.execScalar(strDML);
	}
	catch (CppSQLite3Exception* e)
	{
		DbgPrint((_T("CleanupSlotData error,exception %s."),e->errorMessage()));
		nCount = 0;
	}

	return nCount;
}

BOOL CMySlotData::IsTableExist( CString strTableName )
{
	return s_db.tableExists(strTableName);
}
