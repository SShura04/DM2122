#include "Navmesh.h"

Navmesh::Navmesh()
{
	StartNode = nullptr;
	TargetNode = nullptr;
	zerozeronode = nullptr;
	dimensionsx = 0;
	dimensionsz = 0;
	intervals = 0;
	xcount = 0;
	zcount = 0;
	startposX = 0;
	startposZ = 0;
	endposX = 0;
	endposZ = 0;
	pathfound = false;
	checksperframe = 30;
	isuniform = true;
	searching = false;
}

Navmesh::Navmesh(unsigned dimensionx, unsigned dimensionz, float interval, float startposx, float startposz)
{
	//generate the nav mesh
	isuniform = true;
	searching = false;
	checksperframe = 30;
	startposX = 0;
	startposZ = 0;
	endposX = 0;
	endposZ = 0;
	pathfound = false;
	intervals = interval;
	dimensionsx = dimensionx;
	dimensionsz = dimensionz;
	StartNode = nullptr;
	TargetNode = nullptr;
	xcount = dimensionx / interval;
	zcount = dimensionz / interval;
	zerozeronode = new NavmeshNode(false, Vector3(startposx, 0, startposz), 0, 0);
	//generate nodes
	NavmeshNode* marker, *marker2, *marker3;
	marker = zerozeronode;
	marker2 = zerozeronode;
	for (unsigned idx = 0; idx < xcount; idx++)
	{
		for (unsigned idx1 = 1; idx1 < zcount; idx1++)
		{
			marker3 = new NavmeshNode(false, Vector3(startposx + interval * idx, 0, startposz + interval * idx1), idx, idx1);
			marker->setuppointer(marker3);
			marker3->setdownpointer(marker);
			marker = marker3;

		}
		marker3 = new NavmeshNode(false, Vector3(startposx + interval * (idx + 1), 0, startposz), idx + 1, 0);
		marker2->setrightpointer(marker3);
		marker3->setleftpointer(marker2);
		marker2 = marker3;
		marker = marker3;
	}
	//link the navmesh together
	marker = zerozeronode;
	marker2 = zerozeronode->getrightpointer();
	marker3 = zerozeronode->getrightpointer();
	for (unsigned idx = 0; idx < xcount; idx++)
	{
		for (unsigned idx1 = 1; idx1 < zcount; idx1++)
		{
			marker = marker->getuppointer();
			marker2 = marker2->getuppointer();
			if (marker2 == nullptr)
			{
				break;
			}
			marker->setrightpointer(marker2);
			marker2->setleftpointer(marker);
		}
		if (marker2 == nullptr)
		{
			break;
		}
		marker = marker3;
		marker2 = marker->getrightpointer();
		marker3 = marker2;
	}
}

Navmesh::Navmesh(bool setisuniform)
{
	isuniform = setisuniform;
	StartNode = nullptr;
	TargetNode = nullptr;
	zerozeronode = nullptr;
	dimensionsx = 0;
	dimensionsz = 0;
	intervals = 0;
	xcount = 0;
	zcount = 0;
	startposX = 0;
	startposZ = 0;
	endposX = 0;
	endposZ = 0;
	pathfound = false;
	checksperframe = 30;
	isuniform = true;
	searching = false;
}

Navmesh::~Navmesh()
{
	NavmeshNode* marker;
	NavmeshNode* marker2;
	NavmeshNode* marker3;
	marker = zerozeronode;
	//move the marker to the other end
	while (marker->getrightpointer() != nullptr)
	{
		marker = marker->getrightpointer();
	}
	while (marker->getuppointer() != nullptr)
	{
		marker = marker->getrightpointer();
	}

	//delete nodes
	marker3 = marker->getdownpointer();
	while (true)
	{
		marker2 = marker->getleftpointer();
		if (marker2 == nullptr && marker3 == nullptr)
			break;
		if (marker2 == nullptr)
		{
			marker = marker3;
			marker2 = marker->getleftpointer();
			marker3 = marker->getdownpointer();
		}
		else
		{
			marker = marker2;
			marker2 = marker->getleftpointer();
		}
	}
}

