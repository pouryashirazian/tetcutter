/*
 * SGHeaders.h
 *
 *  Created on: Mar 8, 2014
 *      Author: pourya
 */

#ifndef SGHEADERS_H_
#define SGHEADERS_H_


#include "base/String.h"
#include "base/FastAccessToNamedResource.h"
#include "SGNode.h"

using namespace PS;

namespace PS {
namespace SG {

class SGHeaders : public SGNode
{
public:
	SGHeaders();
	virtual ~SGHeaders();

	void cleanup();

	//Draw
	void draw();
	static void DrawText(const char* chrText, int x, int y);

    //Header Lines
    int addHeaderLine(const AnsiStr& title, const AnsiStr& strInfo);
    int getHeaderId(const char* title);
    bool removeHeaderLine(const char* title);
    void removeAllHeaders();
    bool updateHeaderLine(int id, const AnsiStr& strInfo);
    bool updateHeaderLine(const AnsiStr& title, const AnsiStr& strInfo);
    bool isValidID(bool id) {return (id >=0 && id < m_vHeaders.size());}

protected:
    //Info String
    std::vector<AnsiStr> m_vHeaders;
    FastAccessNamedResource<int, TypeValue, InsertRemoveNoop, Logging, false> m_hashHeaders;
};

}
}


#endif /* SGHEADERS_H_ */
