/*
 * TetgenLoader.h
 *
 *  Created on: May 19, 2013
 *      Author: Dennis Luebke
 */

#ifndef TETGENLOADER_H_
#define TETGENLOADER_H_

#include "BaseMeshLoader.h"

#include "TetraToolsExports.h"

namespace TetraTools
{

	class DLL_EXPORT TetgenLoader : public BaseMeshLoader
	{
	protected:
		std::string _fileName;

		std::vector<Vec3f>			_vertices;
		std::vector<Tetrahedron>	_tetraIndices;

		virtual bool ParseFile(std::ifstream& file_);

		void ParseTetgen(std::ifstream &file_);

		void ParseNodes();

		void ParseElems();

	public:
		TetgenLoader();

		TetgenLoader(const std::string& fileName_);

		bool Load(const std::string& fileName_);

		const std::vector<Vec3f>& GetVertices() const;

		const std::vector<Tetrahedron>& GetTetras() const;

		bool IsLoaded();

		void Clear();
	};

} /// end namespace Mesh

#endif /* GMSHMESHLOADER_H_ */
