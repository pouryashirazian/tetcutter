//#include "stdafx.h"
#include "String.h"
#include "stdio.h"

#if defined(__linux__)
	#include <wchar.h>
	#include <stdarg.h>
	#include <string>
#endif
#include "MathBase.h"

namespace PS
{
	//Implementations Start by CAString and then CWString
	//==================================================================================================
	int  CAString::decompose(const char delimiter, std::vector<CAString>& lstWords) const
	{
        int pos = -1;
		lstWords.resize(0);
		CAString strTemp = *this;
		while(strTemp.lfind(delimiter, pos))
		{
			CAString strOut = strTemp.substr(0, pos);
			strOut.trim();
			strOut.removeStartEndSpaces();

			lstWords.push_back(strOut);
			strTemp = strTemp.substr(pos + 1);
			strTemp.removeStartEndSpaces();
		}

		if(strTemp.length() > 0)
			lstWords.push_back(strTemp);

		return (int)lstWords.size();
	}

    bool CAString::lcompare(const char chrBuffer[], int bufSize) const
	{
		if((chrBuffer != NULL)&&(bufSize == 0))
			bufSize = getInputLength(chrBuffer);

		if((chrBuffer == NULL) || (bufSize == 0))
			return -1;

        int ctProcessed = 0;
        int ctTotal = MATHMIN(bufSize, m_length);
        for(int i=0; i < ctTotal; i++)
		{
			if(m_sequence[i] != chrBuffer[i])
			{
				return false;
			}

			ctProcessed++;
		}

		return (ctProcessed == bufSize);
	}


    CAString CAString::substr(int offset, int count) const
	{
		CTString<char> str = substrT(offset, count);
		CAString strOutput(str.ptr());
		return strOutput;
	}

	CAString& CAString::toUpper()
	{
        for (int i=0; i < m_length; i++)
			m_sequence[i] = toupper(m_sequence[i]);	
		return (*this);
	}

	CAString& CAString::toLower()
	{
        for (int i=0; i < m_length; i++)
			m_sequence[i] = tolower(m_sequence[i]);	
		return (*this);
	}

	CAString& CAString::removeStartEndSpaces()
	{	
		//Remove trailing spaces
		while(this->lastChar() == ' ')
			this->resize(m_length - 1);
		
        int pos = 0;
		CAString temp;
        for (int i=0; i < m_length; i++)
		{			
			if(m_sequence[i] != ' ')
			{
				pos = i;			 
				break;
			}
		}
		
        for (int i=pos; i < m_length; i++)
			temp += m_sequence[i];

		this->copyFrom(temp);
		return (*this);
	}

	CAString& CAString::trim()
	{
		CAString temp;		
		temp.reserve(m_allocated);

		//Remove trailing spaces
		while(this->lastChar() == ' ')
			this->resize(m_length - 1);

		char ch;
        for (int i=0; i < m_length; i++)
		{
			 ch = m_sequence[i];
			 if((ch != '\0')&&(ch != '\n')&&(ch != '\r')&&(ch != '\t'))			 
				temp += m_sequence[i];			 
		}

		this->copyFrom(temp);
		return (*this);
	}


	bool operator==(const CAString& a, const CAString& b)	
	{
		return a.isEqual(b);
	}

	bool operator!=(const CAString& a, const CAString& b)
	{
		return !a.isEqual(b);
	}
	
	std::ostream& operator <<(std::ostream& outs, const CAString& src)
	{		
        for (int i = 0; i < src.m_length; i++)
		{
			outs << (src.m_sequence[i]);
		}
		return outs;
	}

	std::istream& operator >>(std::istream& ins, CAString& src)
	{
		CAString strRead;
		char ch;
		strRead.reserve(512);
		while(!ins.eof())
		{
			ins >> ch;
			if((ch == '\n') || (ch == '\0'))			
				break;			
			strRead.appendFromT(ch);
		}
		src = strRead;
		return ins;
	}

	void CAString::operator=(const wchar_t src[])
	{
		this->copyFromW(src);
	}

	void CAString::operator=(const char src[])
	{
		this->copyFromT(src);
	}

	void CAString::operator +=(const CAString& src)
	{
		this->appendFrom(src);
	}

	void CAString::operator +=(const wchar_t src[])
	{
		this->appendFromW(src);
	}

