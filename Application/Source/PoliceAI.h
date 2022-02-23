#ifndef POLICE_AI_H
#define POLICE_AI_H

#include "Vector3.h"
#include "Navmesh.h"
#include "Objects.h"
#include <cmath>
#include <vector>
#include <iostream>
class PoliceAI
{
private:
	Vector3 worldposition;
	unsigned fovangle, viewdistance;
	float policemovementspeed, policeangle;
	std::vector <NavmeshNode*>* movementnodes;
	Vector3 playerlastknownlocation;
	Vector3 viewdirection;

	//to accomodate pathfinding
	std::vector <NavmeshNode*>* closednodes;
	std::vector <NavmeshNode*>* opennodes;
	NavmeshNode* StartNode;
	NavmeshNode* TargetNode;
	NavmeshNode* current;
	bool canseeplayer;
	bool policepathfound, policesearching, policespawned;

public:
	static unsigned numberofobject;
	PoliceAI();
	PoliceAI(unsigned setfovangle, Vector3 startposition, unsigned viewdistance, unsigned hb_NPC1);
	~PoliceAI();

	void PoliceAIUpdate(Vector3 playerpos, Navmesh* navmesh, float dt, Objects objectlist[]);
	bool SpawnPolice(Vector3 spawnposition, float PoliceMovementSpeed, Vector3 viewdirection, Navmesh* navmesh, Vector3 playerpos);
	void DespawnPolice();
	bool GetSpawnStatus();
	float getangle();
	bool getcanseeplayer();
	void setposition(Vector3 newposition);
	void setpolicemovementspeed(float newspeed);
	std::vector <NavmeshNode*>* getmovementnodes();
	Vector3 GetPosition();
};

#endif