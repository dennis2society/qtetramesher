/*
 * BaseMeshLoader.h
 *
 *  Created on: Aug 20, 2011
 *      Author: Dennis Luebke
 *
 * Base class for mesh loading from files.
 *
 */

#ifndef BASEMESHLOADER_H_
#define BASEMESHLOADER_H_

#include "GeometryTypes.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "TetraToolsExports.h"

namespace TetraTools {
class BaseMeshLoader {
protected:
  std::ifstream &ReadFile(const std::string &fileName_);

  virtual bool ParseFile(std::ifstream &file_) = 0;
};

} // namespace TetraTools

#endif /* BASEMESHLOADER_H_ */
