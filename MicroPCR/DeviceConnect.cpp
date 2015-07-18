// DeviceConnect.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DeviceConnect.h"


// CDeviceConnect

CDeviceConnect::CDeviceConnect()
{
	// None
}

/******************************************************************
*	Function :	CDeviceConnect(HWND hwnd)
*
*	OverView :	생성자로, 기본 생성자말고 이 생성자를 호출해야된다.
*
*	Note	 :	장치가 연결됬을 때, 해당 다이얼로그로 메시지를 날려줘야 하기 때문에
*				그 다이얼로그의 윈도우 핸들값을 매개변수로 받는다.
******************************************************************/
CDeviceConnect::CDeviceConnect(HWND hwnd)
:	m_hDlg(hwnd)
{
}

CDeviceConnect::~CDeviceConnect()
{
}


// CDeviceConnect 멤버 함수

/******************************************************************
*	Function :	int GetDevices(int Vid, int Pid, int Max)
*
*	OverView :	특정 Vid, Pid 값을 가지는 장치들이 몇개나 연결되어 있는지 확인하는 부분이다.
*				장치의 개수를 세어 그 개수를 리턴해준다.
*
*	Note	 :	Vid, Pid 값은 디폴트로 MyPCR에 맞춰져 있다.
******************************************************************/
int CDeviceConnect::GetDevices(int Vid, int Pid, int Max)
{
	mdeviceList *pList = m_DeviceList;

	int nEntries = m_cDevices.GetList(Vid, Pid, NULL, NULL, NULL, pList, Max);

	// Reset the device_list structure pointer
	pList = m_DeviceList;

	return nEntries;
}

/******************************************************************
*	Function :	BOOL OpenDevice(int Vid, int Pid, char* Serial, BOOL IsBlock)
*
*	OverView :	특정 Vid, Pid 값을 가지는 장치에게 연결시도를 한다.
*				연결에 성공 여부에 따라서 BOOL 값을 넘겨준다.
*
*	Note	 :	다른 특정 시리얼을 가지는 기기에 연결하기 위해서는 해당 하는 시리얼
*				을 매개변수로 넘겨줘야한다. 
******************************************************************/
BOOL CDeviceConnect::OpenDevice(int Vid, int Pid, char* Serial, BOOL IsBlock)
{
	int status = m_cDevices.Open(Vid, Pid, NULL, Serial, NULL, IsBlock);

	if( status )
	{
		char *Serial = m_cDevices.m_SerialNumber;
		PostMessage(m_hDlg, WM_SET_SERIAL, 0x00, (LPARAM)Serial);
		return TRUE;
	}

	return FALSE;
}

/******************************************************************
*	Function :	BOOL CheckDevice(void)
*
*	OverView :	장치의 연결상태를 확인하는 콜백함수에서 이 함수를 호출하여
*				연결된 장치가 있으면 그 장치에 연결을 시도하는 행동을 해준다.
*				현재는 1:1 소스이기때문에, 다중 연결을 위해서는 소스를 수정해야한다.
*				
*	Note	 :	None
******************************************************************/
BOOL CDeviceConnect::CheckDevice(void)
{
	if( GetDevices() )
	{
		return OpenDevice();
	}

	return FALSE;
}

/******************************************************************
*	Function :	int Read(BYTE *Buffer)
*
*	OverView :	OpenDevice 가 성공한 장치로부터 Buffer 값에 데이터를 입력받는다.
*				읽은 버퍼의 크기를 리턴해준다.
*				
*	Note	 :	None
******************************************************************/
int CDeviceConnect::Read(void *Buffer)
{
	return m_cDevices.Read(Buffer);
}


/******************************************************************
*	Function :	int Write(BYTE *Buffer)
*
*	OverView :	OpenDevice 가 성공한 장치에게 Buffer를 전송한다.
*				전송에 성공한 버퍼의 크기를 리턴해준다.
*				
*	Note	 :	None
******************************************************************/
int CDeviceConnect::Write(void *Buffer)
{
	return m_cDevices.Write(Buffer);
}
