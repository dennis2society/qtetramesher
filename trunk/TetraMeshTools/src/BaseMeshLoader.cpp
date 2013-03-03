/*
 * BaseMeshLoader.cpp
 *
 *  Created on: Aug 20, 2011
 *      Author: Dennis Luebke
 */

#include <fstream>
#include "TetraMeshTools/BaseMeshLoader.h"

std::ifstream& TetraTools::BaseMeshLoader::ReadFile(const std::string& fileName_)
{
	std::cout<<"BaseMeshLoader: Read file \""<<fileName_<<"\""<<std::endl;
	std::ifstream* myfile = new std::ifstream(fileName_.c_str());
	if (myfile->is_open())
		return *myfile;
	else
	{
		std::cout<<"Could not open file "<<fileName_<<std::endl;
		const std::string defaultFileName = "data\\cow.obj";
		std::ifstream* myfile = new std::ifstream(defaultFileName.c_str());
		return *myfile;
	}
}
