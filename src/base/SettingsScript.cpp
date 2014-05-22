#include "SettingsScript.h"
#include "FileDirectory.h"
#include "Logger.h"

#define MAX_CFG_LINE_SIZE 256

namespace PS{

	using namespace PS::FILESTRINGUTILS;

	//====================================================================================
	SettingsScript::SettingsScript(const AnsiStr& strFileName, FileMode mode)
	{
		set(strFileName, mode);
	}
	//====================================================================================
	bool SettingsScript::setForRead()
	{
		//m_strFileName = changeFileExt(getExePath(), AnsiStr(".inf"));
		m_strFileName = CreateNewFileAtRoot(".inf");
		m_fmode = fmRead;
		m_content.resize(0);
		return readFile();
	}
	//====================================================================================
	void SettingsScript::set( const AnsiStr& strFileName, FileMode mode /*= fmReadWrite*/ )
	{
		m_fmode = mode;
		m_strFileName = strFileName;
		m_content.resize(0);

		if(m_fmode == fmRead || m_fmode == fmReadWrite)
			readFile();
	}
	//====================================================================================
	void SettingsScript::setForWrite()
	{
		m_strFileName = CreateNewFileAtRoot(".inf");
		m_fmode = fmWrite;
	}
	//====================================================================================
	SettingsScript::~SettingsScript()
	{
		if((m_fmode == fmReadWrite)||(m_fmode == fmWrite))
		{
			if(m_content.size() > 0)
			{
				writeFile();
			}
		}

		m_content.resize(0);
	}

	//====================================================================================
	bool SettingsScript::readFile()
	{

		ifstream ifs(m_strFileName.ptr(), ios::in);
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
			m_content.push_back(strLine);
		}
		ifs.close();

