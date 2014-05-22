/*
 * SettingsDB.cpp
 *
 *  Created on: Jan 29, 2014
 *      Author: pourya
 */
#include "SettingsDB.h"
#include "SettingsScript.h"
#include "Value.h"
#include "String.h"
#include "Logger.h"

namespace PS {
    SettingsDB::SettingsDB() {
        
    }
    
    SettingsDB::~SettingsDB() {
        
    }
    
    void SettingsDB::printHelp() {

    	printf("Usage: The following values can be set:\n");
        for(ValueStorage::CONST_ITER it = this->cbegin(); it != this->cend(); it++) {
            try {
                printf("Name: %s, Type: %s\n", it->first.c_str(), it->second.type_info().name());
            }
            catch(std::invalid_argument& e) {
                LogErrorArg2("unable to store value named %s. details: %s", it->first.c_str(), e.what());
            }
        }
    }

    //IO
    int SettingsDB::processCmdLine(int argc, char **argv) {
        int iArg = 0;
        
        int processed = 0;
        while(iArg < argc) {
            AnsiStr str = AnsiStr(argv[iArg]);
            
            if(str.firstChar() == '-') {
                str.replaceChars('-', ' ');
                str.removeStartEndSpaces();
                
                if(str == "help") {
                	printHelp();
                	return -1;
                }

                try {
                	SettingsDB::ITER it = this->at(str.cptr());
                    if(it != this->end()) {
                        string strArg = string(argv[++iArg]);
                        it->second.fromString(strArg);
                        LogInfoArg2("CMD SET: %s=%s", str.c_str(), strArg.c_str());
                    }
                }
                catch(std::invalid_argument& e) {
                    LogErrorArg1("Unable to parse commandline token: %s", str.cptr());
                }
                
                processed++;
            }
            
            ++iArg;
        }
        
        return processed;
    }
}



