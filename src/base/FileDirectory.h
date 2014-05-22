#pragma once
#include <string>
#include <vector>
#include "String.h"

using namespace std;

namespace PS{
	namespace FILESTRINGUTILS{

		long GetFileSize(const char* chrFilePath);

		bool FileExists(const AnsiStr& strFilePath);

		AnsiStr GetExePath();

		void GetExePath(char *exePath, int szBuffer);

		AnsiStr ExtractFileTitleOnly(const AnsiStr& strFilePath);

		AnsiStr ExtractFilePath(const AnsiStr& strFilePath);

		AnsiStr ExtractFileName(const AnsiStr& strFilePath);

		/*!
		 * Extracts one level up directory. If the original path contains a slash at the end, the
		 * output path will also have an slash.
		 */
		AnsiStr ExtractOneLevelUp(const AnsiStr& strFilePath);

		/*!
		 * Returns file extension without dot
		 */
		AnsiStr ExtractFileExt(const AnsiStr& strPathFileName);

		AnsiStr CreateNewFileAtRoot(const char* pExtWithDot);

		AnsiStr ChangeFileExt(const AnsiStr& strFilePath, const AnsiStr& strExtWithDot);


        //int ListFilesOrDirectories(std::vector<AnsiStr>& lstOutput, const char* chrPath, const char* chrExt, bool bDirOnly);
		int ListFilesInDir(std::vector<AnsiStr>& lstFiles, const char* pDir, const char* pExtensions, bool storeWithPath);
		//int ListFilesInDir(DVec<AnsiStr>& lstFiles, const char* pDir, const char* pExtensions, bool storeWithPath);


        bool WriteTextFile(const AnsiStr& strFN, const std::vector<AnsiStr>& content);
        bool WriteTextFile(const AnsiStr& strFN, const AnsiStr& strContent );

        bool ReadTextFile(const AnsiStr& strFN, std::vector<AnsiStr>& content);
        bool ReadTextFile(const AnsiStr& strFN, AnsiStr& content);
	}
}
