// ConvertTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ConvertTool.h"

#define __YJDEBUG__
//#undef  __YJDEBUG__		/* 버퍼 송수신을 파일로 출력하려면 이 줄을 주석하세요. */

// CConvertTool

/******************************************************************
*	Function :	CConvertTool()
*
*	OverView :	생성자로, AfxPrintToPBYTE 함수를 사용하기 위해서는 생성자를 호출해주어야한다.
*
*	Note	 :	버퍼 값을 출력할 파일이 없을 경우에 파일을 생성해주기 위해서 사용.
******************************************************************/
CConvertTool::CConvertTool()
{
	CStdioFile filer;
	filer.Open(L"test.txt", CFile::modeCreate);
	filer.Close();
}

CConvertTool::~CConvertTool()
{
}


// CConvertTool 전역 함수

/******************************************************************
*	Function :	void AfxPrintToInt(CString format, void* value)
*
*	OverView :	Debug모드로 값을 출력창에 출력하기 위해서 사용하는 함수.
*				Int 형을 출력하기 위한 함수이다.
*
*	Note	 :	Ex) AfxPrintToInt(L"test : %d", &value);
******************************************************************/
void AfxPrintToInt(CString format, void* value)
{
	CString string;
	string.Format(format, *((unsigned char *)value));
	string = string + L"\n";
	OutputDebugString(string);
}

/******************************************************************
*	Function :	void AfxPrintToDouble(CString format, void* value)
*
*	OverView :	Debug모드로 값을 출력창에 출력하기 위해서 사용하는 함수.
*				Double 형을 출력하기 위한 함수이다.
*
*	Note	 :	Ex) AfxPrintToDouble(L"test : %f", &value);
******************************************************************/
void AfxPrintToDouble(CString format, void* value)
{
	CString string;
	string.Format(format, *((double *)value));
	string = string + L"\n";
	OutputDebugString(string);
}

/******************************************************************
*	Function :	void AfxPrintToPBYTE(CString footer, BYTE *value, int length)
*
*	OverView :	BYTE형 버퍼가 어떻게 출력되는지 파일로 확인하고 싶을 때 사용한다.
*				첫번째 인자는 출력하려고 하는 값에 앞에 뭐라고 쓸것인지 사용한다.
*
*	Note	 :	Ex) AfxPrintToPBYTE(L"test : ", readdata, 65);
******************************************************************/
void AfxPrintToPBYTE(CString footer, BYTE *value, int length)
{
#ifdef __YJDEBUG__
	CStdioFile filer;
	
	CString string1, string2;
	string1 = footer;

	filer.Open(L"test.txt", CFile::modeWrite);
	filer.SeekToEnd();

	for(int i=1; i<length; i++)
	{
		string2.Format(L"%d/", value[i]);
		string1 = string1 + string2;
	}
	string1 = string1 + L"\n";
	filer.WriteString(string1);
#endif
}

/******************************************************************
*	Function :	void AfxCharToString(char *ch)
*
*	OverView :	c++ 에서의 문자열을 MFC에서의 문자열로 변경할 때 사용한다.
*				char* -> CString (unicode 형: CStringA)
*
*	Note	 :	None
******************************************************************/
CString AfxCharToString(char *ch)
{
	CString string;
	int	len = MultiByteToWideChar(CP_ACP, 0, ch, strlen(ch), NULL, NULL);
	BSTR buf = SysAllocStringLen(NULL, len);
	MultiByteToWideChar(CP_ACP, 0, ch, strlen(ch), buf, len);
	string.Format(L"%s",buf);
	return string;
}

/******************************************************************
*	Function :	double AfxQuickSort(double d[], int n)
*
*	OverView :	첫번째 인자로 배열을 받아서 값의 크기에 따라 오름차순으로
*				정렬해준다. 그리고 가장 큰 값을 리턴한다.
*
*	Note	 :	None
******************************************************************/
double AfxQuickSort(double d[], int n)
{
	int left, right;
	double pivot;
	double temp;

	if( n > 1 )			// 재귀 호출에 대한 종료가 아닌 조건
	{
		pivot = d[n-1];	// 오른쪽 끝을 피봇으로 설정
		left = -1;		// 왼쪽에서부터 검색할 위치 설정
		right = n-1;	// 오른쪽에서부터 검색할 위치 설정

		while(TRUE)
		{
			while( d[++left] < pivot );		// d[]의 값과 피봇을 비교
			while( d[--right] > pivot );	// d[]의 값과 피봇을 비교

			if( left >= right ) break;		// 왼쪽의 값이 오른쪽의 값보다 크거나 같으면 탈출

			temp = d[left];					// 크지 않을 경우 왼쪽과 오른쪽 값의 교환
			d[left] = d[right];
		}

		temp = d[left];
		d[left] = d[n-1];
		d[n-1] = temp;
		AfxQuickSort(d, left);				// 왼쪽 소구간에 대한 퀵 정렬
		AfxQuickSort(d + left + 1, n - left - 1);	// 오른쪽 소구간에 대한 퀵 정렬
	}
	return d[2];
}
