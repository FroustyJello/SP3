#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Vertex.h"
#include "SingletonTemplate.h"
#include <map>
#include <string>

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/

// Forward Declaration
class Mesh;

class MeshBuilder : public Singleton<MeshBuilder>
{
	friend Singleton<MeshBuilder>;
public:
	Mesh* GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);
	Mesh* GenerateQuad(const std::string &meshName, Color color, float length = 1.f);
	Mesh* GenerateCube(const std::string &meshName, Color color, float length = 1.f);
	Mesh* GenerateRing(const std::string &meshName, Color color, unsigned numSlice, float outerR = 1.f, float innerR = 0.f);
	Mesh* GenerateSphere(const std::string &meshName, Color color, unsigned numStack, unsigned numSlice, float radius = 1.f);
	Mesh* GenerateCone(const std::string &meshName, Color color, unsigned numSlice, float radius, float height);
	Mesh* GenerateOBJ(const std::string &meshName, const std::string &file_path);
	Mesh* GenerateText(const std::string &meshName, unsigned row, unsigned col);

	Mesh* GetMesh(const std::string& _meshName);
	void AddMesh(const std::string& _meshName, Mesh* _newMesh);
	void RemoveMesh(const std::string& _meshName);
	void cleanlist();

private:
	std::map<std::string, Mesh*> meshMap;
};

#endif