/*
 * Profiler.cpp
 *
 *  Created on: Oct 14, 2013
 *      Author: pourya
 */
#include "Profiler.h"
#include "Logger.h"
#include "FileDirectory.h"
#include <fstream>

using namespace PS::FILESTRINGUTILS;

namespace PS {

ProfileAutoEvent::ProfileAutoEvent(const char* filename, const char* funcname, int line, const char* desc) {
	psProfileStart(filename, funcname, line, desc);
}

ProfileAutoEvent::~ProfileAutoEvent() {
	psProfileEnd();
}
///////////////////////////////////////////////////////////////////////////////////////////////////

ProfileEvent::ProfileEvent(const char* filename, const char* funcname, int line, const char* desc) {
	m_strFileName = AnsiStr(filename);
	m_strFuncName = AnsiStr(funcname);
	if(desc == NULL)
		m_strDesc = AnsiStr("ProfileEvent");
	else
		m_strDesc = AnsiStr(desc);
	m_line = line;
	m_timeMS = 0.0;
}

ProfileEvent::~ProfileEvent() {
}

void ProfileEvent::start() {
	m_start = tbb::tick_count::now();
}

double ProfileEvent::end() {
	m_end = tbb::tick_count::now();
	m_timeMS =  (m_end - m_start).seconds() * 1000.0;
	return m_timeMS;
}

//////////////////////////////////////////////////////////////////////////////////
ProfileSession::ProfileSession() {
	m_strTextFile = ChangeFileExt(GetExePath(), AnsiStr(".psprofile.txt"));
	m_strSQLDB = ChangeFileExt(GetExePath(), AnsiStr(".sqlite"));

	cleanup();
}

ProfileSession::~ProfileSession() {
	cleanup();
}

void ProfileSession::cleanup() {
	m_isStatsValid = false;
	m_avgMS = m_lowestMS = m_highestMS = 0.0;
	m_lpSlowestE = m_lpFastestE = NULL;

	//Cleanup
	for(U32 i=0; i<m_vEvents.size(); i++)
		SAFE_DELETE(m_vEvents[i]);
	m_vEvents.resize(0);

	while(!m_stkCurrent.empty())
		m_stkCurrent.pop();
}

void ProfileSession::start() {
	cleanup();
	m_tickStart = tbb::tick_count::now();
}

void ProfileSession::end() {
	//Compute Stats and set valid
	if(m_vEvents.size() == 0)
		return;


	m_highestMS = m_vEvents[0]->timeMS();
	m_lowestMS = m_vEvents[0]->timeMS();
	m_lpSlowestE = m_lpFastestE = m_vEvents[0];
	double sum = m_lowestMS;
	double current = 0.0;
	for(int i=1; i<(int)m_vEvents.size(); i++) {

		current = m_vEvents[i]->timeMS();
		if(current > m_highestMS) {
			m_highestMS = current;
			m_lpSlowestE = m_vEvents[i];
		}

		if(current < m_lowestMS) {
			m_lowestMS = current;
			m_lpFastestE = m_vEvents[i];
		}

		sum += current;
	}

	m_avgMS = sum / static_cast<double>(m_vEvents.size());
	m_isStatsValid = true;
	m_tickEnd = tbb::tick_count::now();
}

//Events
void ProfileSession::startEvent(ProfileEvent* e) {
	if(e == NULL)
		return;

	m_vEvents.push_back(e);
	m_stkCurrent.push(e);
}

ProfileEvent* ProfileSession::endEvent() {
	if(m_stkCurrent.empty()) {
		LogError("The Profiler stack is empty! Did you forget to end an event before starting a new one?");
		return NULL;
	}

	ProfileEvent* lpEvent = m_stkCurrent.top();
	m_stkCurrent.pop();
	lpEvent->end();
	return lpEvent;
}

ProfileEvent* ProfileSession::get(int index) const {
	if(!isEventIndex(index))
		return NULL;
	return m_vEvents[index];
}

AnsiStr ProfileSession::toString() const {
	AnsiStr str;
	if(m_isStatsValid) {
		str = printToAStr("SLOWEST Event: %.3f and FASTEST Event: %.3f and AVG: %.3f",
                          m_highestMS, m_lowestMS, m_avgMS);
    }
	else
		str = "Results are not computed yet!";
	return str;
}

double ProfileSession::duration() const {
	 return (m_tickEnd - m_tickStart).seconds() * 1000.0;
}

//Write profiler events into the text file
int ProfileSession::writeToTextFile() {
	if(m_vEvents.size() == 0)
		return 0;

	ofstream ofs;
	if(FileExists(m_strTextFile))
		ofs.open(m_strTextFile.cptr(), ios::out | ios::app);
	else
		ofs.open(m_strTextFile.cptr(), ios::out);
	if(!ofs.is_open())
		return false;

	ProfileEvent* lpEvent = NULL;
	AnsiStr strLine;
	for(size_t i=0; i < m_vEvents.size(); i++)
	{
		lpEvent = m_vEvents[i];
		strLine = lpEvent->filename() + AnsiStr(":") + lpEvent->funcname() + printToAStr(":%d", lpEvent->line());
		strLine += printToAStr(" %s took %.4f [ms]", lpEvent->desc().cptr(), lpEvent->timeMS());
		ofs << strLine << '\0' << endl;
	}
	ofs.close();

	return 1;
}

int ProfileSession::writeToSqlDB() {
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
Profiler::Profiler() {
	m_flags = pbInjectToLogger | pbWriteToTextFile;
}

Profiler::~Profiler() {
	flush();
}

void Profiler::flush() {
	if((m_flags & pbWriteToTextFile) != 0)
		m_session.writeToTextFile();
	if((m_flags & pbWriteToSqlDB) != 0)
		m_session.writeToSqlDB();
}

void Profiler::startSession() {
	flush();
	m_session.start();
}

void Profiler::endSession() {
	m_session.end();
	if((m_flags & pbInjectToLogger) != 0)
		LogInfoArg1("Session ended: Stats: %s", m_session.toString().cptr());
}


void Profiler::startEvent(const char* filename, const char* funcname, int line, const char* desc) {
	ProfileEvent* lpEvent = new ProfileEvent(filename, funcname, line, desc);
	lpEvent->start();
	m_session.startEvent(lpEvent);
}

double Profiler::endEvent() {
	ProfileEvent* e = m_session.endEvent();
	if(e == NULL)
		return 0.0;

	//Inject log info
	if((m_flags & pbInjectToLogger) != 0) {
		AnsiStr strSource = e->filename() + AnsiStr(":") + e->funcname();
		psLog(EventLogger::etInfo, strSource.cptr(), e->line(), "%s Took %.4f [ms]", e->desc().cptr(), e->timeMS());
	}

	//Flush if exceeded the container size and there is no pending events
	if(m_session.count() > MAX_LOG_EVENTS && !m_session.hasPendingEvents())
		flush();

	return e->timeMS();
}


tick Profiler::GetTickCount() {
	return tbb::tick_count::now();
}

bool Profiler::getInjectToLogFlag() const {
	return ((m_flags & pbInjectToLogger) != 0);
}

void Profiler::setInjectToLogFlag(bool enable) {
	if(enable)
		m_flags |= pbInjectToLogger;
	else if(m_flags & pbInjectToLogger)
		m_flags ^= pbInjectToLogger;
}


//////////////////////////////////////////////////////////////////////////////////////////
void psProfileStart(const char* filename, const char* funcname, int line, const char* desc) {
	TheProfiler::Instance().startEvent(filename, funcname, line, desc);
}

void psProfileEnd() {
	TheProfiler::Instance().endEvent();
}


}


