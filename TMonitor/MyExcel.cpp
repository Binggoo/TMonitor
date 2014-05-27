#include "stdafx.h"
#include "MyExcel.h"

CMyExcel::CMyExcel()
{
	strFilePath = _T("");
}

CString CMyExcel::GetAppPath()
{
	TCHAR lpFileName[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(),lpFileName,MAX_PATH);

	CString strFileName = lpFileName;
	int nIndex = strFileName.ReverseFind (_T('\\'));
	
	CString strPath;

	if (nIndex > 0)
		strPath = strFileName.Left (nIndex);
	else
		strPath = _T("");

	return strPath;
}

CMyExcel::~CMyExcel()
{
	COleVariant covFalse((short)FALSE);
	COleVariant	covOptional((long)DISP_E_PARAMNOTFOUND,VT_ERROR); 
	MyRange.ReleaseDispatch();
	MySheet.ReleaseDispatch();
	MySheets.ReleaseDispatch();
	MyBook.Close(covFalse,_variant_t(strFilePath),covOptional);
	MyBook.ReleaseDispatch();
	MyBooks.Close();
	MyBooks.ReleaseDispatch();
	MyApp.Quit();
	MyApp.ReleaseDispatch();
	CoUninitialize();
}

BOOL CMyExcel::Open()
{
	LPDISPATCH lpDisp=NULL;
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND,VT_ERROR);   
	CoInitialize(NULL);
	if (!MyApp.CreateDispatch(_T("Excel.Application"),NULL))
	{
		AfxMessageBox(_T("EXCEL initial error"),MB_OK|MB_ICONERROR);
		return FALSE;
	}

	lpDisp=MyApp.GetWorkbooks();
	MyBooks.AttachDispatch(lpDisp,TRUE);
	lpDisp = MyBooks.Add(covOptional); 
	MyBook.AttachDispatch(lpDisp,TRUE);
	lpDisp=MyBook.GetWorksheets();
	MySheets.AttachDispatch(lpDisp,TRUE);

	return TRUE;
}

BOOL CMyExcel::Open(CString strFile)
{
	LPDISPATCH   lpDisp = NULL;
	CoInitialize(NULL);
	if (!MyApp.CreateDispatch(_T("Excel.Application"),NULL))
	{
		AfxMessageBox(_T("EXCEL initial error"),MB_OK|MB_ICONERROR);
		return FALSE;
	}

	lpDisp = MyApp.GetWorkbooks();
	MyBooks.AttachDispatch(lpDisp,TRUE);
	lpDisp = MyBooks.Open(strFile, vtMissing, vtMissing, vtMissing, 
		vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing, vtMissing,vtMissing);
	MyBook.AttachDispatch(lpDisp,TRUE);
	lpDisp = MyBook.GetWorksheets(); 
	MySheets.AttachDispatch(lpDisp,TRUE);
	strFilePath = strFile;

	return TRUE;
}

