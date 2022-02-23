#pragma once
#include <vector>
#include <iostream>
#include "NavmeshNode.h"
class Navmesh
{
private:
	std::vector <NavmeshNode*> closednodes; //stores nodes that cannot be travelled on
	std::vector <NavmeshNode*> travelnodes; //stores the nodes chosen to be travelled on
	std::vector <NavmeshNode*> opennodes;
	NavmeshNode* StartNode;
	NavmeshNode* TargetNode;
	NavmeshNode* zerozeronode; //placed at the start
	float startposX, startposZ, endposX, endposZ, intervals;
	unsigned dimensionsx, dimensionsz, xcount, zcount;
	bool pathfound, searching;
	unsigned checksperframe;
	bool isuniform; //if the type is a uniform grid or not
public:
	Navmesh();
	Navmesh(unsigned dimensionx, unsigned dimensionz, float interval, float startposx, float startposz);
	Navmesh(bool setisuniform);
	~Navmesh();
	void FindPathGrid();
	void FindPathGrid(std::vector <NavmeshNode*> *pclosednodes, std::vector <NavmeshNode*> *popennodes, std::vector <NavmeshNode*> *ptravelnodes, NavmeshNode* pStartNode, NavmeshNode* pTargetNode, NavmeshNode* pcurrent, bool *ppathfound, bool *psearching);
	void FindPathCoord(float startposX, float startposz, float endposx, float endposz);
	unsigned getdimensionx();
	unsigned getdimensionz();
	float getintervals();
	unsigned getxcount();
	unsigned getzcount();
	NavmeshNode* getzerozeronode();
	std::vector <NavmeshNode*> gettravelnodes();
	bool getisuniform();
	void setisuniform();
};