void Navmesh::FindPathGrid()
{
	if (pathfound && !searching)
	{
		return;
	}
	//add start node to open node
	static NavmeshNode* current = nullptr; //if the program is not searching yet
	if (current == nullptr || opennodes.size() == 0)
	{
		opennodes.clear();
		opennodes.push_back(StartNode);
		closednodes.clear(); //clear the closednodes
		current = opennodes[0];
		if (isuniform)
		{
			current->sethcost((TargetNode->getgridx() - StartNode->getgridx()) + (TargetNode->getgridy() - StartNode->getgridy()));
			current->setgcost(0);
			current->setfcost(current->gethcost() + current->getgcost());
		}
	}
	//while loop that checks each node in open
	unsigned count = 0;
	while (opennodes.size() > 0)
	{
		count++;
		if (count > checksperframe)
		{
			return;
		}
		current = opennodes[0];
		//get the node with the lowest f cost, if f is same check h cost, if h same choose whichever is last checked
		for (unsigned idx = 0; idx < opennodes.size(); idx++)
		{
			if (opennodes[idx]->getfcost() < current->getfcost())
			{
				current = opennodes[idx];
			}
			else if (opennodes[idx]->getfcost() == current->getfcost())
			{
				if (opennodes[idx]->gethcost() < current->gethcost())
				{
					current = opennodes[idx];
				}
			}
		}
		
		//remove current node from the open list
		for (unsigned idx = 0; idx < opennodes.size(); idx++)
		{
			if (current == opennodes[idx])
			{
				std::vector<NavmeshNode*> ::iterator it = opennodes.begin();
				it += idx;
				opennodes.erase(it);
			}
		}

		closednodes.push_back(current); //add current to closed

		if (current == TargetNode)
		{
			NavmeshNode* marker = current;
			travelnodes.clear();
			while (marker != StartNode)
			{
				travelnodes.push_back(marker);
				marker = marker->getparentpointer();
			}
			current = nullptr;
			pathfound = true;
			return;
		}

		NavmeshNode* currentNeighbour;
		
		for (unsigned direction = 0; direction < 4; direction++)
		{
			if (direction == 0)
				currentNeighbour = current->getuppointer();
			else if (direction == 1)
				currentNeighbour = current->getdownpointer();
			else if (direction == 2)
				currentNeighbour = current->getleftpointer();
			else
				currentNeighbour = current->getrightpointer();

			if (currentNeighbour == nullptr)
				continue;
			bool isinsideclosednodes = false;
			for (unsigned idx = 0; idx < closednodes.size(); idx++)
			{
				if (currentNeighbour == closednodes[idx])
				{
					isinsideclosednodes = true;
					break;
				}
			}

			if (!(currentNeighbour == nullptr || currentNeighbour->getIsWall() || isinsideclosednodes))
			{
				//calculate the distance to the node from current
				float tempgcost = current->getgcost() + 1;
				
				//check if the neighbour is inside the openlist
				bool insideopenlist = false;
				for (unsigned idx = 0; idx < opennodes.size(); idx++)
				{
					if (opennodes[idx] == currentNeighbour)
					{
						insideopenlist = true;
					}
				}

				//check if new path to neighbour is shorter or neighbour is no in open
				if ((tempgcost < currentNeighbour->getgcost() || !insideopenlist) && !isinsideclosednodes)
				{
					//set the new fcost
					if (insideopenlist) //if it already exists only update the g and f cost as h is the same
					{
						currentNeighbour->setgcost(tempgcost);
						currentNeighbour->setfcost(currentNeighbour->gethcost() + tempgcost);
					}
					else //calculate all three cost values
					{
						currentNeighbour->setgcost(tempgcost);
						int temphcost = (TargetNode->getgridx() - currentNeighbour->getgridx()) + (TargetNode->getgridy() - currentNeighbour->getgridy());
						if (temphcost < 0)
						{
							temphcost = -temphcost;
						}
						currentNeighbour->sethcost(temphcost);
						currentNeighbour->setfcost(tempgcost + temphcost);
					}
					currentNeighbour->setparentnode(current);
					if (!insideopenlist)
						opennodes.push_back(currentNeighbour);
				}
			}
		}
	}
}

