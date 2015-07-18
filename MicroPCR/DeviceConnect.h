#pragma once

// CDeviceConnect 명령 대상입니다.

#include "./Lib/UsbHidApi.h"
#include "UserDefs.h"

class CDeviceConnect
{
public:
	CDeviceConnect();			// Not Use this
	CDeviceConnect(HWND hwnd);	// Use this
	virtual ~CDeviceConnect();

private:
	// Class For Usb Handle
	CUsbHidApi m_cDevices;
	// For Save Device List
	mdeviceList m_DeviceList[DEVICE_MAX];
	// HWND Handle for control Dlg
	HWND m_hDlg;

	// For Related Connection functions ( private )
private:
	// Open Device specific vid, pid by serial
	BOOL OpenDevice(int Vid = LS4550EK_VID, int Pid = LS4550EK_PID, char* Serial = NULL, BOOL IsBlock = TRUE);

	// For Related Connection functions ( public )
public:
	// Check Devices & return Device Counts
	int GetDevices(int Vid = LS4550EK_VID, int Pid = LS4550EK_PID, int Max = DEVICE_MAX);
	// If ConnectDevice Events actived, Called this function
	BOOL CheckDevice(void);
	// Read buffer from Device & return read count
	int Read(void *Buffer);
	// Write buffer to Device & return write count
	int Write(void *Buffer);
};


