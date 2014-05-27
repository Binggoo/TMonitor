//ʹ��˵��:
//�������Ʒ�����VC��EXCEL�Ĳ�������Ҫ��������:
//	A.��EXCEL�ĵ������޸ĺͱ���
//	B.�½�EXCEL�ĵ����в����洢
//	B.��ȡ����дEXCEL�е�����
//	C.����EXCEL�б߿�Ĳ���
//	D.����EXCEL�б�����ɫ�Ĳ���
//	E.��ӡ��Ԥ��
//	F.�Զ������ڴ�
//	G.����ʾ֮��,���ཫ�Զ��رղ��ܽ���������صĲ���
//---------------------------------------------------------�����:ţ��ƽ
#include "excel9.h"
#include <comdef.h>

using namespace excel;
//�»��ߵ�����
#define xlUnderlineStyleDouble -4119
#define xlUnderlineStyleDoubleAccounting 5
#define xlUnderlineStyleNone -4142
#define xlUnderlineStyleSingle 2
#define xlUnderlineStyleSingleAccounting 4
//�߿�λ
#define xlDiagonalDown 5
#define xlDiagonalUp 6
#define xlEdgeBottom 9
#define xlEdgeLeft 7
#define xlEdgeRight 10
#define xlEdgeTop 8
#define xlInsideHorizontal 12
#define xlInsideVertical 11
//�߿�������
#define xlContinuous 1
#define xlDash -4115
#define xlDashDot 4
#define xlDashDotDot 5
#define xlDot -4118
#define xlDouble -4119
#define xlLineStyleNone -4142
#define xlNone -4142
#define xlSlantDashDot 13
//����ˮƽ���뷽ʽ
#define xlGeneral 1
#define xlCenter -4108
#define xlLeft -4131
#define xlRight -4152
#define xlFill 5
#define xlJustify -4130
//���崹ֱ���뷽ʽ
// Download by http://www.codefans.net
#define xlTop -4160
#define xlBottom -4107
//�߿��ߵĴ�ϸ
#define xlHairline 1
#define xlMedium -4138
#define xlThick 4
#define xlThin 2
//����ͼ��������
#define xlSolid 1
#define xlGray8 18
#define xlGray75 -4126
#define xlGray50 -4125
#define xlGray25 -4124
#define xlGray16 17
#define xlHorizontal -4128
#define xlVertical -4166
#define xlDown -4121
#define xlUp -4162
#define xlChecker 9
#define xlSemiGray75 10
#define xlLightHorizontal 11
#define xlLightVertical 12
#define xlLightDown 13
#define xlLightUp 14
#define xlGrid 15
#define xlCrissCross 16
//�߿����ò�����
class MyBorder
{
public:
	//������״
	short LineStyle;
	//����
    short Weight;
	//��ɫ
    long Color;
	//���캯��
	MyBorder();
};
//�������ò�����
class MyBackStyle
{
public:
	//������ɫ
	long Color;
	//����ͼ��
    short Pattern;
	//����ͼ����ɫ
    long PatternColor;
	//͸����͸��
	BOOL transparent;
	//���캯��
	MyBackStyle();
};
//�������ò�����
class MyFont
{
public:
	//����
    CString Name;
	//��С
	long size;
	//ǰ��
	long ForeColor;
	//����
	BOOL Bold;
	//б��
	BOOL Italic;
	//�м���
	BOOL Strikethrough;
	//��Ӱ
	BOOL Shadow;
	//�±�
	BOOL Subscript;
	//�ϱ�
	BOOL Superscricp;
	//�»���
	short Underline;
	//���캯��
	MyFont();
};
//���ֶ��뷽ʽ���ò�����
class MyAlignment
{
public:
	//ˮƽ����
	short HorizontalAlignment;
	//��ֱ����
	short VerticalAlignment;
	//���캯��
	MyAlignment();
};
//���������͵����ò�����
class MyNumberFormat
{
public:
	MyNumberFormat();
	//���ò���
	CString strValue;
	//�����ı����͵�����
	CString GetText();
	//�������ָ�ʽ������
	//blnBox: ʹ�÷ָ��� RightSum: С��λ�� 
	CString GetNumber(BOOL blnBox,int RightSum);
	//���ػ��Ҹ�ʽ������
	//blnChinese: TURE:"��",FALSE:"$"
	//RightSum: С��λ�� 
	CString GetMoney(BOOL blnChinese,int RightSum);
	//�������ڸ�ʽ������
	//blnChinese: TURE:"������",FALSE:"-"
	CString GetDate(BOOL blnChinese);
	//����ʱ���ʽ������
	//blnChinese: TURE:"ʱ����",FALSE:":"
	CString GetTime(BOOL blnChinese);
	//���س�������
	CString GetGeneral();
	//�����������ֵ�����
	//blnChinese: TURE:��д,FALSE:Сд
	CString GetDBNumber(BOOL blnChinese);
	//���ذٷ���������
	//RightSum: С��λ�� 
	CString GetPercentNumBer(int RightSum);
	//���ط���������
	//DownSum:��ĸλ�� DownNum(0): �̶���ĸ��(0)
	CString GetFractionNumBer(int DownSum,int DownNum);
	//���ؿ�ѧ����������
	//RightSum: С��λ�� 
	CString GetTechNumBer(int RightSum);
	//����6λ���������ʽ
	//Sum: �̶����������λ��
	CString GetPost(int Sum);
	//�������ڼ�ʱ�������
	//blnChinese: TURE:"������ʱ����",FALSE:"-:"
	CString GetDateTime(BOOL blnChinese);
};
class CMyExcel
{
public:
	//Excel��Ӧ��
	_Application   MyApp;  
	_Workbook   MyBook;
	_Worksheet   MySheet;   
	Workbooks   MyBooks;   
	Worksheets   MySheets;   
	Range   MyRange;  
	CString strFilePath;