void Navmesh::FindPathGrid(std::vector<NavmeshNode*>* pclosednodes, std::vector<NavmeshNode*>* popennodes, std::vector<NavmeshNode*>* ptravelnodes, NavmeshNode* pStartNode, NavmeshNode* pTargetNode, NavmeshNode* pcurrent, bool *ppathfound, bool *psearching)
{
	if (*ppathfound && !*psearching)
	{
		return;
	}
	//add start node to open node
	pcurrent = nullptr; //if the program is not searching yet
	if (pcurrent == nullptr || popennodes->size() == 0)
	{
		popennodes->clear();
		popennodes->push_back(pStartNode);
		pclosednodes->clear(); //clear the closednodes
		pcurrent = (*popennodes)[0];
		pcurrent->sethcost((pTargetNode->getgridx() - pStartNode->getgridx()) + (pTargetNode->getgridy() - pStartNode->getgridy()));
		pcurrent->setgcost(0);
		pcurrent->setfcost(pcurrent->gethcost() + pcurrent->getgcost());
	}
	//while loop that checks each node in open
	unsigned count = 0;
	while (popennodes->size() > 0)
	{
		count++;
		if (count > checksperframe)
		{
			return;
		}
		pcurrent = (*popennodes)[0];
		//get the node with the lowest f cost, if f is same check h cost, if h same choose whichever is last checked
		for (unsigned idx = 0; idx < popennodes->size(); idx++)
		{
			if ((*popennodes)[idx]->getfcost() < pcurrent->getfcost())
			{
				pcurrent = (*popennodes)[idx];
			}
			else if ((*popennodes)[idx]->getfcost() == pcurrent->getfcost())
			{
				if ((*popennodes)[idx]->gethcost() < pcurrent->gethcost())
				{
					pcurrent = (*popennodes)[idx];
				}
			}
		}

		//remove current node from the open list
		for (unsigned idx = 0; idx < popennodes->size(); idx++)
		{
			if (pcurrent == (*popennodes)[idx])
			{
				std::vector<NavmeshNode*> ::iterator it = popennodes->begin();
				it += idx;
				popennodes->erase(it);
			}
		}

		pclosednodes->push_back(pcurrent); //add current to closed

		if (pcurrent == pTargetNode) //check if the algo has found the target node
		{
			NavmeshNode* marker = pcurrent;
			ptravelnodes->clear();
			while (marker != pStartNode)
			{
				for (unsigned dumbchecker = 0; dumbchecker < ptravelnodes->size(); dumbchecker++)
				{
					if (marker->getparentpointer() == (*ptravelnodes)[dumbchecker]) //check if the parent is already inside
					{
						return;
					}
				}
				ptravelnodes->push_back(marker);
				marker = marker->getparentpointer();
				//check if the stupid thing is going into a infinite while loop
			}
			pcurrent = nullptr;
			*ppathfound = true;
			return;
		}

		NavmeshNode* currentNeighbour;

		for (unsigned direction = 0; direction < 4; direction++)
		{
			if (direction == 0)
				currentNeighbour = pcurrent->getuppointer();
			else if (direction == 1)
				currentNeighbour = pcurrent->getdownpointer();
			else if (direction == 2)
				currentNeighbour = pcurrent->getleftpointer();
			else
				currentNeighbour = pcurrent->getrightpointer();

			if (currentNeighbour == nullptr)
				continue;
			bool isinsideclosednodes = false;
			for (unsigned idx = 0; idx < pclosednodes->size(); idx++)
			{
				if (currentNeighbour == (*pclosednodes)[idx])
				{
					isinsideclosednodes = true;
					break;
				}
			}

			if (!(currentNeighbour == nullptr || currentNeighbour->getIsWall() || isinsideclosednodes))
			{
				//calculate the distance to the node from current
				float tempgcost = pcurrent->getgcost() + 1;

				//check if the neighbour is inside the openlist
				bool insideopenlist = false;
				for (unsigned idx = 0; idx < popennodes->size(); idx++)
				{
					if ((*popennodes)[idx] == currentNeighbour)
					{
						insideopenlist = true;
					}
				}

				//check if new path to neighbour is shorter or neighbour is not in open
				if ((tempgcost < currentNeighbour->getgcost() || !insideopenlist) && !isinsideclosednodes)
				{
					//set the new fcost
					if (insideopenlist) //if it already exists only update the g and f cost as h is the same
					{
						currentNeighbour->setgcost(tempgcost);
						currentNeighbour->setfcost(currentNeighbour->gethcost() + tempgcost);
					}
					else //calculate all three cost values
					{
						currentNeighbour->setgcost(tempgcost);
						int temphcost = (pTargetNode->getgridx() - currentNeighbour->getgridx()) + (pTargetNode->getgridy() - currentNeighbour->getgridy());
						if (temphcost < 0)
						{
							temphcost = -temphcost;
						}
						currentNeighbour->sethcost(temphcost);
						currentNeighbour->setfcost(tempgcost + temphcost);
					}
					currentNeighbour->setparentnode(pcurrent);
					if (!insideopenlist)
						popennodes->push_back(currentNeighbour);
				}
			}
		}
	}
}

