#include "StdAfx.h"
#include "Utils.h"
#pragma comment(lib,"version.lib")


CUtils::CUtils(void)
{
}


CUtils::~CUtils(void)
{
}

char * CUtils::UTF8_TO_ANSI( const char *szUTF8 )
{
	if (szUTF8 == NULL)
	{
		return NULL;
	}

	int nLen = MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, NULL, 0) ;
	WCHAR *pWstr = new WCHAR[nLen+1] ;
	ZeroMemory(pWstr,sizeof(WCHAR) * (nLen+1)) ;
	MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, pWstr, nLen) ;
	char *pBufer = _bstr_t((wchar_t *)pWstr);
	delete[] pWstr;
	return pBufer;
}

char * CUtils::ANSI_TO_UTF8( const char *szAnsi )
{
	if (szAnsi == NULL)
	{
		return NULL;
	}

	_bstr_t bstrTmp(szAnsi);
	int nLen = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)bstrTmp, -1, NULL, 0, NULL, NULL);

	char *pUTF8 = new char[nLen+1];
	ZeroMemory (pUTF8, nLen + 1);
	WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)bstrTmp, -1, pUTF8, nLen, NULL, NULL);

	return pUTF8;
}

void CUtils::ANSI_TO_UTF8( const char *szSrc,char *szDest,int dlen )
{
	if (!szSrc || !szDest)
	{
		return;
	}
	int nSrc = strlen(szSrc);
	DWORD nwLen = MultiByteToWideChar(CP_ACP,0,szSrc,nSrc,NULL,0); //�����ֽ���
	WCHAR *pwsBuf = new WCHAR[nwLen+1];
	ZeroMemory(pwsBuf,sizeof(WCHAR) * (nwLen+1));

	MultiByteToWideChar(CP_ACP,0,szSrc,nSrc,pwsBuf,nwLen); //CHAR TO WCHAR
	WideCharToMultiByte(CP_UTF8,0,pwsBuf,-1,szDest,dlen,NULL,NULL); //wchar to utf8
	delete []pwsBuf;
}

char * CUtils::ANSI_TO_UTF8( const void *pAnsi,unsigned int *len )
{
	if (!pAnsi || !len)
	{
		return NULL;
	}

	DWORD nwLen = MultiByteToWideChar(CP_ACP,0,(CHAR *)pAnsi,*len,NULL,0); //�����ֽ���
	WCHAR *pwsBuf = new WCHAR[nwLen+1];
	if (!pwsBuf)
	{
		return NULL;
	}

	ZeroMemory(pwsBuf,sizeof(WCHAR) * (nwLen+1));
	MultiByteToWideChar(CP_ACP,0,(CHAR *)pAnsi,*len,pwsBuf,nwLen); //CHAR TO WCHAR

	int nLen = WideCharToMultiByte(CP_UTF8,0,pwsBuf,-1,NULL,0,NULL,NULL); //
	char *pUTF8 = new char[nLen+1];
	ZeroMemory(pUTF8,nLen + 1);
	WideCharToMultiByte(CP_UTF8,0,pwsBuf,-1,pUTF8,nLen,NULL,NULL); //WCHAR TO UTF8
	*len = nLen;
	delete []pwsBuf;

	return pUTF8;
}

int CUtils::utf8_to_utf16( const BYTE *s,const USHORT ns,WCHAR *d,const USHORT nd )
{
	if (!s)
	{
		return -1;
	}

	int need = 0;
	int j = 0;
	try
	{
		for (int i = 0;i < ns;i++) //UTF8�ĳ��ȱ�����ʱ��3���ֽ�,��ʱ2���ֽ�,��ʱ1���ֽ�
		{
			if (s[i] >= 0 && s[i] <= 0x7f)
			{
			}else if (s[i] >= 0xC0 && s[i] <= 0xDF)
			{
				i += 1;
			}else if (s[i] >= 0xE0 && s[i] <= 0xEF)
			{
				i += 2;
			}
			need++;
		}

		if (!d) //��������destΪNULL����������Ҫ��С�ֽ�
			return need;

		for (int i = 0;i < ns;i++,j++)
		{
			if (j >= nd) 
				return j;
			if ((s[i] >= 0) && (s[i] <= 0x7f))
			{
				d[j] = s[i];
			}else if ((s[i] >= 0xC0) && (s[i] <= 0xDF))
			{
				//111 11111111
				USHORT u = 0;
				u = (s[i] - 0xC0) << 8;
				u |= (s[i+1] - 0x80);
				i += 1;
				d[j] = u;
			}else if ((s[i] >= 0xE0) && (s[i] <= 0xEF))
			{
				//11111111 11111111
				USHORT u = 0;
				u = (s[i] - 0xE0) << 12;
				u |= (s[i+1] - 0x80) << 6;
				u |= (s[i+2] - 0x80);
				i += 2;
				d[j] = u;
			}
		}
	}catch(CException *e)
	{
		e->ReportError();
	}
	return j;
}

