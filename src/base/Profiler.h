/*
 * Profiler.h
 *
 *  Created on: Oct 14, 2013
 *      Author: pourya
 */

#ifndef PROFILER_H_
#define PROFILER_H_

#include <vector>
#include <stack>
#include <time.h>
#include "String.h"
#include "loki/Singleton.h"
#include "tbb/tick_count.h"

using namespace std;
using namespace Loki;
using namespace tbb;

#define MAX_LOG_EVENTS	32

//Easy usage with preprocessor
#define ProfileAuto() PS::ProfileAutoEvent profile(__FILE__, __FUNCTION__, __LINE__, NULL);
#define ProfileAutoArg(desc) PS::ProfileAutoEvent profile(__FILE__, __FUNCTION__, __LINE__, desc);

#define	 ProfileStart() psProfileStart(__FILE__, __FUNCTION__, __LINE__, NULL);
#define	 ProfileStartArg(desc) psProfileStart(__FILE__, __FUNCTION__, __LINE__, desc);
#define ProfileEnd() psProfileEnd();

namespace PS {

typedef tbb::tick_count		tick;

class ProfileAutoEvent {
public:
	ProfileAutoEvent(const char* filename, const char* funcname, int line, const char* desc = NULL);
	virtual ~ProfileAutoEvent();
};

/*!
 * Profiler event is a small structure
 */
class ProfileEvent {
public:
	ProfileEvent(const char* filename, const char* funcname, int line, const char* desc = NULL);
	virtual ~ProfileEvent();

	//Set timestamps
	void start();
	double end();

	//Access
	double timeMS() const { return m_timeMS;}
	AnsiStr filename() const {return m_strFileName;}
	AnsiStr funcname() const {return m_strFuncName;}
	AnsiStr desc() const {return m_strDesc;}
	int line() const {return m_line;}
	//AnsiStr toString() const;

	//Ticks
	tick getStartTick() const {return m_start;}
	tick getEndTick() const {return m_end;}
private:
	AnsiStr m_strFileName;
	AnsiStr m_strFuncName;
	AnsiStr m_strDesc;
	int m_line;
	tick m_start;
	tick m_end;
	double m_timeMS;
};


//Aggregated Stats
class ProfileSession {
public:
	ProfileSession();
	virtual ~ProfileSession();

	//Start
	void start();

	//End
	void end();

	//Events
	void startEvent(ProfileEvent* e);
	ProfileEvent* endEvent();
	ProfileEvent* get(int index) const;
	int count() const {return (int)m_vEvents.size();}

	//Avg
	void setAvg(double a) {m_avgMS = a;}
	double getAvg() const {return m_avgMS;}

	//Lowest
	void setLowest(double l) {m_lowestMS = l;}
	double getLowest() const {return m_lowestMS;}

	//Highest
	void setHighest(double h) {m_highestMS = h;}
	double getHightest() const {return m_highestMS;}

	double duration() const;
	void setValid() {m_isStatsValid = true;}

	bool hasPendingEvents() const { return (!m_stkCurrent.empty());}
	AnsiStr toString() const;

	//Serialization
	int writeToTextFile();
	int writeToSqlDB();
	void cleanup();
protected:

	bool isEventIndex(int idx) const { return (idx >= 0 && idx < (int)m_vEvents.size());}


protected:
	AnsiStr m_strTextFile;
	AnsiStr m_strSQLDB;

	//Stats
	ProfileEvent* m_lpSlowestE;
	ProfileEvent* m_lpFastestE;
	double m_avgMS;
	double m_lowestMS;
	double m_highestMS;
	bool m_isStatsValid;
	tick m_tickStart;
	tick m_tickEnd;

	//Storage
	std::stack<ProfileEvent*> m_stkCurrent;
	std::vector<ProfileEvent*> m_vEvents;
};

/*!
 * Profiler manages all profiling events and provides access for reporting
 */
class Profiler {
public:
	enum Behaviour {pbInjectToLogger = 1, pbWriteToTextFile = 2, pbWriteToSqlDB = 4};
public:
	Profiler();
	virtual ~Profiler();
	void flush();

	//Session
	void startSession();
	void endSession();
	ProfileSession& session() {return m_session;}

	//Event Generation
	void startEvent(const char* filename, const char* funcname, int line, const char* desc);
	double endEvent();

	//Flags
	int flags() const { return m_flags;}
	void setWriteFlags(int flags) {m_flags = flags;}
	bool getInjectToLogFlag() const;
	void setInjectToLogFlag(bool enable);


	static tick GetTickCount();

protected:

private:
	ProfileSession m_session;
	int m_flags;
};

//Singleton Instance
typedef SingletonHolder<Profiler, CreateUsingNew, PhoenixSingleton> TheProfiler;

void psProfileStart(const char* filename, const char* funcname, int line, const char* desc = NULL);
void psProfileEnd();

}

#endif /* PROFILER_H_ */