BOOL CMyExcel::OpenSheet(CString strSheet)
{
	LPDISPATCH  lpDisp = NULL;
	long len;
	len = MySheets.GetCount();
	for(long i=1;i<=len;i++)
	{
		lpDisp = MySheets.GetItem((_variant_t)(long)i);
		MySheet.AttachDispatch(lpDisp,TRUE);
		CString str = MySheet.GetName();
		if(MySheet.GetName() == strSheet)
		{
			lpDisp = MySheet.GetCells();
			MyRange.AttachDispatch(lpDisp,TRUE);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMyExcel::SetItemText(long Row,long Col,CString strText)
{
	long lRow = 0,lCol = 0;
	lRow = GetRowS();
	lCol = GetColS();

	if(Row>lRow || Col>lCol)
	{
		CString strText;
		strText.Format(_T("Error, (%d,%d) out of range (%d,%d)!")
			,Row,Col,lRow,lCol);

		AfxMessageBox(strText,MB_OK|MB_ICONERROR);

		return FALSE;
	}
	MyRange.SetItem(_variant_t(Row), _variant_t(Col), _variant_t(strText));
	return TRUE;
}

CString CMyExcel::GetItemText(long Row,long Col)
{
	CString strValue = _T("");
	long lRow = 0,lCol = 0;
	lRow = GetRowS();
	lCol = GetColS();
	if(Row>lRow || Col>lCol)
	{
		CString strText;
		strText.Format(_T("Error, (%d,%d) out of range (%d,%d)!")
			,Row,Col,lRow,lCol);

		AfxMessageBox(strText,MB_OK|MB_ICONERROR);
		return strValue;
	}

	VARIANT lpDisp = MyRange.GetItem(_variant_t(Row), _variant_t(Col));
	Range rgRgeValue;
	rgRgeValue.AttachDispatch(lpDisp.pdispVal, TRUE);
	_variant_t vtVal = rgRgeValue.GetValue();
	if (vtVal.vt == VT_EMPTY)
	{
		rgRgeValue.ReleaseDispatch();
		strValue = _T("");
		rgRgeValue.ReleaseDispatch();
		return strValue;
	}
	vtVal.ChangeType(VT_BSTR);
	strValue= vtVal.bstrVal;
	rgRgeValue.ReleaseDispatch();
	return strValue;
}

void CMyExcel::SaveAs(CString strPath)
{
	if(IsFileExist(strPath,FALSE) == TRUE)
		DeleteFile(strPath);

	MyBook.SaveAs(_variant_t(strPath),vtMissing,vtMissing,vtMissing,vtMissing,vtMissing
		,0,vtMissing,vtMissing,vtMissing,vtMissing);

	strFilePath = strPath;
//	AfxMessageBox(_T("Excel����ɹ�"),MB_OK|MB_ICONINFORMATION);
}

void CMyExcel::Save()
{
	MyBook.Save();
//	AfxMessageBox(_T("Excel����ɹ�"),MB_OK|MB_ICONINFORMATION);
}
 
void CMyExcel::AddSheet(CString strSheet)
{
	LPDISPATCH  lpDisp = NULL;
	MyRange.ReleaseDispatch();
	MySheet.ReleaseDispatch();

	lpDisp = MySheets.Add(vtMissing,vtMissing,vtMissing,vtMissing);
	MySheet.AttachDispatch(lpDisp,TRUE);
	MySheet.SetName(strSheet);
	lpDisp = MySheet.GetCells();
	MyRange.AttachDispatch(lpDisp,TRUE);
}

void CMyExcel::GetRange(CString strBegin,CString strEnd)
{
	MyRange = MySheet.GetRange(_variant_t(strBegin),_variant_t(strEnd));
}

void CMyExcel::AutoColFit()
{
	Range rg = MyRange.GetEntireColumn();
	rg.AutoFit();
	rg.ReleaseDispatch();
}

void CMyExcel::AutoRowFit()
{
	Range rg = MyRange.GetEntireRow();
	rg.AutoFit();	
	rg.ReleaseDispatch();
}

void CMyExcel::SetWrapText(BOOL blnTrue)
{
	MyRange.SetWrapText((_variant_t)(short)blnTrue);
}

void CMyExcel::SetVisible(BOOL blnVisible)
{
	if(blnVisible == TRUE)
		if(strFilePath != _T(""))
		{
			if(IsFileExist(strFilePath,FALSE))
			{
				Exit();
				ShellExecute(NULL,_T("open"),strFilePath,NULL,NULL,SW_SHOW);
			}
			else
			{
				CString strName;
				strName = _T("Path: ") + strFilePath + _T(" error,can't open !");
				AfxMessageBox(strFilePath,MB_OK|MB_ICONINFORMATION);
			}
		}
		else
		{
			AfxMessageBox(_T("Please save first !"),MB_OK|MB_ICONINFORMATION);
		}
}

void CMyExcel::SetFont(MyFont font)
{
	excel::Font f = MyRange.GetFont();
	f.SetName(_variant_t(font.Name));
	f.SetShadow((_variant_t)(short)font.Shadow);
	f.SetSize((_variant_t)(short)font.size);
	f.SetUnderline((_variant_t)(short)font.Underline);
	f.SetBold((_variant_t)(short)font.Bold);
	f.SetColor((_variant_t)(long)font.ForeColor);
	f.SetItalic((_variant_t)(short)font.Italic);
	f.SetStrikethrough((_variant_t)(short)font.Strikethrough);
	f.SetSubscript((_variant_t)(short)font.Subscript);
	f.SetSuperscript((_variant_t)(short)font.Subscript);
	f.ReleaseDispatch();
}

void CMyExcel::SetAlignment(MyAlignment XMyAlignment)
{
	MyRange.SetHorizontalAlignment((_variant_t)(short)XMyAlignment.HorizontalAlignment);
	MyRange.SetVerticalAlignment((_variant_t)(short)XMyAlignment.VerticalAlignment);
}

void CMyExcel::AutoRange()
{
	LPDISPATCH  lpDisp = NULL;
	lpDisp = MySheet.GetCells();
	MyRange.AttachDispatch(lpDisp,TRUE);
}

void CMyExcel::SetMergeCells(BOOL blnTrue)
{
	if(blnTrue == TRUE)
	{
		int i,j;
		long Row=GetRowS();
		long Col=GetColS();

		for(j=2;j<=Col;j++)
		{
			SetItemText(1,j,_T(""));
		}
		for(i=2;i<=Row;i++)
		{
			for(j=1;j<=Col;j++)
			{
				SetItemText(i,j,_T(""));
			}
		}
	}
	MyRange.SetMergeCells((_variant_t)(short)blnTrue);
}

void CMyExcel::SetBackStyle(MyBackStyle BackStyle)
{
	LPDISPATCH  lpDisp = NULL;
	Interior Itor;
	lpDisp = MyRange.GetInterior();
	Itor.AttachDispatch(lpDisp,TRUE);
	if(BackStyle.transparent == TRUE)
		Itor.SetColorIndex((_variant_t)(short)xlNone);
	else
	{
		Itor.SetColor((_variant_t)(long)BackStyle.Color);
		Itor.SetPattern((_variant_t)(short)BackStyle.Pattern);
		Itor.SetPatternColor((_variant_t)(long)BackStyle.PatternColor);
	}
	Itor.ReleaseDispatch();

}

void CMyExcel::SetBorderLine(short Xposition,MyBorder XBorder)
{
	long Row,Col;
	Row = GetRowS();
	Col = GetColS();
	if(Row == 1)
		if(Xposition==xlInsideHorizontal) return;

	if(Col==1)
		if(Xposition==xlInsideVertical) return;

	LPDISPATCH  lpDisp = NULL;
	lpDisp = MyRange.GetBorders();   
	Borders   bds;   
	bds.AttachDispatch(lpDisp);   
	Border   bd;   
	lpDisp = bds.GetItem((long)Xposition);   
	bd.AttachDispatch(lpDisp);   
	bd.SetLineStyle((_variant_t)(short)XBorder.LineStyle);
	bd.SetColor((_variant_t)(long)XBorder.Color);
	bd.SetWeight((_variant_t)(short)XBorder.Weight);
	bd.ReleaseDispatch();
	bds.ReleaseDispatch();
}

long CMyExcel::GetRowS()
{
	long len = 0;
	Range rg = MyRange.GetEntireRow();	
	len = rg.GetCount();
	rg.ReleaseDispatch();
	return len;
}

long CMyExcel::GetColS()
{
	long len = 0;
	Range rg = MyRange.GetEntireColumn();
	len = rg.GetCount();
	rg.ReleaseDispatch();
	return len;
}

void CMyExcel::SetNumberFormat(MyNumberFormat XNumberFormat)
{
	CString strText = XNumberFormat.strValue;
	MyRange.SetNumberFormat(_variant_t(strText));
}

void CMyExcel::SetColumnWidth(int intWidth)
{
	double f = intWidth/8.08;
	MyRange.SetColumnWidth((_variant_t)(double)f);
}

void CMyExcel::SetRowHeight(int intHeight)
{
	double f = intHeight/8.08;
	MyRange.SetRowHeight((_variant_t)(double)f);
}

void CMyExcel::InsertPicture(CString strFilePath)
{
	LPDISPATCH  lpDisp = NULL;
	if(IsFileExist(strFilePath,FALSE))
	{
	//	lpDisp=MySheet.get
	}
}

void CMyExcel::SetBackPicture(CString strFilePath)
{
	if(IsFileExist(strFilePath,FALSE) == TRUE)
		MySheet.SetBackgroundPicture(strFilePath);
}

void CMyExcel::PrintOut(short CopySum)
{
	if(CopySum<=0) CopySum = 1;
	COleVariant covTrue((short)TRUE); 
	COleVariant covFalse((short)FALSE); 
	COleVariant	covOptional((long)DISP_E_PARAMNOTFOUND,VT_ERROR); 
	MySheet.PrintOut(vtMissing,vtMissing,(_variant_t)(short)CopySum,vtMissing
		,vtMissing,vtMissing,covTrue,vtMissing);
}

void CMyExcel::PrePrintOut(BOOL blnEnable)
{
	COleVariant covOptional((short)blnEnable);
	MySheet.PrintPreview(covOptional);
}

BOOL CMyExcel::IsFileExist(CString strFn, BOOL bDir)
{
    HANDLE h;
	LPWIN32_FIND_DATA pFD = new WIN32_FIND_DATA;
	BOOL bFound = FALSE;
	if(pFD)
	{
		h = FindFirstFile(strFn,pFD);
		bFound = (h != INVALID_HANDLE_VALUE);
		if(bFound)
		{
			if(bDir)
				bFound = (pFD->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=NULL;
			FindClose(h);
		}
		delete pFD;
	}
	return bFound;
}

void CMyExcel::Exit()
{
	COleVariant covFalse((short)FALSE);
	COleVariant	covOptional((long)DISP_E_PARAMNOTFOUND,VT_ERROR); 
	MyRange.ReleaseDispatch();
	MySheet.ReleaseDispatch();
	MySheets.ReleaseDispatch();
	MyBook.Close(covFalse,_variant_t(strFilePath),covOptional);
	MyBook.ReleaseDispatch();
	MyBooks.Close();
	MyBooks.ReleaseDispatch();
	MyApp.Quit();
	MyApp.ReleaseDispatch();
	CoUninitialize();
}

MyFont::MyFont()
{
	//����
    Name = _T("Microsoft Sans Serif");

	//��С
	size = 12;

	//ǰ��
	ForeColor = RGB(0,0,0);

	//����
	Bold = FALSE;

	//б��
	Italic = FALSE;

	//�м���
	Strikethrough = FALSE;

	//��Ӱ
	Shadow = FALSE;

	//�±�
	Subscript = FALSE;

	//�ϱ�
	Superscricp = FALSE;

	//�»���
	Underline = xlUnderlineStyleNone;	
}

MyBorder::MyBorder()
{
	//������״
	LineStyle = xlContinuous;

	//����
    Weight=xlThin;

	//��ɫ
    Color = RGB(0,0,0);
}

MyBackStyle::MyBackStyle()
{
	//������ɫ
	Color = RGB(255,255,255);

	//����ͼ��
    Pattern = xlSolid;

	//����ͼ����ɫ
    PatternColor = RGB(255,0,0);

	//Ĭ��Ϊ��͸��
	transparent = FALSE;
}

MyAlignment::MyAlignment()
{
	//��ͨ
	HorizontalAlignment = xlGeneral;

	//���ж���
	VerticalAlignment = xlCenter;
}

MyNumberFormat::MyNumberFormat()
{
	strValue= _T("G/ͨ�ø�ʽ");
}

CString MyNumberFormat::GetText()
{
	strValue=_T("@");
	return strValue;
}

CString MyNumberFormat::GetGeneral()
{
	strValue= _T("G/ͨ�ø�ʽ");
	return strValue;
}

CString MyNumberFormat::GetNumber(BOOL blnBox,int RightSum)
{
	CString str = _T("0");
	int i;
	if(RightSum < 0) RightSum = 0;

	if(blnBox == TRUE)
	{
		if(RightSum == 0)
		{
			str = _T("#,##0_ ");
			strValue = str;
			return strValue;
		}
		else
		{
			str = _T("#,##0.");
			for(i=0;i<RightSum;i++) str = str + _T("0");
			str = str + _T("_ ");
			strValue = str;
			return strValue;	
		}
	}
	else
	{
		if(RightSum == 0)
		{
			str = _T("0_ ");
			strValue = str;
			return strValue;
		}
		else
		{
			str = _T("0.");
			for(i=0;i<RightSum;i++) str = str + _T("0");
			str = str + _T("_ ");
			strValue = str;
			return strValue;	
		}
	}
}

CString MyNumberFormat::GetDate(BOOL blnChinese)
{
	if(blnChinese==TRUE)
		strValue = _T("yyyy\"��\"m\"��\"d\"��\";@");
	else
		strValue = _T("yyyy-m-d;@");

	return strValue;
}

CString MyNumberFormat::GetDateTime(BOOL blnChinese)
{
	if(blnChinese == TRUE)
		strValue = _T("yyyy\"��\"m\"��\"d\"��\" h\"ʱ\"mm\"��\"ss\"��\";@");
	else
		strValue = _T("yyyy-m-d h:mm:ss;@");
	return strValue;
}

CString MyNumberFormat::GetDBNumber(BOOL blnChinese)
{
	if(blnChinese == TRUE)
		strValue = _T("[DBNum1][$-804]G/ͨ�ø�ʽ");
	else
		strValue = _T("[DBNum2][$-804]G/ͨ�ø�ʽ");

	return strValue;
}

CString MyNumberFormat::GetFractionNumBer(int DownSum,int DownNum)
{	
	CString str;
	int i;
	if(DownNum>0 && DownSum>0)
	{
		AfxMessageBox(_T("��ĸλ���͹̶���ֻ������һ��,Ĭ�Ϸ��ط�ĸλ��������!")
			,MB_ICONINFORMATION|MB_OK);
	}

	if(DownSum <= 0)
	{
		if(DownNum <= 0)
		{
			str = _T("# ?/1");
			strValue = str;
			return strValue;
		}
		else
		{
			str.Format(_T("# ?/%d"),DownNum);
			strValue = str;
			return strValue;
		}
	}
	else
	{
		str = _T("# ?/");
		for(i=0;i<DownSum;i++) str = str + _T("?");
		strValue = str;
		return strValue;
	}
}

CString MyNumberFormat::GetMoney(BOOL blnChinese,int RightSum)
{
	CString str;
	int i;
	if(RightSum <= 0) RightSum = 0;

	if(blnChinese == TRUE)
	{
		if(RightSum==0)
		{
			str = _T("#,##0");
			strValue = _T("��") + str + _T(";") + _T("��-") + str;
			return strValue;
		}
		else
		{
			str = _T("#,##0.");
			for(i=0;i<RightSum;i++) str = str + _T("0");

			strValue = _T("��") + str + _T(";") + _T("��-") + str;
			return strValue;
		}

	}
	else
	{
		if(RightSum==0)
		{
			str = _T("#,##0");
			strValue = _T("$") + str + _T(";") + _T("$-") + str;
			return strValue;
		}
		else
		{
			str = _T("#,##0.");
			for(i=0;i<RightSum;i++) str = str + _T("0");
			strValue =  _T("$") + str + _T(";") + _T("$-") + str;
			return strValue;
		}
	}
}

CString MyNumberFormat::GetPercentNumBer(int RightSum)
{
	CString str;
	int i;
	if(RightSum <=0 ) RightSum = 0;
	if(RightSum == 0)
	{
		str = _T("0%");
		strValue = str;
		return strValue;
	}
	else
	{
		str = _T("0.");
		for(i=0;i<RightSum;i++) str = str + _T("0");
		strValue = str + _T("%");
		return strValue;
	}
}

CString MyNumberFormat::GetTechNumBer(int RightSum)
{
	CString str;
	int i;
	if(RightSum<=0) RightSum = 0;
	str = _T("0.");
	for(i=0;i<RightSum;i++) str = str + _T("0");
	strValue = str + _T("E+00");
	return strValue;
}

CString MyNumberFormat::GetTime(BOOL blnChinese)
{
	if(blnChinese==TRUE)
	{
		strValue = _T("h\"ʱ\"mm\"��\"ss\"��\";@");
		return strValue;
	}
	else
	{
		strValue = _T("h:mm:ss;@");
		return strValue;
	}
}

CString MyNumberFormat::GetPost(int Sum)
{
	int i;
	CString str = _T("");
	if(Sum<=0) Sum = 1;
	for(i=0;i<Sum;i++)
		str = str + _T("0");
	strValue = str;
	return strValue;
}