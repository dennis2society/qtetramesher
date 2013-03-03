/*
 * GMSHMeshLoader.cpp
 *
 *  Created on: Aug 21, 2011
 *      Author: Dennis Luebke
 */

#include "TetraMeshTools/GMSHMeshLoader.h"
#include <stdlib.h>
#include <algorithm>


TetraTools::GMSHMeshLoader::GMSHMeshLoader()
{

}

TetraTools::GMSHMeshLoader::GMSHMeshLoader(const std::string& fileName_)
{
	Clear();
	std::cout<<"Loading GMSH file \""<<fileName_<<"\"."<<std::endl;
	Load(_fileName);
}

void TetraTools::GMSHMeshLoader::Clear()
{
	_vertices.clear();
	_tetraIndices.clear();
	_fileName.clear();
	_fileContent.clear();
}

bool TetraTools::GMSHMeshLoader::ParseFile(std::ifstream& file_)
{
	std::string cmd;
	unsigned int gmshFormat = 0;
	std::getline(file_, cmd); //Version
	std::istringstream versionReader(cmd);
	std::string version;
	versionReader >> version;
	std::string line;
	if (version == "$MeshFormat") // Reading gmsh 2.0 file
	{
		gmshFormat = 2;
		std::getline(file_, line); // we don't care about this line (2 0 8)
		std::getline(file_, cmd); // end Version
		std::istringstream endMeshReader(cmd);
		std::string endMesh;
		endMeshReader >> endMesh;

		if (endMesh != std::string("$EndMeshFormat") ) // it should end with $EndMeshFormat
		{
			std::cerr << "Closing File as it doesn't end with $EndMeshFormat" << std::endl;
			file_.close();
			return false;
		}
		else
		{
			std::getline(file_, cmd); // First Command
		}
	}
	else
	{
		//sout << "Gmsh format 1.0" << sendl;
		gmshFormat = 1;
	}

	std::istringstream nodeReader(cmd);
	std::string node;
	nodeReader >> node;
	// -- Reading file
	if (node == "$NOD" || node == "$Nodes") // Gmsh format
	{
		ParseGMSH(file_, gmshFormat);
	}
	if (file_.is_open())
		file_.close();
	std::sort(_tetraIndices.begin(), _tetraIndices.end());
	return true;
}

