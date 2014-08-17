/*
 * CmdLineProcessor.cpp
 *
 *  Created on: Aug 16, 2014
 *      Author: pourya
 */
#include "CmdLineParser.h"
#include "base/Logger.h"

using namespace PS;

CmdLineParser::CmdLineParser() {

}

CmdLineParser::~CmdLineParser() {
	//m_shortcuts.clear();
}

void CmdLineParser::printHelp() const {
	printf("\n");
	printf("The following options are available:\n");
	for(CONST_ITER it = this->cbegin(); it != this->cend(); it++) {
		printf("-%s, -%s\t\t%s\n", it->first.c_str(), it->second.shortcut.c_str(), it->second.desc.c_str());
	}
}

bool CmdLineParser::isTokenName(const AnsiStr& str) {
	return (str.firstChar() == '-');
}

bool CmdLineParser::add_option(const AnsiStr& name, const AnsiStr& desc, const Value& defval) {

	if(name.length() == 0)
		return false;

	CmdOption cmdOp;
	cmdOp.desc = desc;
	cmdOp.value = defval;
	cmdOp.shortcut = "";

	//register shortcut if not present
	for(int i=0; i < name.length() - 1; i++) {
		AnsiStr shortcut = name.substr(i, 1);
		if(m_shortcuts.find(shortcut.cptr()) == m_shortcuts.end()) {
			cmdOp.shortcut = shortcut;
			m_shortcuts.insert( std::make_pair(shortcut.cptr(), name.cptr()));
			break;
		}
	}

	//if there is no shortcut
	if(cmdOp.shortcut.length() == 0)
		LogWarningArg1("No shortcut registered for option: %s", name.cptr());

	return this->add(cmdOp, name.cptr());
}

int CmdLineParser::parse(int argc, char* argv[]) {

	int i = 0;
	while(i < argc) {
		AnsiStr strName = AnsiStr(argv[i]).trim();
		if(strName.firstChar() == '-') {
			strName = strName.substr(1);

			//resolve shortcut
			if (strName.length() == 1) {
				string temp = string(strName.c_str());
				if (m_shortcuts.find(temp) != m_shortcuts.end())
					strName = AnsiStr(m_shortcuts[temp].c_str());
			}


			if (!this->has(strName.cptr())) {
				printHelp();
				return -1;
			}

			//get the option
			CmdOption cmdOp = this->get(strName.cptr());
			i++;

			//toggle does not require value
			if (!cmdOp.toggle) {

				if (i >= argc) {
					printHelp();
					return -2;
				}

				AnsiStr nextToken = AnsiStr(argv[i]).trim();
				if (nextToken.firstChar() == '-') {
					LogErrorArg1("Expected value for %s!", strName.cptr());
					printHelp();
					return -2;
				}

				//value
				string strVal = string(nextToken.cptr());
				cmdOp.value.fromString(strVal);

				//set variable
				printf("Setting variable: %s to %s\n", strName.cptr(),
						strVal.c_str());

				//update
				this->set(strName.cptr(), cmdOp);
			}
		}


		//incr
		i++;
	}

	return i;
}