void Navmesh::FindPathCoord(float startposx, float startposz, float endposx, float endposz)
{
	//check if either the target position/startposition is outside of the map
	if (startposx < zerozeronode->getPosition().x || startposx > zerozeronode->getPosition().x + dimensionsx || startposz < zerozeronode->getPosition().z || startposz > zerozeronode->getPosition().z + dimensionsz || endposx < zerozeronode->getPosition().x || endposx > zerozeronode->getPosition().x + dimensionsx || endposz < zerozeronode->getPosition().z || endposz > zerozeronode->getPosition().z + dimensionsz)
	{
		return;
	}

	//check if the start and final position is the same
	bool updatestart = false, updateend = false;
	if (startposX != startposx || startposZ != startposz)
	{
		updatestart = true;
		startposX = startposx;
		startposZ = startposz;
	}
	else
		updatestart = false;
	if (endposX != endposx || endposZ != endposz)
	{
		updateend = true;
		endposX = endposx;
		endposZ = endposz;
	}
	else
		updateend = false;

	if (updateend || updatestart || StartNode == nullptr || TargetNode == nullptr)
	{
		//convert the startpositions to the grid number by finding the nearest coords
		NavmeshNode* marker;
		marker = zerozeronode;
		unsigned xmove, zmove;

		//get the start node pointer
		if (updatestart || StartNode == nullptr)
		{
			xmove = floor((startposX - zerozeronode->getPosition().x) / intervals + 0.5);
			zmove = floor((startposZ - zerozeronode->getPosition().z) / intervals + 0.5);
			for (unsigned idx = 0; idx < xmove; idx++)
			{
				marker = marker->getrightpointer();
			}
			for (unsigned idx = 0; idx < zmove; idx++)
			{
				marker = marker->getuppointer();
			}
			StartNode = marker;
		}

		//do the same for end node
		if (updateend || TargetNode == nullptr)
		{
			xmove = floor((endposX - zerozeronode->getPosition().x) / intervals + 0.5);
			zmove = floor((endposZ - zerozeronode->getPosition().z) / intervals + 0.5);
			marker = zerozeronode;
			for (unsigned idx = 0; idx < xmove; idx++)
			{
				marker = marker->getrightpointer();
			}
			for (unsigned idx = 0; idx < zmove; idx++)
			{
				marker = marker->getuppointer();
			}
			TargetNode = marker;
		}

		pathfound = false;
		searching = true;
	}

	FindPathGrid();
}

unsigned Navmesh::getdimensionx()
{
	return dimensionsx;
}
unsigned Navmesh::getdimensionz()
{
	return dimensionsz;
}
float Navmesh::getintervals()
{
	return intervals;
}
NavmeshNode* Navmesh::getzerozeronode()
{
	return zerozeronode;
}
std::vector<NavmeshNode*> Navmesh::gettravelnodes()
{
	return travelnodes;
}
bool Navmesh::getisuniform()
{
	return isuniform;
}
void Navmesh::setisuniform()
{
	isuniform = !isuniform;
}
unsigned Navmesh::getxcount()
{
	return xcount;
}
unsigned Navmesh::getzcount()
{
	return zcount;
}