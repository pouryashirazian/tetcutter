/*
 * SettingsDB.h
 *
 *  Created on: Jan 29, 2014
 *      Author: pourya
 */

#ifndef SETTINGSDB_H_
#define SETTINGSDB_H_

#include "String.h"
#include "base/Vec.h"
#include "FastAccessToNamedResource.h"
#include "loki/Singleton.h"
#include "ValueStorage.h"

using namespace PS::MATH;
using namespace Loki;

namespace PS {

    
    //Value Storage
	class SettingsDB : public ValueStorage {
    public:
        SettingsDB();
        virtual ~SettingsDB();

        void printHelp();
        int processCmdLine(int argc, char* argv[]);
	};

    typedef SingletonHolder<SettingsDB, CreateUsingNew, PhoenixSingleton> Settings;

}


#endif /* SETTINGSDB_H_ */