	void CAString::operator +=(wchar_t src[])
	{		
		this->appendFromW(src);
	}

	void CAString::operator +=(const char src[])
	{
		this->appendFromT(src);
	}

	void CAString::operator +=(char src[])
	{
		this->appendFromT(src);
	}

	void CAString::operator +=(const char ch)
	{
		this->appendFromT(ch);
	}

	void CAString::operator +=(const wchar_t ch)
	{
		this->appendFromW(ch);
	}

	CAString CAString::operator +(const CAString& src) const
	{
		CAString x;
		x.copyFrom(*this);
		x.appendFrom(src);
		return x;
	}

	CAString CAString::operator +(const wchar_t src[]) const
	{
		CAString x;
		x.copyFrom(*this);
		x.appendFromW(src);
		return x;
	}

	CAString CAString::operator +(wchar_t src[]) const
	{
		CAString x;
		x.copyFrom(*this);
		x.appendFromW(src);
		return x;	
	}

	CAString CAString::operator +(const wchar_t ch) const
	{
		CAString x;
		x.copyFrom(*this);
		x.appendFromW(ch);
		return x;
	}

	CAString CAString::operator +(const char src[]) const
	{
		CAString x;
		x.copyFrom(*this);
		x.appendFromT(src);
		return x;
	}


	CAString CAString::operator +(char src[]) const
	{
		CAString x;
		x.copyFrom(*this);
		x.appendFromT(src);
		return x;
	}

	CAString CAString::operator +(const char ch) const
	{
		CAString x;
		x.copyFrom(*this);
		x.appendFromT(ch);
		return x;
	}


    void CAString::appendFromW(const wchar_t src[], int srcSize)
	{
		if(src != NULL && srcSize == 0)
			srcSize = getInputLengthW(src);

		if((src == NULL) || (srcSize == 0)) return;

		if(capacity() < (srcSize + DEFAULT_STRING_SIZE))
			reserve(m_allocated + srcSize + DEFAULT_STRING_SIZE);

		char* pDst = &m_sequence[m_length];
        size_t ctConverted = 0;
#ifdef PS_SECURE_API
		errno_t err = wcstombs_s(&ctConverted, pDst, m_allocated - m_length, src, srcSize);
#else
		ctConverted = wcstombs(pDst, src, srcSize);
#endif
		PS_UNUSED(ctConverted);
		m_length += srcSize;
		m_sequence[m_length] = nullChar();
	}


	void CAString::appendFromW(const wchar_t wch)
	{
		if(capacity() < DEFAULT_STRING_SIZE)
			reserve(m_allocated + DEFAULT_STRING_SIZE);

		char *pmb = new char[MB_CUR_MAX];

#ifdef PS_SECURE_API
		int i;
		wctomb_s(&i, pmb, MB_CUR_MAX, wch);
		if(i == 1)
			m_sequence[m_length] = pmb[0];
#else
		if(wctomb(pmb, wch)> 0)
			m_sequence[m_length] = pmb[0];
#endif
		SAFE_DELETE_ARRAY(pmb);

		m_length++;
		m_sequence[m_length] = nullChar();
	}


    int CAString::getInputLength(const char src[]) const
	{
		if(src == NULL)
			return 0;
		return (int)strlen(src);
	}

    int CAString::getInputLengthW(const wchar_t src[]) const
	{
		if(src == NULL)
			return 0;
	#if defined(__linux__)
		wchar_t nc = nullChar();
        int i = 0;
		while(src[i] != nc)	i++;
		return i;
	#else
		return (int)wcslen(src);
	#endif

	}

	CAString::CAString(const CAString& src)
	{
		init();
		copyFrom(src);
	}

    CAString::CAString(const wchar_t src[], int srcSize /* = 0 */)
	{		
		init();
		copyFromW(src, srcSize);
	}

    CAString::CAString(const char src[], int srcSize)
	{	
		init();
		copyFromT(src, srcSize);
	}


