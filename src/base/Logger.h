/*!
 * A multi-purpose logging system for writing high quality, human readable logs.
 * Usages: Debugging, Performance measurements, Profiling, Event Reviews, Process Control
 * Logs are written to disk after a certain sized buffer is filled. 
 * Event logging system is accessible globally so it can be used in all classes. 
 */
#include <vector>
#include <time.h>
#include "String.h"
#include "loki/Singleton.h"

using namespace std;
using namespace PS;
using namespace Loki;

#define PS_LOG_WRITE_EVENTTYPE 1
#define PS_LOG_WRITE_TIMESTAMP 2
#define PS_LOG_WRITE_SOURCE	   4
#define PS_LOG_WRITE_TO_SCREEN 8
#define PS_LOG_WRITE_TO_MODAL  16

#define PS_LOG_LINE_SIZE	2048
//16 KiloBytes for the memory log
#define PS_LOG_BUFFER_SIZE  8*PS_LOG_LINE_SIZE

//Logging Info, Error or Warning
#define LogInfo(message) psLog(EventLogger::etInfo, __FILE__, __LINE__, message)
#define LogError(message) psLog(EventLogger::etError, __FILE__, __LINE__, message)
#define LogWarning(message) psLog(EventLogger::etWarning, __FILE__, __LINE__, message)

#define LogInfoArg1(message, arg1) psLog(EventLogger::etInfo, __FILE__, __LINE__, message, arg1)
#define LogErrorArg1(message, arg1) psLog(EventLogger::etError, __FILE__, __LINE__, message, arg1)
#define LogWarningArg1(message, arg1) psLog(EventLogger::etWarning, __FILE__, __LINE__, message, arg1)

#define LogInfoArg2(message, arg1, arg2) psLog(EventLogger::etInfo, __FILE__, __LINE__, message, arg1, arg2)
#define LogErrorArg2(message, arg1, arg2) psLog(EventLogger::etError, __FILE__, __LINE__, message, arg1, arg2)
#define LogWarningArg2(message, arg1, arg2) psLog(EventLogger::etWarning, __FILE__, __LINE__, message, arg1, arg2)

#define LogInfoArg3(message, arg1, arg2, arg3) psLog(EventLogger::etInfo, __FILE__, __LINE__, message, arg1, arg2, arg3)
#define LogErrorArg3(message, arg1, arg2, arg3) psLog(EventLogger::etError, __FILE__, __LINE__, message, arg1, arg2, arg3)
#define LogWarningArg3(message, arg1, arg2, arg3) psLog(EventLogger::etWarning, __FILE__, __LINE__, message, arg1, arg2, arg3)


//Display Error Message
typedef void (*FOnDisplay)(const char* message);


namespace PS{

/*!
* Event Logger class for writing major application events to disk. 
*/
class EventLogger{
public:
	EventLogger();

	/*!
	* Constructor for setting the log file path on disk and setting up the flags for 
	* controlling how the log is written.
	* @param lpFilePath the string of the file path
	* @param flags that indicate how each log entry has to be written 
	*/
	EventLogger(const char* lpFilePath, int flags = 0);
	~EventLogger();

	enum EVENTTYPE {etInfo, etWarning, etError};

	
	//Internal Class for holding an instance of an event
	struct Event{
		EventLogger::EVENTTYPE etype;
		AnsiStr strDesc;
		AnsiStr strSource;
		int value;
	};

	/*!
	* Adds an event to the event log system
	* @param e reference to the event variable
	*/
	void add(const Event& e);

	/*!
	* Adds an entry to the log system
	* @param lpStrDesc Description for this event
	* @param t type of this event
	* @param lpSource the source of the event (Can be a File, File + Function Name)
	* @param value can be line number or the error code
	*/
	void add(const char* lpStrDesc, 			  
			 EVENTTYPE t = etInfo,
			 const char* lpStrSource = NULL,
			 int value = 0);

	/*!
	* @param flags to control the way log entries are being written to disk
	*/
	void setWriteFlags(int flags);

	//Set output filepath
	void setOutFilePath(const char* lpStrFilePath);

	//Set error display callback
	void setDisplayCallBack(FOnDisplay cb) {m_fOnDisplay = cb;}


	//Flush the content of string buffer to disk
	bool flush();
    
    AnsiStr rootPath() const {return m_strRootPath;}
    AnsiStr shortenPathBasedOnRoot(const AnsiStr& strPath) const;
private:

	void display(const char* chrMessage) const;

//Private Variables
private:
	//On Display Error
	FOnDisplay m_fOnDisplay;
	
	//Flags to control serialization
	bool m_bWriteEventTypes;
	bool m_bWriteTimeStamps;
	bool m_bWriteSourceInfo;
	bool m_bWriteToScreen;
	bool m_bWriteToModal;

	U32 m_szBufferSize;
	AnsiStr m_strFP;
    AnsiStr m_strRootPath;
	std::vector<AnsiStr> m_lstLog;
};

typedef SingletonHolder<EventLogger, CreateUsingNew, PhoenixSingleton> TheEventLogger;

void psLog(const char* lpDesc, ...);

/*!
* Adds a new entry to log using printf style formatting. 
*/
void psLog(EventLogger::EVENTTYPE etype, const char* lpSource, int line, const char* lpDesc, ...);

}
