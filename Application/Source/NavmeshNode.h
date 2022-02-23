#pragma once
#include "Vector3.h"
#include "Hitbox.h"
class NavmeshNode
{
private:
	float gridx, gridy; //store the position of the node in the world
	bool IsWall; //stores whether this node is accessible or not
	Vector3 Position; //stores in world position
	unsigned gcost, hcost, fcost; //stores the relevant cost of the node. g stores the distance from the start node, h stores the distance from the target node, f stores the sum of g and h
	NavmeshNode* upnode;
	NavmeshNode* downnode;
	NavmeshNode* leftnode;
	NavmeshNode* rightnode;
	NavmeshNode* parentnode;
	Hitbox nodehitbox;
public:
	NavmeshNode();
	NavmeshNode(bool iswall, Vector3 pos, float gridX, float gridY);

	float getgridx();
	float getgridy();
	void setgridx(float newgridx);
	void setgridy(float newgridy);
	bool getIsWall();
	void setIsWall(bool newIsWall);
	Vector3 getPosition();
	void setPosition(Vector3 newpos);
	Hitbox gethitbox();

	unsigned getgcost();
	unsigned gethcost();
	unsigned getfcost();

	void setgcost(unsigned newcost);
	void sethcost(unsigned newcost);
	void setfcost(unsigned newcost);

	void setuppointer(NavmeshNode* newpointer);
	void setdownpointer(NavmeshNode* newpointer);
	void setleftpointer(NavmeshNode* newpointer);
	void setrightpointer(NavmeshNode* newpointer);
	void setparentnode(NavmeshNode* newpointer);

	NavmeshNode* getuppointer();
	NavmeshNode* getdownpointer();
	NavmeshNode* getleftpointer();
	NavmeshNode* getrightpointer();
	NavmeshNode* getparentpointer();
};

