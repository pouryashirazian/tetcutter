#pragma once
#ifndef PS_APPCONFIG_H
#define PS_APPCONFIG_H

#include <vector>
#include <fstream>
#include "String.h"
#include "Vec.h"


using namespace std;
using namespace PS::MATH;

namespace PS{


	class SettingsScript
	{
	public:
		enum FileMode {fmRead, fmWrite, fmReadWrite, fmMemoryStream};
	protected:        
		AnsiStr   m_strFileName;
		FileMode   m_fmode;
		std::vector<AnsiStr> m_content;

		bool readFile();
		bool writeFile();

	public:
		//const int MAX_LINE_SIZE;
		SettingsScript():m_strFileName(""), m_fmode(fmRead) {};
		SettingsScript(const AnsiStr& strFileName, FileMode mode = fmReadWrite);
		virtual ~SettingsScript();

		FileMode getFileMode() const {return m_fmode;}
		AnsiStr getFileName() const {return m_strFileName;}
        
        //Gets the entire section as stringlist
        int extractSection(const AnsiStr& title, std::vector<AnsiStr>& lines);

		void clearContentBuffer();
		void getContentBuffer(std::vector<AnsiStr> outContentBuf) const;
		void setContentBuffer(const std::vector<AnsiStr>& inContentBuf);

		void set(const AnsiStr& strFileName, FileMode mode = fmReadWrite);
		bool setForRead();
		void setForWrite();

		AnsiStr getFilePath() const {return m_strFileName;}

		int hasSection(const AnsiStr& strSection);
		bool writeValue(const AnsiStr& section, const AnsiStr& variable, const AnsiStr& strValue);

		bool readLine(const AnsiStr& section, const AnsiStr& variable, AnsiStr& strLine);
		bool readValue(const AnsiStr& section, const AnsiStr& variable, AnsiStr& strValue);

		int readInt(const AnsiStr& section, const AnsiStr& variable, int def = 0);
		void writeInt(const AnsiStr& section, const AnsiStr& variable, int val);

		float readFloat(const AnsiStr& section, const AnsiStr& variable, float def = 0.0f);
		void writeFloat(const AnsiStr& section, const AnsiStr& variable, float val);

		double readDouble(const AnsiStr& section, const AnsiStr& variable, double def = 0.0);
		void writeDouble(const AnsiStr& section, const AnsiStr& variable, double val);

		AnsiStr readString(const AnsiStr& section, const AnsiStr& variable, const AnsiStr& def = AnsiStr(""));
		void writeString(const AnsiStr& section, const AnsiStr& variable, const AnsiStr& val);

		bool readBool(const AnsiStr& section, const AnsiStr& variable, bool def = false);
		void writeBool(const AnsiStr& section, const AnsiStr& variable, bool val);

		bool readIntArray(const AnsiStr& section, const AnsiStr& variable, int ctExpected, std::vector<int>& arrayInt);
		int writeIntArray(const AnsiStr& section, const AnsiStr& variable, const std::vector<int>& arrayInt);

		bool readIntArrayU32(const AnsiStr& section, const AnsiStr& variable, int ctExpected, std::vector<U32>& arrayInt);
		int writeIntArrayU32(const AnsiStr& section, const AnsiStr& variable, const std::vector<U32>& arrayInt);

        
        //Read and Write Vectors
		vec2f readVec2f(const AnsiStr& section, const AnsiStr& variable);
		void writeVec2f(const AnsiStr& section, const AnsiStr& variable, const vec2f& val);
        
		vec3f readVec3f(const AnsiStr& section, const AnsiStr& variable);
		void writeVec3f(const AnsiStr& section, const AnsiStr& variable, const vec3f& val);
        
		vec4f readVec4f(const AnsiStr& section, const AnsiStr& variable);
		void writeVec4f(const AnsiStr& section, const AnsiStr& variable, const vec4f& val);
	};
}
#endif