int CUtils::utf16_to_utf8( const WCHAR *s,const USHORT ns,BYTE *d,const USHORT nd )
{
	if (!s)
	{
		return -1;
	}

	int j = 0;
	int need = 0;
	try
	{
		for (int i = 0;i < ns;i++)
		{
			if (s[i] >= 0 && s[i] <= 0x7f)
			{
				need += 1;
			}else if (s[i] >= 0x80 && s[i] <= 0x7FF)	
			{
				need += 2;
			}else if (s[i] >= 0x800 && s[i] <= 0xFFFF)
			{
				need += 3;
			}
		}

		if (!d)
		{
			return need;
		}

		for (int i = 0;i < ns;i++,j++)
		{
			if (j >= nd)
			{
				return j;
			}

			if (s[i] >= 0 && s[i] <= 0x7f)
			{
				d[j] = s[i] & 0x7F; //0x7F 111 1111 ��ȡ��7λ
			}else if (s[i] >= 0x80 && s[i] <= 0x7FF)	
			{
				//111 11111111
				unsigned char u = 0,k = 0;
				u = s[i] << 8;
				u |= s[i+1]; //�ȸ��һ������,��Ϊ 110xxxxx 10xxxxxx
				k = u >> 6; //��ȡ��5λ
				k |= 0xC0;
				d[j] = k;
				k = u & 0x3F; //111 11111111 ��ȡ��6λ
				if (j < nd)
					d[j+1] = k;
				else
					break;
				j += 1;
			}else if (s[i] >= 0x800 && s[i] <= 0xFFFF)
			{
				//11111111 11111111
				unsigned char u = 0;
				u = s[i] >> 12;
				u |= 0xE0;
				d[j] = u;
				u = (s[i] >> 6) & 0x3F;//0x3F 111111
				u |= 0x80;
				if (j < nd - 1)
					d[j+1] = u;
				else
					break;
				u = s[i] & 0x3F;
				u |= 0x80;
				d[j+2] = u;
				j += 2;
			}
		}
	}catch(CException *e)
	{
		e->ReportError();
	}
	return j;
}

CString CUtils::GetAppPath()
{
	CString strPath;
	GetModuleFileName(NULL,strPath.GetBuffer(MAX_PATH),MAX_PATH);
	strPath.ReleaseBuffer();

	return strPath;
}

CString CUtils::GetAppVersion( const CString strPath )
{
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo = NULL;
	CString strVersion ;
	dwVerInfoSize = GetFileVersionInfoSize(strPath, &dwVerHnd);
	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it:
		HANDLE	 hMem;
		LPVOID	 lpvMem;
		unsigned int	uInfoSize = 0;
		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = GlobalLock(hMem);
		GetFileVersionInfo(strPath, dwVerHnd, dwVerInfoSize, lpvMem);
		::VerQueryValue(lpvMem, (LPTSTR)_T("\\"), (void**)&pFileInfo, &uInfoSize);
		WORD wProdVersion[4];
		// Product version from the FILEVERSION of the version info resource
		wProdVersion[0] = HIWORD(pFileInfo->dwProductVersionMS);
		wProdVersion[1] = LOWORD(pFileInfo->dwProductVersionMS);
		wProdVersion[2] = HIWORD(pFileInfo->dwProductVersionLS);
		wProdVersion[3] = LOWORD(pFileInfo->dwProductVersionLS);
		strVersion.Format(_T("%d.%d.%d.%d"),wProdVersion[0],
			wProdVersion[1],wProdVersion[2],wProdVersion[3]);
		GlobalUnlock(hMem);
		GlobalFree(hMem);
	}
	return strVersion;
}

CString CUtils::GetFilePathWithoutName( const CString strFilePath )
{
	return strFilePath.Left(strFilePath.ReverseFind(_T('\\')));
}

CString CUtils::GetFileName( const CString strFilePath )
{
	return strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind(_T('\\')) - 1);
}

CString CUtils::GetFileTitle( const CString strFilePath )
{
	CString strFileName = GetFileName(strFilePath);

	int nPos = strFileName.ReverseFind(_T('.'));
	if (nPos == -1)
	{
		nPos = strFileName.GetLength();
	}

	return strFileName.Left(nPos);
}

CString CUtils::GetErrorMsg( DWORD dwErrorCode )
{
	CString strErrorMsg;
	LPTSTR lpMsgBuffer;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuffer,
		0, NULL );
	strErrorMsg = CString(lpMsgBuffer);
	LocalFree(lpMsgBuffer);
	strErrorMsg.Trim();
	return strErrorMsg;
}

void _DbgPrintA( char *szFormat,... )
{
	char szBuffer[1024] = {NULL};
	va_list va;
	va_start(va, szFormat);
	_vsnprintf(szBuffer, 1024, szFormat, va);
	va_end(va);

	OutputDebugStringA(szBuffer);
}

void _DbgPrintW( wchar_t *szFormat,... )
{
	TCHAR szBuffer[1024] = {NULL};
	va_list va;
	va_start(va, szFormat);
	_vsnwprintf(szBuffer, 1024, szFormat, va);
	va_end(va);

	OutputDebugStringW(szBuffer);
}
