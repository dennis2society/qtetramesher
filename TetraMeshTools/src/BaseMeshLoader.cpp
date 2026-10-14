/*
 * BaseMeshLoader.cpp
 *
 *  Created on: Aug 20, 2011
 *      Author: Dennis Luebke
 */

#include "TetraMeshTools/BaseMeshLoader.h"
#include <fstream>

bool TetraTools::BaseMeshLoader::OpenFile(const std::string &fileName_,
                                          std::ifstream &file_) {
  std::cout << "BaseMeshLoader: Read file \"" << fileName_ << "\"" << std::endl;
  file_.open(fileName_.c_str());
  if (!file_.is_open()) {
    std::cerr << "Could not open file " << fileName_ << std::endl;
    return false;
  }
  return true;
}