    void CAString::copyFromW(const wchar_t src[], int srcSize)
	{
		if(src != NULL && srcSize == 0)
			srcSize = getInputLengthW(src);

		if((src == NULL) || (srcSize == 0))
		{
			reset();
			init();
			return;
		}

		this->reserve(srcSize * 2);
		this->zero_sequence();


		size_t res = 0;
#ifdef PS_SECURE_API
        int ctConverted = 0;
		res = wcstombs_s(&ctConverted, m_sequence, m_allocated, src, m_allocated);
#else
		res = wcstombs(m_sequence, src, m_allocated);
#endif
		if(res == 0)
			m_length = srcSize;					
		else
			m_length = 0;
		m_sequence[m_length] = nullChar();
	}

	//=========================================================================================================
	//Now WideString implementations
	//=========================================================================================================
    CWString CWString::substr(int offset, int count) const
	{
		CTString<wchar_t> str = substrT(offset, count);
		CWString strOutput(str.ptr());
		return strOutput;
	}

	CWString& CWString::toUpper()
	{
#if defined(__linux__)
		CAString str(m_sequence, m_length);
		str.toUpper();
		this->copyFromA(str.cptr(), str.length());
#else
        for (int i=0; i < m_length; i++)
			m_sequence[i] = towupper(m_sequence[i]);
#endif
		return (*this);
	}

	CWString& CWString::toLower()
	{
#if defined(__linux__)
		CAString str(m_sequence, m_length);
		str.toLower();
		this->copyFromA(str.cptr(), str.length());
#else
        for (int i=0; i < m_length; i++)
			m_sequence[i] = towlower(m_sequence[i]);
#endif
		return (*this);
	}

	bool operator==(const CWString& a, const CWString& b)
	{
		return a.isEqual(b);
	}

	bool operator!=(const CWString& a, const CWString& b)
	{
		return !a.isEqual(b);
	}

	std::ostream& operator <<(std::ostream& outs, const CWString& src)	
	{		
		CAString strWrite;
		strWrite.copyFromW(src.ptr(), src.length());
		outs << strWrite.cptr();
		return outs;
	}

	std::istream& operator >>(std::istream& ins, CWString& src)
	{
		CAString strRead;
		char ch;
		strRead.reserve(512);
		while(!ins.eof())
		{
			ins >> ch;
			if((ch == '\n') || (ch == '\0'))			
				break;			
			strRead.appendFromT(ch);
		}
		if(strRead.length() > 0)
			src.copyFromA(strRead.ptr(), strRead.length());
		return ins;
	}


    void CWString::appendFromA(const char src[], int srcSize)
	{
		if(src != NULL && srcSize == 0)
			srcSize = (int)strlen(src);

		if((src == NULL) || (srcSize == 0)) return;

		if(capacity() < (srcSize + DEFAULT_STRING_SIZE))
			reserve(m_allocated + srcSize + DEFAULT_STRING_SIZE);

		wchar_t* pDst = static_cast<wchar_t*>(&m_sequence[m_length]);
#ifdef PS_SECURE_API
        int ctConverted;
		errno_t err = mbstowcs_s(&ctConverted, pDst, m_allocated - m_length, src, srcSize);
#else
		mbstowcs(pDst, src, srcSize);
#endif

		m_length += srcSize;
		m_sequence[m_length] = nullChar();

	}

	void CWString::appendFromA(const char ch)
	{
		if(capacity() < DEFAULT_STRING_SIZE)
			reserve(m_allocated + DEFAULT_STRING_SIZE);

		int i;
		wchar_t* pwc = new wchar_t[1];
		i = mbtowc(pwc, &ch, MB_CUR_MAX);
		if(i == 1)
		{
			m_sequence[m_length] = *pwc;				
		}
		SAFE_DELETE_ARRAY(pwc);


		m_length++;
		m_sequence[m_length] = nullChar();

	}

	CWString CWString::operator +(const CWString& src) const
	{
		CWString x;
		x.copyFrom(*this);
		x.appendFrom(src);
		return x;
	}

	CWString CWString::operator +(const wchar_t src[]) const
	{
		CWString x;
		x.copyFrom(*this);
		x.appendFromT(src);
		return x;
	}

	CWString CWString::operator +(wchar_t src[]) const
	{
		CWString x;
		x.copyFrom(*this);
		x.appendFromT(src);
		return x;
	}

	CWString CWString::operator +(const wchar_t ch) const
	{
		CWString x;
		x.copyFrom(*this);
		x.appendFromT(ch);
		return x;
	}


	CWString CWString::operator +(const char src[]) const
	{
		CWString x;
		x.copyFrom(*this);
		x.appendFromA(src);
		return x;
	}