void TetraTools::GMSHMeshLoader::ParseGMSH(std::ifstream &file, const unsigned int gmshFormat_)
{
	std::string cmd;
	std::string line;

	unsigned int npoints = 0;
	unsigned int nelems = 0;

	// --- Loading Verti ces ---
	file >> npoints; //nb points

	std::vector<unsigned int> pmap; // map for reordering vertices possibly not well sorted
	for (unsigned int i=0; i<npoints; ++i)
	{
		unsigned int index = i;
		float x,y,z;
		file >> index >> x >> y >> z;
		_vertices.push_back(Vec3f(x, y, z));
		if (pmap.size() <= index)
			pmap.resize(index+1);

		pmap[index] = i; // In case of hole or switch
	}
	file >> cmd;
	if (cmd != "$ENDNOD" && cmd != "$EndNodes")
	{
		std::cerr << "Error: MeshGmshLoader: '$ENDNOD' or '$EndNodes' expected, found '" << cmd << "'" << std::endl;
		file.close();
		return;
	}

	// --- Loading Elements ---
	file >> cmd;
	if (cmd != "$ELM" && cmd != "$Elements")
	{
		std::cerr << "Error: MeshGmshLoader: '$ELM' or '$Elements' expected, found '" << cmd << "'" << std::endl;
		file.close();
		return;
	}

	file >> nelems; //Loading number of Element

	for (unsigned int i=0; i<nelems; ++i) // for each elem
	{
		int index, etype, rphys, relem, nnodes, ntags, tag;
		if (gmshFormat_ == 1)
		{
			// version 1.0 format is
			// elm-number elm-type reg-phys reg-elem number-of-nodes <node-number-list ...>
			file >> index >> etype >> rphys >> relem >> nnodes;
		}
		else if (gmshFormat_ == 2)
		{
			// version 2.0 format is
			// elm-number elm-type number-of-tags < tag > ... node-number-list
			file >> index >> etype >> ntags;
			for (int t=0; t<ntags; t++)
			{
				file >> tag;
			}
			switch (etype)
			{
			case 15: //point
				nnodes = 1;
				break;
			case 1: // Line
				nnodes = 2;
				break;
			case 2: // Triangle
				nnodes = 3;
				break;
			case 3: // Quad
				nnodes = 4;
				break;
			case 4: // Tetra
				nnodes = 4;
				break;
			case 5: // Hexa
				nnodes = 8;
				break;
			default:
				std::cerr << "Error: MeshGmshLoader: Elements of type 1, 2, 3, 4, 5, or 6 expected. Element of type " << etype << " found." << std::endl;
				nnodes = 0;
				break;
			}
		}
		//store real index of node and not line index
	      std::vector<unsigned int> nodes;
	      nodes.resize (nnodes);

	      for (int n=0; n<nnodes; ++n)
	      {
	    	  int t = 0;
	    	  file >> t;
	    	  nodes[n] = (((unsigned int)t)<pmap.size())?pmap[t]:0;
	      }
	      //helper::fixed_array <unsigned int,8> hexa;
	      switch (etype)
	      {
	      case 1: // Line
//	    	  addEdge(&my_edges, helper::fixed_array <unsigned int,2>(nodes[0], nodes[1]));
//	    	  ++nlines;
	    	  break;
	      case 2: // Triangle
//	    	  addTriangle(&my_triangles, helper::fixed_array <unsigned int,3>(nodes[0], nodes[1], nodes[2]));
//	    	  ++ntris;
	    	  break;
	      case 3: // Quad
//	    	  addQuad(&my_quads, helper::fixed_array <unsigned int,4>(nodes[0], nodes[1], nodes[2], nodes[3]));
//	    	  ++nquads;
	    	  break;
	      case 4: // Tetra
	    	  //addTetrahedron(&my_tetrahedra, helper::fixed_array <unsigned int,4>(nodes[0], nodes[1], nodes[2], nodes[3]));
	    	  //++ntetrahedra;
	    	  Tetrahedron t;
	    	  t.index[0] = nodes[0];
	    	  t.index[1] = nodes[1];
	    	  t.index[2] = nodes[2];
	    	  t.index[3] = nodes[3];
	    	  _tetraIndices.push_back(t);
	    	  break;
	      case 5: // Hexa
//	    	  for (unsigned int n=0; n<8; n++)
//	    		  hexa[n] = nodes[n];
//	    	  addHexahedron(&my_hexahedra,hexa);
//	    	  ++ncubes;
	    	  break;
		  default:
			  //if the type is not handled, skip rest of the line
			  std::string tmp;
			  std::getline(file, tmp);
			  break;
	      }
	}

	file >> cmd;
	if (cmd != "$ENDELM" && cmd!="$EndElements")
	{
		std::cerr << "Error: MeshGmshLoader: '$ENDELM' or '$EndElements' expected, found '" << cmd << "'" << std::endl;
		file.close();
		return;
	}
}

bool TetraTools::GMSHMeshLoader::Load(const std::string& fileName_)
{
	Clear();
	_fileName = fileName_;
	return ParseFile(ReadFile(_fileName));
}

const std::vector<Vec3f>& TetraTools::GMSHMeshLoader::GetVertices() const
{
	return _vertices;
}

const std::vector<Tetrahedron>& TetraTools::GMSHMeshLoader::GetTetras() const
{
	return _tetraIndices;
}

bool TetraTools::GMSHMeshLoader::IsLoaded()
{
	return false;
}


