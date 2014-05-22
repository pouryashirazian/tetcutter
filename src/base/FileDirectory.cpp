//#include "stdafx.h"
#include "FileDirectory.h"
#include "MathBase.h"
#include <fstream>

#ifdef PS_OS_MAC
    #include <mach-o/dyld.h>
    #include <unistd.h>		/* execve */
    #include <libgen.h>		/* dirname */
    #include <dirent.h>
#elif defined(PS_OS_WINDOWS)
	#include <io.h>
	#include "Windows.h"
#elif defined(PS_OS_LINUX)
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>
	#include <unistd.h>
#endif


namespace PS
{

namespace FILESTRINGUTILS
{

long GetFileSize(const AnsiStr& strFilePath)
{
	ifstream ifs(strFilePath.ptr(), ios::in | ios::binary );
	if(!ifs.is_open())
		return -1;
		
	long begin, end;
	begin = ifs.tellg();
	ifs.seekg(0, ios::end);
	end = ifs.tellg();	
	ifs.close();

	return end - begin;
}


//==================================================================
bool FileExists(const AnsiStr& strFilePath)
{
	ifstream ifs(strFilePath.ptr(), ios::in | ios::binary );
	if(ifs.is_open())
	{
		ifs.close();
		return true;
	}
	else
	{
		return false;
	}	
}
//==================================================================
AnsiStr GetExePath()
{
	char buff[1024];
	GetExePath(buff, 1024);
	AnsiStr strOut = AnsiStr(buff);
	return strOut;	
}
//==================================================================
void GetExePath(char *exePath, int szBuffer)
{
#ifdef PS_OS_MAC
    uint32_t szExePath = szBuffer;
    if(_NSGetExecutablePath(exePath, &szExePath) == 0)
        exePath[szBuffer] = 0;
    else {
        exePath[0] = 0;
        printf("Not enough memory to get exepath. Needs %d\n", szBuffer);
    }
/*

    AnsiStr strInput(exePath, )
    int index = result.lastIndexOf(QString(".app"));
    if(index < 0)
        index = result.length();
    else
        result = result.left(index);
    index = result.lastIndexOf(QString("/"));
    if(index < 0)
    {
        index = result.lastIndexOf(QString("\\"));
        if(index < 0)
            index = result.length();
    }

    result = result.left(index + 1);
    */
#elif defined(PS_OS_WINDOWS)
	WCHAR wszExeName[MAX_PATH + 1];
	wszExeName[MAX_PATH] = 0;
	GetModuleFileNameW(NULL, wszExeName, sizeof(wszExeName) - 1);
	WideCharToMultiByte(CP_ACP, 0, wszExeName, -1, (LPSTR)exePath, szBuffer, NULL, NULL);
#elif defined(PS_OS_LINUX)
	//getcwd only retrieves the current working directory
    //getcwd(exePath, (int)szBuffer);

	pid_t pid = getpid();
	AnsiStr strProcessPath = printToAStr("/proc/%d/exe", pid);

	int nCharsWritten = readlink(strProcessPath.ptr(), exePath, szBuffer);
	if(nCharsWritten != -1)
	{
		exePath[nCharsWritten] = 0;

	}
#endif
}
//==================================================================
AnsiStr ExtractFileTitleOnly(const AnsiStr& strFilePath)
{
    int pos = 0;
	AnsiStr strTemp = ExtractFileName(strFilePath);
	if(strTemp.lfind('.', pos))
		strTemp = strTemp.substr(0, pos);
	return strTemp;
}
//==================================================================
AnsiStr ExtractFileName(const AnsiStr& strPathFileName)
{
    int npos;

	AnsiStr strOutput = strPathFileName;
	if(strPathFileName.rfind(L'/', npos) || strPathFileName.rfind(L'\\', npos))
	{
		strOutput = strPathFileName.substr(npos+1);
	}

	return strOutput;
}
//==================================================================
AnsiStr ExtractOneLevelUp(const AnsiStr& strFilePath)
{
	AnsiStr strOutput = strFilePath;
	bool bHasLastSlash = false;
	char lastSlash;

	//Remove last forward or backward slash
	while((strOutput.lastChar() == '/') || (strOutput.lastChar() == '\\'))
	{
		lastSlash = strOutput.lastChar();
		strOutput = strOutput.substr(0, strOutput.length() - 1);		
		bHasLastSlash = true;
	}

    int pos = 0;
	//Up one level
	if(strOutput.rfind('/', pos))
		strOutput = strOutput.substr(0, pos);
	else if(strOutput.rfind('\\', pos))
		strOutput = strOutput.substr(0, pos);
	
	if(bHasLastSlash)
		strOutput += lastSlash;

	return strOutput;
}
//==================================================================
AnsiStr ChangeFileExt(const AnsiStr& strFilePath, const AnsiStr& strExtWithDot)
{
	AnsiStr strOut;
    int npos;
	if(strFilePath.rfind('.', npos))
	{
		strOut = strFilePath.substr(0, npos);
		strOut += strExtWithDot;
	}
	else
		strOut = strFilePath + strExtWithDot;

	return strOut;
}
//==================================================================
AnsiStr CreateNewFileAtRoot(const char* pExtWithDot)
{
	char buffer[1024];
	GetExePath(buffer, 1024);

	AnsiStr strOutput(buffer);
    int posDot;
	if(strOutput.rfind(L'.', posDot))
	{
		AnsiStr temp = strOutput.substr(0, posDot);
		temp.appendFromT(pExtWithDot);
		return temp;
	}
	else
		return strOutput;	
}

//==================================================================
AnsiStr ExtractFilePath(const AnsiStr& fileName)
{
    int npos;
	AnsiStr strOutput;
	if(fileName.rfind(L'/', npos) || fileName.rfind(L'\\', npos))	
	{
		strOutput = fileName.substr(0, npos+1);
	}
	return strOutput;	
}
//==================================================================
AnsiStr ExtractFileExt(const AnsiStr& strPathFileName)
{
    int npos;
	AnsiStr strOut;
	if(strPathFileName.rfind('.', npos))
	{
		strOut = strPathFileName.substr(npos+1);
	}
	return strOut;	
}
//==================================================================
/*
int ListFilesOrDirectories(std::vector<AnsiStr>& lstOutput, const char* chrPath, const char* chrExt, bool bDirOnly)
{

#ifdef WIN32
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWideStr wstrDir = toWideString(chrPath);

	hFind = FindFirstFile(wstrDir.ptr(), &ffd);
	if(hFind == INVALID_HANDLE_VALUE)
		return 0;

	AnsiStr temp;
	do
	{
		if(bDirOnly &&(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
			if (storeWithPath)
			{
				temp = strResolvedDir + "\\";
				temp += AnsiStr(ffd.cFileName);
				lstOutput.push_back(temp);
			}
			else
			{
				temp = AnsiStr(ffd.cFileName);
				lstOutput.push_back(temp);
			}
        }
		else
		{
			temp = strResolvedDir + "\\";
			temp += AnsiStr(ffd.cFileName);

			if(chrExt)
			{
				AnsiStr strExt = PS::FILESTRINGUTILS::ExtractFileExt(AnsiStr(temp.ptr()));
				if(strExt == AnsiStr(chrExt))
					lstOutput.push_back(temp);
			}
			else
				lstOutput.push_back(temp);
		}
	}while (FindNextFile(hFind, &ffd) != 0);
   FindClose(hFind);

#else
	DIR *dp;
	struct dirent *pEntry;
	struct stat st;

	if((dp  = opendir(chrPath)) == NULL)
	{
			return 0;
	}

	while ((pEntry = readdir(dp)) != NULL)
	{
		AnsiStr strFileName = AnsiStr(pEntry->d_name);
		AnsiStr strFullFileName = AnsiStr(chrPath) + AnsiStr("/") + strFileName;

		if(strFileName[0] == '.')
			continue;

		if(stat(strFullFileName.c_str(), &st) == -1)
			continue;

		const bool isDir = (st.st_mode & S_IFDIR) != 0;

		if(bDirOnly)
		{
			if(isDir)
				lstOutput.push_back(strFullFileName);
		}
		else
		{
			if(chrExt)
			{
				AnsiStr strExt = PS::FILESTRINGUTILS::ExtractFileExt(strFullFileName);
				if(strExt == AnsiStr(chrExt))
					lstOutput.push_back(strFullFileName);
			}
			else
				lstOutput.push_back(strFullFileName);
		}
	}
	closedir(dp);
#endif

	return lstOutput.size();
}
*/
//==================================================================
int ListFilesInDir(std::vector<AnsiStr>& lstFiles, const char* pDir, const char* pExtensions, bool storeWithPath)
{
	AnsiStr strDir;
	AnsiStr strResolvedDir;

	lstFiles.resize(0);
	if(pDir == NULL)	
		strResolvedDir = ExtractFilePath(GetExePath());
	else
		strResolvedDir = AnsiStr(pDir);
		

	if(pExtensions != NULL)
		strDir = printToAStr("%s/*.%s", strResolvedDir.ptr(), pExtensions);
	else
		strDir = printToAStr("%s/*.*", strResolvedDir.ptr()); 
		

#ifdef PS_OS_WINDOWS
		WIN32_FIND_DATA ffd;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWideStr wstrDir = toWideString(strDir);

		hFind = FindFirstFile(wstrDir.ptr(), &ffd);
		if(hFind == INVALID_HANDLE_VALUE)
			return 0;

		AnsiStr temp;
		do
		{
		    if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	        {
				if (storeWithPath)
				{
					temp = strResolvedDir + "\\";
					temp += AnsiStr(ffd.cFileName);
					lstFiles.push_back(temp);
				}
				else
				{
					temp = AnsiStr(ffd.cFileName);
					lstFiles.push_back(temp);
				}
	        }
    	}while (FindNextFile(hFind, &ffd) != 0);
	   FindClose(hFind);

#else
    DIR* dirFile = opendir( strResolvedDir.cptr() );
    if ( dirFile )
    {
        struct dirent* hFile;
        string strExt = "." + string(pExtensions);
        while (( hFile = readdir( dirFile )) != NULL )
        {
            if ( !strcmp( hFile->d_name, "."  )) continue;
            if ( !strcmp( hFile->d_name, ".." )) continue;
            
            // in linux hidden files all start with '.'
            if (hFile->d_name[0] == '.' ) continue;
            
            // dirFile.name is the name of the file. Do whatever string comparison
            // you want here. Something like:
            if ( strstr( hFile->d_name,  strExt.c_str())) {
                if(storeWithPath)
                    lstFiles.push_back(strResolvedDir + "//" + AnsiStr(hFile->d_name));
                else
                    lstFiles.push_back(AnsiStr(hFile->d_name));
                printf("Found file %s", hFile->d_name);
            }
        } 
        closedir( dirFile );
    }
#endif

	return (int)lstFiles.size();
}

//Write a text file
bool WriteTextFile(const AnsiStr& strFN, const std::vector<AnsiStr>& content )
{
	ofstream ofs(strFN.ptr(), ios::out | ios::trunc);
	if(!ofs.is_open())
		return false;

	AnsiStr strLine;
    for(int i=0; i < content.size(); i++)
	{
		strLine = content[i];
		if(strLine.length() > 0)
		{
			ofs << strLine << '\0' << '\n';
		}
	}
	ofs.close();

	return true;
}

bool WriteTextFile(const AnsiStr& strFN, const AnsiStr& strContent )
{
	ofstream ofs(strFN.ptr(), ios::out | ios::trunc);
	if(!ofs.is_open())
		return false;

	ofs << strContent << '\0' << '\n';
	ofs.close();

	return true;
}

bool ReadTextFile(const AnsiStr& strFN, std::vector<AnsiStr>& content )
{
	ifstream ifs(strFN.ptr(), ios::in);
	if(!ifs.is_open())
		return false;

	AnsiStr strLine;
	char buffer[2048];

	while( !ifs.eof())
	{
		ifs.getline(buffer, 2048);
		//ifs >> strLine;
		strLine.copyFromT(buffer);
		strLine.trim();
        strLine.removeStartEndSpaces();
		content.push_back(strLine);
	}	
	ifs.close();

	return true;
}

bool ReadTextFile(const AnsiStr& strFN, AnsiStr& strContent )
{
	ifstream ifs(strFN.ptr(), ios::in);
	if(!ifs.is_open())
		return false;

	AnsiStr strLine;
	char buffer[2048];

	while( !ifs.eof())
	{
		ifs.getline(buffer, 2048);
		//ifs >> strLine;
		strLine.copyFromT(buffer);
		strLine.trim();
		strContent.appendFrom(strLine);
	}
	ifs.close();

	return true;
}

}
}
