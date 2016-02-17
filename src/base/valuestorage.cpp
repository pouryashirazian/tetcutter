//
//  ValueStorage.cpp
//  hifem
//
//  Created by pshiraz on 2/6/14.
//  Copyright (c) 2014 pshiraz. All rights reserved.
//

#include <istream>
#include <fstream>

#include "valuestorage.h"
#include "loki/TypeTraits.h"
#include "inifile.h"
#include "logger.h"

using namespace std;
using namespace Loki;
using namespace ps::utils;

namespace ps {
    
    bool ValueStorage::read(const AnsiStr& strFP) {

        /*
        ifstream fp(strFP.c_str());
        if(!fp.is_open())
            return false;
        string line;
        char t[512];
        char key[512];
        char value[512];

        while(getline(fp, line)) {
            sscanf(str.cptr(), "%s:%s=%s", t, key, value);
            Value v;
        
            
            this->add(Resource element, <#const char *name#>)
        }
         */
        return false;
    }

    bool ValueStorage::write(const AnsiStr& strFP) {
        /*
        ofstream fp;
        fp.open(strFP.cptr());
        
        char buffer[1024];
        for(ValueStorage::CONST_ITER it = this->cbegin(); it != this->cend(); it++) {
            try {
                sprintf(buffer, "%s:%s=%s\n", it->second.typeToString(), it->first.c_str(), it->second.toString().c_str());
                fp << string(buffer);
            }
            catch(std::invalid_argument& e) {
                LogErrorArg2("unable to store value named %s. details: %s", it->first.c_str(), e.what());
            }
        }

        fp.close();
        return true;
         */
        return false;
    }

    void ValueStorage::printAll() {
        for(ValueStorage::CONST_ITER it = this->cbegin(); it != this->cend(); it++) {
            try {
                LogInfoArg2("Current Setting are: %s=%s", it->first.c_str(), it->second.toString().c_str());
            }
            catch(std::invalid_argument& e) {
                LogErrorArg2("unable to store value named %s. details: %s", it->first.c_str(), e.what());
            }
        }

    }
    /////////////////////////////////////////////////////////
    bool ValueStorage::readScript(const AnsiStr& strFP, const AnsiStr& section) {
        IniFile* script = new IniFile(strFP, IniFile::fmRead);
        
        
        if(this->size() == 0) {
            return false;
        }
        
        AnsiStr strValue;
        for(ValueStorage::ITER it = this->begin(); it != this->end(); it++) {
            
            try {
                strValue = script->readString(section, it->first.c_str());
                it->second.fromString(strValue.cptr());
                
            }
            catch(std::invalid_argument& e) {
                LogErrorArg2("unable to load value named %s. details: %s", it->first.c_str(), e.what());
            }
        }

        SAFE_DELETE(script);
        return true;
    }

    bool ValueStorage::writeScript(const AnsiStr& strFP, const AnsiStr& section) {
        IniFile* script = new IniFile(strFP, IniFile::fmReadWrite);
        
        
        for(ValueStorage::CONST_ITER it = this->cbegin(); it != this->cend(); it++) {
            try {
                script->writeString(section, it->first.c_str(), AnsiStr(it->second.toString().c_str()));
            }
            catch(std::invalid_argument& e) {
                LogErrorArg2("unable to store value named %s. details: %s", it->first.c_str(), e.what());
            }
        }
        
        SAFE_DELETE(script);
        return true;
    }
    

}