	//���캯��
	CMyExcel();

	//��������
	~CMyExcel();

	//���µ�Excel�ļ�
	BOOL Open();

	//��strFile�ļ�
	BOOL Open(CString strFile);

	//����ΪstrSheet�ı�
	BOOL OpenSheet(CString strSheet);

	//����(ROW,COL)���ַ�strText
	BOOL SetItemText(long Row,long Col,CString strText);

	//ȡ��(ROW,COL)���ַ�
	CString GetItemText(long Row,long Col);

	//�˳�excel
	void Exit();

	//��ʾexcel
	void SetVisible(BOOL blnVisible);

	//���Ҵ��ļ��Ƿ����
	BOOL IsFileExist(CString strFn, BOOL bDir);

	//�Զ�����(��Դ��Ѿ����ڵ��ļ�)
	void Save();

	//����ΪstrPath
	void SaveAs(CString strPath);

	//����µı�
	void AddSheet(CString strSheet);

	//�õ��µ�ѡ������
	void GetRange(CString strBegin,CString strEnd);

	//���Զ�չ��
	void AutoColFit();

	//���Զ�չ��
	void AutoRowFit();

	//�����Զ�����
	void SetWrapText(BOOL blnTrue);

	//��������
	void SetFont(MyFont font);

	//�õ���������
	void AutoRange();

	//�ϲ���Ԫ��
	void SetMergeCells(BOOL blnTrue);

	//���ñ���
	void SetBackStyle(MyBackStyle BackStyle);

	//���ñ߿�
	void SetBorderLine(short Xposition,MyBorder XBorder);

	//���ö��뷽ʽ
	void SetAlignment(MyAlignment XMyAlignment);

	//�õ�����
	long GetRowS();

	//�õ�����
	long GetColS();

	//�������ݵ�����
	void SetNumberFormat(MyNumberFormat XNumberFormat);

	//�����п�
	void SetColumnWidth(int intWidth);

	//�����и�
	void SetRowHeight(int intHeight);

	//��ӡ
	//CopySum:��ӡ�ķ���
	void PrintOut(short CopySum);

	//��ӡԤ��
	//blnEnable:TRUE-�����޸� FALSE-������
	void PrePrintOut(BOOL blnEnable);

	//����ͼƬ
	//strFilePath:�ļ���·��
	void InsertPicture(CString strFilePath);
	//���ñ���ͼƬ

	//strFilePath:�ļ���·��
	void SetBackPicture(CString strFilePath);

	//���ص�ǰ��������·��
	CString GetAppPath();
};