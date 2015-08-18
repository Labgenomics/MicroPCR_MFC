#pragma once
#include "UserDefs.h"

#define RECENT_PID_PATH			L"Recent_PID.txt"
#define RECENT_PROTOCOL_PATH	L"Recent_Protocol.txt"


namespace FileManager
{
	enum RECENT_TYPE {
		PID_PATH,
		PROTOCOL_PATH
	};

	void loadRecentPath(RECENT_TYPE recentType, CString &returnPath);
	void saveRecentPath(RECENT_TYPE recentType, CString path);
	bool findFile(CString path, CString fileName);
	void enumFiles(CString path, vector<CString> &list);

	bool loadPID(CString label, vector< PID > &pid);
	bool savePID(CString label, vector< PID > &pid);

	void log(CString msg);
};