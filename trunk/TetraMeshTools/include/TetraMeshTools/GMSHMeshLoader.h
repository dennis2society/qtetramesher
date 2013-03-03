/*
 * GMSHMeshLoader.h
 *
 *  Created on: Aug 21, 2011
 *      Author: Dennis Luebke
 */

#ifndef GMSHMESHLOADER_H_
#define GMSHMESHLOADER_H_

#include "BaseMeshLoader.h"

#include "TetraToolsExports.h"

namespace TetraTools
{

	class DLL_EXPORT GMSHMeshLoader : public BaseMeshLoader
	{
	protected:
		std::string _fileName;
		std::string	_fileContent;

		std::vector<Vec3f>			_vertices;
		std::vector<Tetrahedron>	_tetraIndices;

		virtual bool ParseFile(std::ifstream& file_);

		void ParseGMSH(std::ifstream &file, const unsigned int gmshFormat_);

	public:
		GMSHMeshLoader();

		GMSHMeshLoader(const std::string& fileName_);

		bool Load(const std::string& fileName_);

		const std::vector<Vec3f>& GetVertices() const;

		const std::vector<Tetrahedron>& GetTetras() const;

		bool IsLoaded();

		void Clear();
	};

} /// end namespace Mesh

#endif /* GMSHMESHLOADER_H_ */