	CWString CWString::operator +(char src[]) const
	{
		CWString x;
		x.copyFrom(*this);
		x.appendFromA(src);
		return x;
	}

	CWString CWString::operator +(const char ch) const
	{
		CWString x;
		x.copyFrom(*this);
		x.appendFromA(ch);
		return x;
	}


	void CWString::operator +=(const CWString& src)
	{
		this->appendFrom(src);
	}

	void CWString::operator +=(const wchar_t src[])
	{
		this->appendFromT(src);
	}

	void CWString::operator +=(wchar_t src[])
	{
		this->appendFromT(src);
	}

	void CWString::operator +=(const char src[])
	{
		this->appendFromA(src);
	}

	void CWString::operator +=(char src[])
	{
		this->appendFromA(src);
	}

	void CWString::operator +=(const char ch)
	{
		this->appendFromA(ch);
	}

	void CWString::operator +=(const wchar_t ch)
	{
		this->appendFromT(ch);
	}


	void CWString::operator=(const char src[])
	{
		this->copyFromA(src);
	}

	void CWString::operator=(const wchar_t src[])
	{
		this->copyFromT(src);
	}

	CWString::CWString(const CWString& src)
	{
		init();
		copyFrom(src);
	}

    CWString::CWString(const char src[], int srcSize /* = 0 */)
	{
		init();
		copyFromA(src, srcSize);
	}

    CWString::CWString(const wchar_t src[], int srcSize /* = 0 */)
	{
		init();
		copyFromT(src, srcSize);
	}

    int CWString::getInputLength(const wchar_t src[]) const
	{
		if(src == NULL)
			return 0;
	#if defined(__linux__)
		wchar_t nc = nullChar();
        int i = 0;
		while(src[i] != nc)	i++;
		return i;
	#else
		return (int)wcslen(src);
	#endif
	}


    void CWString::copyFromA(const char src[], int srcSize)
	{
		if(src != NULL && srcSize == 0)
			srcSize = (int)strlen(src);

		if((src == NULL) || (srcSize == 0)) 
		{
			reset();
			init();
			return;
		}

		this->reserve(srcSize * 2);
		this->zero_sequence();

#ifdef PS_SECURE_API
        int ctConverted;
		errno_t err = mbstowcs_s(&ctConverted, (wchar_t*)m_sequence, m_allocated, src, srcSize);
		if(err == 0)
			m_length = srcSize;
		else
			m_length = 0;
#else
		size_t res = mbstowcs(m_sequence, src, srcSize);
		if(res != 0)
			m_length = srcSize;
		else
			m_length = 0;
#endif
		m_sequence[m_length] = nullChar();

	}

	//==================================================================
	//Global functions
	CAString toAnsiString(const CWString& strWide)
	{
		CAString strOutput;

		strOutput.copyFromW(strWide.ptr(), strWide.length());
		return strOutput;
	}

	//==================================================================
	CWString toWideString(const CAString& strAnsi)
	{
		CWString strOutput;

		strOutput.copyFromA(strAnsi.ptr(), strAnsi.length());
		return strOutput;
	}

	//==================================================================
	CAString printToAStr( const char *pFmt, ... )
	{
		va_list	vl;
		va_start( vl, pFmt );

		char	buff[MAX_STRING_BUFFER_LEN];

#ifdef PS_SECURE_API
		vsnprintf_s( buff, MAX_STRING_BUFFER_LEN, _TRUNCATE, pFmt, vl );
#else
		vsnprintf(buff, MAX_STRING_BUFFER_LEN, pFmt, vl);
#endif

		va_end( vl );

		AnsiStr strOut = buff;
		return strOut;
	}
	//==================================================================
	CWString printToWStr( const char *pFmt, ... )
	{
		va_list	vl;
		va_start( vl, pFmt );

		char	buff[MAX_STRING_BUFFER_LEN];

#ifdef PS_SECURE_API
		vsnprintf_s( buff, MAX_STRING_BUFFER_LEN, _TRUNCATE, pFmt, vl );
#else
		vsnprintf(buff, MAX_STRING_BUFFER_LEN, pFmt, vl);
#endif

		va_end( vl );

		WideStr strOut;
		strOut.copyFromA(buff);
		return strOut;
	}


}
