#ifndef FASTACCESSTONAMEDRESOURCE_H
#define FASTACCESSTONAMEDRESOURCE_H

#include <utility>
#include <string>
#include <map>
#include <unordered_map>
//#include <hash_map.h>
#include "MathBase.h"

//using namespace __gnu_cxx;
using namespace std;

namespace PS {

//Logging Policies
struct NoLogging {
    static void LogArg1(const char* message, const char* arg1) {
        PS_UNUSED(message);
        PS_UNUSED(arg1);
    }
};

struct Logging {
    static void LogArg1(const char* message, const char* arg1);
};

//ResourceType Policies
template <typename T>
struct TypeValue {
    typedef T Resource;
};

template <typename T>
struct TypePointer {
    typedef T* Resource;
};

//ResouceCreator Policy
template <typename T>
struct InsertRemoveNoop {
    static void Insert(T element) {
        PS_UNUSED(element);
    }

    static void Remove(T element) {
        PS_UNUSED(element);
    }
};


template <typename T>
struct NoopInsertRemoveBySafeDelete {
    static void Insert(T element) {
        PS_UNUSED(element);
    }

    static void Remove(T element) {
        SAFE_DELETE(element);
    }
};


//Provides named access to resources
template <typename T,
          template <class> class PolicyResourceType = TypePointer,
          template <class> class PolicyResourceInsertRemove = NoopInsertRemoveBySafeDelete,
          class PolicyErrorLogging = Logging,
          bool allowDuplicates = false>
class FastAccessNamedResource
{

public:
    FastAccessNamedResource() { }

    virtual ~FastAccessNamedResource() {
        cleanup();
    }

    //Convenient types
    typedef typename PolicyResourceType<T>::Resource Resource;
    typedef typename std::unordered_map<string, Resource >::iterator ITER;
    typedef typename std::unordered_map<string, Resource >::const_iterator CONST_ITER;

    /*!
     * \brief has checks weather a named resource is present in the collection
     * \param name the title of the resource
     * \return true if the item is found in the collection
     */
    bool has(const char* name) const {
        return (m_hash.find(string(name)) != m_hash.end());
    }

    /*!
     * \brief get
     * \param name
     * \return
     */
    Resource get(const char* name) const {
        CONST_ITER it = m_hash.find(string(name));
        if(it != m_hash.end())
            return it->second;
        else {
            PolicyErrorLogging::LogArg1("Requested resource not found! name = %s", name);
            return (0);
        }
    }

    ITER at(const char* name) {
    	return m_hash.find(string(name));
    }


    bool set(const char* name, Resource element) {
        ITER it = m_hash.find(string(name));
        if(it != m_hash.end())
            it->second = element;
        else
            PolicyErrorLogging::LogArg1("Requested resource not found! name = %s", name);
        return (it != m_hash.end());
    }

    /*!
     * \brief add will add an element into the hashmap
     * \param element
     * \param name
     */
    bool add(Resource element, const char* name) {
        
        if(has(name)) {
            PolicyErrorLogging::LogArg1("Another resource with the same name is stored! Policy: %s", allowDuplicates? "ALLOWED" : "NOT ALLOWED");
            if(!allowDuplicates)
                return false;
        }
        PolicyResourceInsertRemove<Resource>::Insert(element);
        m_hash.insert(std::make_pair(name, element));
        return true;
    }

    bool remove(const char* name) {
    	ITER it = m_hash.find(string(name));
    	if(it != m_hash.end()) {
    		PolicyResourceInsertRemove<Resource>::Remove(it->second);
    		m_hash.erase(it);
    		return true;
    	}

    	return false;
    }

    //Clear the list
    void cleanup() {
        for(ITER it = m_hash.begin(); it != m_hash.end(); it++) {
            PolicyResourceInsertRemove<Resource>::Remove(it->second);
        }
        m_hash.clear();
    }
    
    U32 size() const {return m_hash.size();}
    CONST_ITER cbegin() const {return m_hash.cbegin();}
    CONST_ITER cend() const {return m_hash.cend();}

    ITER begin() {return m_hash.begin();}
    ITER end()  {return m_hash.end();}

protected:
     std::unordered_map<string, Resource > m_hash;
};

}

#endif // FASTACCESSTONAMEDRESOURCE_H
