
#include "stdafx.h"
#include "FileManager.h"
#include <locale.h>

namespace FileManager
{
	void loadRecentPath(RECENT_TYPE recentType, CString &returnPath)
	{
		CString recentPath = (recentType == PID_PATH) ? RECENT_PID_PATH : RECENT_PROTOCOL_PATH;

		CStdioFile file;
		if( file.Open(recentPath, CFile::modeRead) )
		{
			// 경로에 한글이 포함될 수 있기 때문에 추가함.
			setlocale(LC_ALL, "korean");
			file.ReadString(returnPath);
			file.Close();
		}
	}

	void saveRecentPath(RECENT_TYPE recentType, CString path)
	{
		CString recentPath = (recentType == PID_PATH) ? RECENT_PID_PATH : RECENT_PROTOCOL_PATH;

		CStdioFile file;
		file.Open(recentPath, CStdioFile::modeCreate|CStdioFile::modeWrite);
		file.WriteString(path);
		file.Close();
	}

	bool findFile(CString path, CString fileName)
	{
		CFileFind finder;
		BOOL hasFile = finder.FindFile( path + "*.*" );

		while( hasFile )
		{
			hasFile = finder.FindNextFileW();

			if( finder.IsDots() )
				continue;

			if( finder.GetFileName().Compare(fileName) == 0 )
				return true;
		}

		return false;
	}

	void enumFiles(CString path, vector<CString> &list)
	{
		CFileFind finder;
		BOOL hasFile = finder.FindFile( path + "*.*" );

		list.clear();

		while( hasFile )
		{
			hasFile = finder.FindNextFileW();

			if( finder.IsDots() )
				continue;

			list.push_back(finder.GetFileName());
		}
	}

	bool loadPID(CString label, vector< PID > &pid)
	{
		CFile file;
		bool res = false;

		pid.clear();

		if( file.Open(L"./PID/" + label, CFile::modeRead) )
		{
			CArchive ar(&file, CArchive::load);
			int size = 0;
			float startTemp = 0, targetTemp = 0, kp = 0, ki = 0, kd = 0;
			
			try{
				ar >> size;
	
				for(int i=0; i<size; ++i){
					ar >> startTemp >> targetTemp >> kp >> ki >> kd;

					pid.push_back( PID(startTemp, targetTemp, kp, ki, kd) );
				}
			}catch(CFileException *e1){
				pid.clear();
				ar.Close();
				file.Close();
			}catch(CArchiveException *e2){
				pid.clear();
				ar.Close();
				file.Close();
			}

			res = true;
		}

		return res;
	}

	bool savePID(CString label, vector< PID > &pid)
	{
		CFile file;
		bool res = false;

		file.Open(L"./PID/" + label, CFile::modeCreate|CFile::modeWrite);
		CArchive ar(&file, CArchive::store);
		
		int size = pid.size();

		try{
			ar << size;
	
			for(int i=0; i<size; ++i){
				ar << pid[i].startTemp << pid[i].targetTemp
					<< pid[i].kp << pid[i].ki << pid[i].kd;
			}

			res = true;
		}catch(CFileException *e1){
			ar.Close();
			file.Close();
		}catch(CArchiveException *e2){
			ar.Close();
			file.Close();
		}

		return res;
	}

	void log(CString msg)
	{
		CTime time = CTime::GetCurrentTime();
		static CString path = time.Format(L"./Log/%Y%m%d-%H%M-%S.txt");

		CStdioFile file;
		CFileFind finder;
		if( finder.FindFile(path) )
			file.Open(path, CStdioFile::modeWrite);
		else
			file.Open(path, CStdioFile::modeCreate|CStdioFile::modeWrite);

		file.SeekToEnd();
		file.WriteString(time.Format(L"[%Y:%m:%d-%H:%M:%S] ") + msg);
		file.Close();
	}
};