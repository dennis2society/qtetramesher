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
  /// Opens a file for reading. Returns false if the file cannot be opened.
  /// The caller owns the stream.
  bool OpenFile(const std::string &fileName_, std::ifstream &file_);

  virtual bool ParseFile(std::ifstream &file_) = 0;
};

} // namespace TetraTools

#endif /* BASEMESHLOADER_H_ */
