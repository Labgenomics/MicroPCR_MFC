#ifndef	___multimedia_timers___
#define	___multimedia_timers___
#pragma comment(lib, "winmm.lib")

#include	<mmsystem.h>

#define WM_MMTIMER WM_USER + 2

class CMMTimers
{
public:
	//핸들
	HWND handle_win;

	//생성자
	CMMTimers(UINT resolution, HWND hwnd);

	//소멸자
	virtual ~CMMTimers();

	//타이머 간격
	UINT	getTimerRes() { return timerRes; };

	//타이머 시작
	bool	startTimer(UINT period,bool oneShot);

	//타이머 정지
	bool	stopTimer();

	//타이머 프로시저
	virtual void timerProc() {};

protected:
	UINT	timerRes;
	UINT	timerId;
};


#endif