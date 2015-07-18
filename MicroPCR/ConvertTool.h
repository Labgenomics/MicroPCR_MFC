#pragma once

// CConvertTool 명령 대상입니다.

class CConvertTool : public CObject
{
public:
	CConvertTool();
	virtual ~CConvertTool();
};

/***** 이 클래스는 전역 함수를 사용하기 위한 클래스입니다. *****/

void AfxPrintToInt(CString format, void* value);
void AfxPrintToDouble(CString format, void* value);
void AfxPrintToPBYTE(CString footer, BYTE *value, int length = 65);
CString AfxCharToString(char *ch);		// for use getting Serial number
double AfxQuickSort(double d[], int n);	// for filtering temperature