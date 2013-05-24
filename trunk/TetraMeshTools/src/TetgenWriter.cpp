/*
 * TetgenWriter.cpp
 *
 *  Created on: May 19, 2013
 *      Author: Dennis Luebke
 */
 
#include "TetraMeshTools/TetgenWriter.h"
#include <iostream>
#include <fstream>

bool TetraTools::TetgenWriter::SaveToFile(const std::string& fileName_, const std::vector<Vec3f>& verts, const std::vector<Tetrahedron>& tetras)
{
	std::ofstream nodeFile;
	std::string nodeFilename = fileName_ + std::string(".node");
	nodeFile.open(nodeFilename.c_str());
	if(nodeFile.is_open()) 
	{
		nodeFile<<"# Node count, 3 dim, no attribute, no boundary marker"<<std::endl;
		nodeFile<<verts.size()<<" 3 0 0"<<std::endl;
		nodeFile<<"# Node index, node coordinates"<<std::endl;
		for (unsigned int i=0; i<verts.size(); ++i)
		{
			const Vec3f& v = verts[i];
			nodeFile<<(i+1)<<" "<<v.x<<" "<<v.y<<" "<<v.z<<std::endl;
		}
		nodeFile<<"# End of nodes..."<<std::endl;
		nodeFile.close();
		std::ofstream eleFile;
		std::string eleFilename = fileName_ + std::string(".ele");
		eleFile.open(eleFilename.c_str());
		if (eleFile.is_open())
		{
			eleFile<<"# Number of tetrahedra, number of indices per line, 0"<<std::endl;
			eleFile<<tetras.size()<<" 4 0"<<std::endl;
			eleFile<<"# Tetra index, node indices"<<std::endl;
			for (unsigned int i=0; i<tetras.size(); ++i)
			{
				const Tetrahedron& t = tetras[i];
				eleFile<<(i+1)<<" "<<(t.index[0]+1)<<" "<<(t.index[1]+1)<<" "<<(t.index[2]+1)<<" "<<(t.index[3]+1)<<std::endl;
			}
			eleFile<<"# End of tetras..."<<std::endl;
			eleFile.close();
		}
		else
		{
			return false;
		}
		return true;
		/**
		outputFile<<tetras.size()<<std::endl;
		for (unsigned int i=0; i<tetras.size(); ++i)
		{
			const Tetrahedron& t = tetras[i];
			outputFile<<(i+1)<<" 4 1 1 4 "<<(t.index[0]+1)<<" "<<(t.index[1]+1)<<" "<<(t.index[2]+1)<<" "<<(t.index[3]+1)<<std::endl;
		}
		outputFile<<"$ENDELM"<<std::endl;
		outputFile.close();
		*/
	}
	else
	{
		return false;
	}
	return false;
}