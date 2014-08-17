/*
 * CmdLineProcessor.h
 *
 *  Created on: Aug 16, 2014
 *      Author: pourya
 */

#ifndef CMDLINEPROCESSOR_H_
#define CMDLINEPROCESSOR_H_

#include <string>
#include "FastAccessToNamedResource.h"
#include "Value.h"
#include "String.h"

using namespace std;
namespace PS {

class CmdOption {
public:
	CmdOption(): toggle(false) {
	}

public:
	AnsiStr desc;
	AnsiStr shortcut;
	Value value;
	bool toggle;

	void operator=(const CmdOption& b) {
		desc = b.desc;
		shortcut = b.shortcut;
		value = b.value;
	}
};

//Logging and NoDuplicates enabled by default
typedef FastAccessNamedResource<CmdOption, TypeValue, InsertRemoveNoop> CmdOptionStorage;

/*!
 * Command line parser
 */
class CmdLineParser : public CmdOptionStorage {
public:
	CmdLineParser();
	virtual ~CmdLineParser();

	void printHelp() const;
	bool add_option(const AnsiStr& name, const AnsiStr& desc, const Value& defval);
	int parse(int argc, char* argv[]);

	template <typename value_type>
	value_type value(const char* name) {
		return this->get(name).value.get<value_type>();
	}


	static bool isTokenName(const AnsiStr& str);
protected:
	std::map<string, string> m_shortcuts;
};
}




#endif /* CMDLINEPROCESSOR_H_ */
