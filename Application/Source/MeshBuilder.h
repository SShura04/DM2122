#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "Vertex.h"

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);
	static Mesh* GenerateQuad(const std::string& meshName, Color color, float length, unsigned repeatnum = 1);
	static Mesh* GenerateCube(const std::string &meshName, Color color, float length = 1.f);
	static Mesh* GenerateCircle(const std::string& meshName, Color color, unsigned numSlices, float radius);
	static Mesh* GenerateSphere(const std::string& meshName, Color color, unsigned numStacks, unsigned numSlices, float radius);
	static Mesh* GenerateCylinder(const std::string& meshName, Color color, unsigned numSlices = 10, float radius = 1.f, float height = 1.f);
	static Mesh* GenerateCone(const std::string& meshName, Color color, unsigned numSlices = 10, float radius = 1.f, float height = 1.f);
	static Mesh* GenerateOBJ(const std::string& meshName, const std::string &file_path);
	static Mesh* GenerateOBJMTL(const std::string& meshName, const std::string& file_path, const std::string& mtl_path);
	static Mesh* GenerateText(const std::string& meshName, unsigned numRow, unsigned numCol);

	static Mesh* GenerateHemiSphere(const std::string& meshName, Color color, unsigned numStacks = 10, unsigned numSlices = 10, float radius = 1.f);
	static Mesh* GeneratePyramid(const std::string& meshName, Color color, float length = 1.f);
	static Mesh* GenerateTorus(const std::string& meshName, Color color, unsigned numStack, unsigned numSlice, float outerR, float innerR);
	static Mesh* GenerateEyeLid(const std::string& meshName, Color color, unsigned numStacks = 10, unsigned numSlices = 10, float radius = 1.f); //Hemisphere without bottom
	static Mesh* GenerateHalfTorus(const std::string& meshName, Color color, unsigned numStack, unsigned numSlice, float outerR, float innerR);
};

#endif