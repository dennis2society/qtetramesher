/*
 * GMSHMeshWriter.cpp
 *
 *  Created on: Aug 21, 2011
 *      Author: Dennis Luebke
 */
 
#include "TetraMeshTools/GMSHMeshWriter.h"

bool TetraTools::GMSHMeshWriter::SaveToFile(const std::string& fileName_, const std::vector<Vec3f>& verts, const std::vector<Tetrahedron>& tetras)
{
	std::ofstream outputFile;
	outputFile.open(fileName_.c_str());
	if(outputFile.is_open()) 
	{
		outputFile<<"$NOD"<<std::endl;
		outputFile<<verts.size()<<std::endl;
		for (unsigned int i=0; i<verts.size(); ++i)
		{
			const Vec3f& v = verts[i];
			outputFile<<(i+1)<<" "<<v.x<<" "<<v.y<<" "<<v.z<<std::endl;
		}
		outputFile<<"$ENDNOD"<<std::endl;
		outputFile<<"$ELM"<<std::endl;
		outputFile<<tetras.size()<<std::endl;
		for (unsigned int i=0; i<tetras.size(); ++i)
		{
			const Tetrahedron& t = tetras[i];
			outputFile<<(i+1)<<" 4 1 1 4 "<<(t.index[0]+1)<<" "<<(t.index[1]+1)<<" "<<(t.index[2]+1)<<" "<<(t.index[3]+1)<<std::endl;
		}
		outputFile<<"$ENDELM"<<std::endl;
		outputFile.close();
		return true;
	}
	else
	{
		return false;
	}
	return false;
}