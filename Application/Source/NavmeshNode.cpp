#include "NavmeshNode.h"

NavmeshNode::NavmeshNode()
{
	gridx = 0; gridy = 0;
	IsWall = false;
	Position = Vector3(0, 0, 0);
	gcost = 0;
	hcost = 0;
	fcost = 0;
	upnode = nullptr;
	downnode = nullptr;
	leftnode = nullptr;
	downnode = nullptr;
	parentnode = nullptr;
}

NavmeshNode::NavmeshNode(bool iswall, Vector3 pos, float gridX, float gridY)
{
	gridx = gridX; gridy = gridY;
	IsWall = iswall;
	Position = pos;
	parentnode = nullptr;
	nodehitbox.sethitboxcenterdimensions(Vector3(1, 1, 1), pos);
}

float NavmeshNode::getgridx()
{
	return gridx;
}

float NavmeshNode::getgridy()
{
	return gridy;
}
void NavmeshNode::setgridx(float newgridx)
{
	gridx = newgridx;
}
void NavmeshNode::setgridy(float newgridy)
{
	gridy = newgridy;
}
bool NavmeshNode::getIsWall()
{
	return IsWall;
}
void NavmeshNode::setIsWall(bool newIsWall)
{
	IsWall = newIsWall;
}
Vector3 NavmeshNode::getPosition()
{
	return Position;
}
void NavmeshNode::setPosition(Vector3 newpos)
{
	Position = newpos;
}

Hitbox NavmeshNode::gethitbox()
{
	return nodehitbox;
}

unsigned NavmeshNode::getgcost()
{
	return gcost;
}
unsigned NavmeshNode::gethcost()
{
	return hcost;
}
unsigned NavmeshNode::getfcost()
{
	return fcost;
}

void NavmeshNode::setgcost(unsigned newcost)
{
	gcost = newcost;
}
void NavmeshNode::sethcost(unsigned newcost)
{
	hcost = newcost;
}
void NavmeshNode::setfcost(unsigned newcost)
{
	fcost = newcost;
}

void NavmeshNode::setuppointer(NavmeshNode* newpointer)
{
	upnode = newpointer;
}
void NavmeshNode::setdownpointer(NavmeshNode* newpointer)
{
	downnode= newpointer;
}
void NavmeshNode::setleftpointer(NavmeshNode* newpointer)
{
	leftnode = newpointer;
}
void NavmeshNode::setrightpointer(NavmeshNode* newpointer)
{
	rightnode = newpointer;
}

void NavmeshNode::setparentnode(NavmeshNode* newpointer)
{
	parentnode = newpointer;
}

NavmeshNode* NavmeshNode::getuppointer()
{
	return upnode;
}
NavmeshNode* NavmeshNode::getdownpointer()
{
	return downnode;
}
NavmeshNode* NavmeshNode::getleftpointer()
{
	return leftnode;
}
NavmeshNode* NavmeshNode::getrightpointer()
{
	return rightnode;
}

NavmeshNode* NavmeshNode::getparentpointer()
{
	return parentnode;
}