		return true;
	}
	//====================================================================================
	bool SettingsScript::writeFile()
	{
		ofstream ofs(m_strFileName.ptr(), ios::out | ios::trunc);
		if(!ofs.is_open())
			return false;

		AnsiStr strLine;
        for(int i=0; i < (int)m_content.size(); i++)
		{
			strLine = m_content[i];
			if(strLine.length() > 0)
			{
				ofs << strLine.cptr() << '\n';
			}
		}
		ofs.close();

		return true;
	}
	//====================================================================================
    int SettingsScript::extractSection(const AnsiStr& title, std::vector<AnsiStr>& lines) {

        lines.resize(0);
        AnsiStr str;
        
        AnsiStr titleUpperCased = title;
        titleUpperCased.toUpper();
        int idxStart = -1;
        for(int i=0; i < (int)m_content.size(); i++)
		{
			str = m_content[i];
            
			if((str.firstChar() == '[')&&(str.lastChar() == ']'))
			{
				//[ + ] =  2
				str = str.substr(1, str.length() - 2);
				if(str.toUpper() == titleUpperCased) {
					idxStart = i;
                    break;
                }
			}
		}
        
        //Process Content        
        for(int i=idxStart+1; i < (int)m_content.size(); i++) {
			str = m_content[i];
			if((str.firstChar() == '[')&&(str.lastChar() == ']'))
                break;
            else
                lines.push_back(str);
        }

        return (int)lines.size();
    }
	//====================================================================================
	int SettingsScript::hasSection(const AnsiStr& strSection)
	{
		AnsiStr str;
        for(int i=0; i < (int)m_content.size(); i++)
		{
			str = m_content[i];
			char ch = str.firstChar();
			ch = str.lastChar();

			if((str.firstChar() == '[')&&(str.lastChar() == ']'))
			{
				//[ + ] =  2
				str = str.substr(1, str.length() - 2);
				AnsiStr strSecUpperCased = strSection;
				strSecUpperCased.toUpper();
				if(str.toUpper() == strSecUpperCased)
					return i;
			}
		}

		return -1;
	}
	//====================================================================================
	bool SettingsScript::writeValue(const AnsiStr& section, const AnsiStr& variable, const AnsiStr& strValue)
	{
		AnsiStr strLine, strToAdd;
		strToAdd = variable;
		strToAdd += "=";
		strToAdd += strValue;

		int startPos = hasSection(section);
		if(startPos >= 0)
		{
			//Record file pos
            for(int i=startPos+1; i < (int)m_content.size(); i++)
			{
				strLine = m_content[i];

				//If it is a comment then ignore
				if(strLine.firstChar() == '#')
					continue;

				//Is it another section?
				if((strLine.firstChar() == L'[')&&(strLine.lastChar() == L']'))
				{
					m_content.insert(m_content.begin() + i, strToAdd);
					return true;
				}
				else
				{
					//Check Variable
                    for(int iChar=0; iChar < strLine.length(); iChar++)
					{
						if(strLine[iChar] == L'=')
						{
							strLine = strLine.substr(0, iChar);

							AnsiStr varUpperCased = variable;
							varUpperCased.toUpper();
							if( strLine.toUpper() == varUpperCased )
							{
								//Write it here
								m_content[i] = strToAdd;
								return true;
							}
							else
								break;
						}
					}
				}
			}

			//Not Written
			m_content.push_back(strToAdd);
		}
		else
		{
			//Add it if not found anywhere
			strLine = AnsiStr("[") + section + AnsiStr("]");
			m_content.push_back(strLine);
			m_content.push_back(strToAdd);
		}

		return true;
	}

	//====================================================================================
	bool SettingsScript::readLine(const AnsiStr& section, const AnsiStr& variable, AnsiStr& strLine)
	{
		int startPos = hasSection(section);
		if(startPos < 0) return false;

		//Record file pos
        for(int i=startPos+1; i < (int)m_content.size(); i++)
		{
			strLine = m_content[i];

			//If it is a comment then ignore
			if(strLine.firstChar() == '#')
				continue;


			//Is section?
			if((strLine.firstChar() == L'[')&&(strLine.lastChar() == L']'))
			{
				return false;
			}
			else
			{
                for(int iChar=0; iChar < strLine.length(); iChar++)
				{
					if(strLine[iChar] == L'=')
					{
						AnsiStr str = strLine.substr(0, iChar);
						AnsiStr varUpperCased = variable;
						varUpperCased.toUpper();
						if( str.toUpper() == varUpperCased )
						{
							return true;
						}
						else
							break;
					}
				}
			}

		}

		return false;
	}

	//====================================================================================
	bool SettingsScript::readValue(const AnsiStr& section, const AnsiStr& variable, AnsiStr& strValue)
	{
		AnsiStr strLine = "";
		if(readLine(section, variable, strLine))
		{
            int len = strLine.length();
            for(int i=0; i < len; i++)
			{
				if(strLine[i] == L'=')
				{
					strValue = strLine.substr(i+1);
					return true;
				}
			}

		}
		else
		{
			AnsiStr strFileTitle = ExtractFileName(m_strFileName);
			LogErrorArg3("SettingsScript tried to read [file: %s; section: %s; variable: %s] which is not found.",
				strFileTitle.cptr(),
				section.cptr(),
				variable.cptr());
		}
		return false;
	}
	//====================================================================================
	int SettingsScript::readInt(const AnsiStr& section, const AnsiStr& variable, int def)
	{
		AnsiStr strVal;
		if(readValue(section, variable, strVal))
			//def = atoi(strVal.c_str());
			def = atoi(strVal.ptr());
		return def;
	}

	//====================================================================================
	void SettingsScript::writeInt(const AnsiStr& section, const AnsiStr& variable, int val)
	{
		writeValue(section, variable, printToAStr("%d", val));
	}
	//====================================================================================
	float SettingsScript::readFloat(const AnsiStr& section, const AnsiStr& variable, float def)
	{
		AnsiStr strVal;
		if(readValue(section, variable, strVal))
			//def = static_cast<float>(atof(strVal.c_str()));
			def = static_cast<float>(atof(strVal.ptr()));
		return def;

	}
	//====================================================================================
	void SettingsScript::writeFloat(const AnsiStr& section, const AnsiStr& variable, float val)
	{
		writeValue(section, variable, printToAStr("%f", val));
	}
	//====================================================================================
	double SettingsScript::readDouble(const AnsiStr& section, const AnsiStr& variable, double def)
	{
		AnsiStr strVal;
		if(readValue(section, variable, strVal))
			//def = atof(strVal.c_str());
			def = atof(strVal.ptr());
		return def;

	}
	//====================================================================================
	void SettingsScript::writeDouble(const AnsiStr& section, const AnsiStr& variable, double val)
	{
		writeValue(section, variable, printToAStr("%f", val));
	}
	//====================================================================================
	AnsiStr SettingsScript::readString(const AnsiStr& section, const AnsiStr& variable, const AnsiStr& def)
	{
		AnsiStr strVal;
		if(readValue(section, variable, strVal))
			return strVal;
		else
			return def;
	}

	//====================================================================================
	void SettingsScript::writeString(const AnsiStr& section, const AnsiStr& variable, const AnsiStr& val)
	{
		writeValue(section, variable, val);
	}

	//====================================================================================
	bool SettingsScript::readBool(const AnsiStr& section, const AnsiStr& variable, bool def)
	{
		AnsiStr strVal;
		if(readValue(section, variable, strVal))
			//def = (atoi(strVal.c_str()) == 1)?true:false;
			def = (atoi(strVal.ptr()) == 1)?true:false;
		return def;

	}

	//====================================================================================
	void SettingsScript::writeBool(const AnsiStr& section, const AnsiStr& variable, bool val)
	{
		if(val)
			writeValue(section, variable, "1");
		else
			writeValue(section, variable, "0");
	}

	//====================================================================================
	bool SettingsScript::readIntArray(const AnsiStr& section, const AnsiStr& variable, int ctExpected, std::vector<int>& arrayInt)
	{
		AnsiStr strVal;
		if(readValue(section, variable, strVal))
		{
            int pos;
			int iComp = 0;
			AnsiStr strTemp;
			if(strVal.firstChar() == '(')
				strVal = strVal.substr(1);
			else
				return false;
			while(strVal.lfind(',', pos))
			{
				strTemp = strVal.substr(0, pos);
				strVal = strVal.substr(pos + 1);
				strVal.removeStartEndSpaces();
				arrayInt.push_back(atoi(strTemp.ptr()));
				iComp++;
			}

			if(strVal.length() >= 1)
			{
				if(strVal.lastChar() == ')')
				{
					strTemp = strVal.substr(0, strVal.length() - 1);
					strTemp.removeStartEndSpaces();
					if(strTemp.length() > 0)
						arrayInt.push_back(atoi(strTemp.ptr()));
				}
			}
		}
		return ((int)arrayInt.size() == ctExpected);
	}

	//====================================================================================
	int SettingsScript::writeIntArray(const AnsiStr& section, const AnsiStr& variable, const std::vector<int>& arrayInt)
	{
		AnsiStr strValue, strTemp;
		if(arrayInt.size() > 1)
		{
            for(int i=0; i<(int)arrayInt.size(); i++)
			{
				if(i == 0)
					strTemp = printToAStr("(%d, ", arrayInt[i]);
                else if(i == (int)arrayInt.size() - 1)
					strTemp = printToAStr("%d)", arrayInt[i]);
				else
					strTemp = printToAStr("%d, ", arrayInt[i]);
				strValue += strTemp;
			}
			writeValue(section, variable, strValue);
		}
		else if(arrayInt.size() == 1)
		{
			strTemp = printToAStr("(%d)", arrayInt[0]);
			writeValue(section, variable, strTemp);
		}
		else
			writeValue(section, variable, AnsiStr("()"));
		return (int)arrayInt.size();
	}
	

	bool SettingsScript::readIntArrayU32(const AnsiStr& section, const AnsiStr& variable, int ctExpected, std::vector<U32>& arrayInt)
	{
		AnsiStr strVal;
		if(readValue(section, variable, strVal))
		{
            int pos;
			int iComp = 0;
			AnsiStr strTemp;
			if(strVal.firstChar() == '(')
				strVal = strVal.substr(1);
			else
				return false;
			while(strVal.lfind(',', pos))
			{
				strTemp = strVal.substr(0, pos);
				strVal = strVal.substr(pos + 1);
				strVal.removeStartEndSpaces();
				arrayInt.push_back(atoi(strTemp.ptr()));
				iComp++;
			}

			if(strVal.length() >= 1)
			{
				if(strVal.lastChar() == ')')
				{
					strTemp = strVal.substr(0, strVal.length() - 1);
					strTemp.removeStartEndSpaces();
					if(strTemp.length() > 0)
						arrayInt.push_back(atoi(strTemp.ptr()));
				}
			}
		}
		return ((int)arrayInt.size() == ctExpected);
	}

	//====================================================================================
	int SettingsScript::writeIntArrayU32(const AnsiStr& section, const AnsiStr& variable, const std::vector<U32>& arrayInt)
	{
		AnsiStr strValue, strTemp;
		if(arrayInt.size() > 1)
		{
            for(int i=0; i<(int)arrayInt.size(); i++)
			{
				if(i == 0)
					strTemp = printToAStr("(%d, ", arrayInt[i]);
                else if(i == (int)arrayInt.size() - 1)
					strTemp = printToAStr("%d)", arrayInt[i]);
				else
					strTemp = printToAStr("%d, ", arrayInt[i]);
				strValue += strTemp;
			}
			writeValue(section, variable, strValue);
		}
		else if(arrayInt.size() == 1)
		{
			strTemp = printToAStr("(%d)", arrayInt[0]);
			writeValue(section, variable, strTemp);
		}
		else
			writeValue(section, variable, AnsiStr("()"));
		return (int)arrayInt.size();
	}


	void SettingsScript::clearContentBuffer()
	{
		m_content.resize(0);
	}

	void SettingsScript::getContentBuffer( std::vector<AnsiStr> outContentBuf ) const
	{
		outContentBuf.assign(m_content.begin(), m_content.end());
	}

	void SettingsScript::setContentBuffer( const std::vector<AnsiStr>& inContentBuf )
	{
		m_content.assign(inContentBuf.begin(), inContentBuf.end());
	}
    
    
    //====================================================================================
	vec2f SettingsScript::readVec2f(const AnsiStr& section, const AnsiStr& variable)
	{
		AnsiStr strVal;
		vec2f res;
		if(readValue(section, variable, strVal))
		{
			float f[2];
            int pos;
			int iComp = 0;
			AnsiStr strTemp;
            
			if(strVal.firstChar() == '(')
				strVal = strVal.substr(1);
			else
				return res;
			while(strVal.lfind(',', pos))
			{
				strTemp = strVal.substr(0, pos);
				strVal = strVal.substr(pos + 1);
				strVal.removeStartEndSpaces();
				f[iComp] = static_cast<float>(atof(strTemp.ptr()));
				iComp++;
			}
            
			if(strVal.length() >= 1 && iComp < 2)
			{
				if(strVal.lastChar() == ')')
				{
					strTemp = strVal.substr(0, strVal.length() - 1);
					strTemp.removeStartEndSpaces();
					f[iComp] = static_cast<float>(atof(strTemp.ptr()));
				}
			}
            
			res = vec2f(f[0], f[1]);
		}
		return res;
	}
    
	//====================================================================================
	void SettingsScript::writeVec2f(const AnsiStr& section, const AnsiStr& variable, const vec2f& val)
	{
		AnsiStr strTemp = printToAStr("(%f, %f)", val.x, val.y);
		writeValue(section, variable, strTemp);
	}
	//====================================================================================
	vec3f SettingsScript::readVec3f(const AnsiStr& section, const AnsiStr& variable)
	{
		AnsiStr strVal;
		vec3f res;
		if(readValue(section, variable, strVal))
		{
			float f[4];
            int pos;
			int iComp = 0;
			AnsiStr strTemp;
            
			if(strVal.firstChar() == '(')
				strVal = strVal.substr(1);
			else
				return res;
			while(strVal.lfind(',', pos))
			{
				strTemp = strVal.substr(0, pos);
				strVal = strVal.substr(pos + 1);
				strVal.removeStartEndSpaces();
				f[iComp] = static_cast<float>(atof(strTemp.ptr()));
				iComp++;
			}
            
			if(strVal.length() >= 1 && iComp < 3)
			{
				if(strVal.lastChar() == ')')
				{
					strTemp = strVal.substr(0, strVal.length() - 1);
					strTemp.removeStartEndSpaces();
					f[iComp] = static_cast<float>(atof(strTemp.ptr()));
				}
			}
            
			res = vec3f(f[0], f[1], f[2]);
		}
		return res;
	}
    
	//====================================================================================
	void SettingsScript::writeVec3f(const AnsiStr& section, const AnsiStr& variable, const vec3f& val)
	{
		AnsiStr strTemp = printToAStr("(%f, %f, %f)", val.x, val.y, val.z);
		writeValue(section, variable, strTemp);
	}
    
	//====================================================================================
	vec4f SettingsScript::readVec4f(const AnsiStr& section, const AnsiStr& variable)
	{
		AnsiStr strVal;
		vec4f res;
		if(readValue(section, variable, strVal))
		{
			float f[4];
            int pos;
			int iComp = 0;
			AnsiStr strTemp;
            
			if(strVal.firstChar() == '(')
				strVal = strVal.substr(1);
			else
				return res;
			while(strVal.lfind(',', pos))
			{
				strTemp = strVal.substr(0, pos);
				strVal = strVal.substr(pos + 1);
				strVal.removeStartEndSpaces();
				f[iComp] = static_cast<float>(atof(strTemp.ptr()));
				iComp++;
			}
            
			if(strVal.length() >= 1 && iComp < 4)
			{
				if(strVal.lastChar() == ')')
				{
					strTemp = strVal.substr(0, strVal.length() - 1);
					strTemp.removeStartEndSpaces();
					f[iComp] = static_cast<float>(atof(strTemp.ptr()));
				}
			}
            
			res = vec4f(f[0], f[1], f[2], f[3]);
		}
		return res;
	}
    
	//====================================================================================
	void SettingsScript::writeVec4f(const AnsiStr& section, const AnsiStr& variable, const vec4f& val)
	{
		AnsiStr strTemp = printToAStr("(%f, %f, %f, %f)", val.x, val.y, val.z, val.w);
		writeValue(section, variable, strTemp);
	}
	//====================================================================================

}
