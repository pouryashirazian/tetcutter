/*
 * SGUITools.h
 *
 *  Created on: Jul 12, 2014
 *      Author: pourya
 */

#ifndef SGUITOOLS_H_
#define SGUITOOLS_H_

#include "base/String.h"
#include "base/FastAccessToNamedResource.h"
#include "SGNode.h"

namespace PS {
namespace SG {

class SGUITools : public SGNode {
public:

protected:
    //Info String
    std::vector<AnsiStr> m_vHeaders;
    FastAccessNamedResource<int, TypeValue, InsertRemoveNoop, Logging, false> m_hashUI;
};

}
}




#endif /* SGUITOOLS_H_ */
