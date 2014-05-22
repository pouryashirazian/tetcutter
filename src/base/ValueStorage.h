//
//  ValueStorage.h
//  hifem
//
//  Created by pshiraz on 2/4/14.
//  Copyright (c) 2014 pshiraz. All rights reserved.
//

#ifndef hifem_ValueStorage_h
#define hifem_ValueStorage_h

#include "Value.h"
#include "String.h"
#include "FastAccessToNamedResource.h"


namespace PS {
    
    //Logging and NoDuplicates enabled by default
    typedef FastAccessNamedResource<Value, TypeValue, InsertRemoveNoop> ValueStorageParent;

    class ValueStorage : public ValueStorageParent {
    public:
        ValueStorage() {}
        virtual ~ValueStorage() {
            ValueStorageParent::cleanup();
        }
        
        void printAll();
        
        //IO
        virtual bool read(const AnsiStr& strFP);
        virtual bool write(const AnsiStr& strFP);
        
        virtual bool readScript(const AnsiStr& strFP, const AnsiStr& section = "GENERAL");
        virtual bool writeScript(const AnsiStr& strFP, const AnsiStr& section = "GENERAL");


        
    };
}

#endif
