#include "PoliceAI.h"


PoliceAI::PoliceAI()
{
	worldposition = Vector3(0, -20, 0);
	playerlastknownlocation = Vector3(0, 2, 0);
	StartNode = nullptr;
	TargetNode = nullptr;
	current = nullptr;
	policepathfound = false;
	policesearching = false;
	closednodes = new std::vector <NavmeshNode*>;
	opennodes = new std::vector <NavmeshNode*>;
	movementnodes = new std::vector <NavmeshNode*>;
	fovangle = 0;
	viewdistance = 0;
	canseeplayer = false;
}
PoliceAI::PoliceAI(unsigned setfovangle, Vector3 startposition, unsigned setviewdistance, unsigned hb_NPC1)
{
	fovangle = setfovangle;
	worldposition = startposition;
	viewdistance = setviewdistance;
	StartNode = nullptr;
	TargetNode = nullptr;
	current = nullptr;
	policepathfound = false;
	policesearching = false;
	closednodes = new std::vector <NavmeshNode*>;
	opennodes = new std::vector <NavmeshNode*>;
	movementnodes = new std::vector <NavmeshNode*>;
	canseeplayer = false;
	numberofobject = hb_NPC1;
}
PoliceAI::~PoliceAI()
{
	delete closednodes;
	delete opennodes;
	delete movementnodes;
}
unsigned PoliceAI::numberofobject;

void PoliceAI::PoliceAIUpdate(Vector3 playerpos, Navmesh* navmesh, float dt, Objects objectlist[])
{
	//check line of sight with the player
	if (!policespawned)
	{
		return;
	}
	Vector3 policeplayervector = worldposition - playerpos;
	Vector3 policeplayervector2 = (worldposition - playerpos).Normalize();
	Vector3 viewXZ = -Vector3(viewdirection.x, 0, viewdirection.z);


	canseeplayer = false;
	float player2policeangle = atan2(policeplayervector2.z * viewXZ.x - policeplayervector2.x * viewXZ.z, policeplayervector2.x * viewXZ.x + policeplayervector2.z * viewXZ.z) * (180 / Math::PI);
	if (player2policeangle < 0) //convert player2policeangle from negative to positive
	{
		player2policeangle = -player2policeangle;
	}
	//std::cout << "check\n";
	if (player2policeangle <= fovangle)
	{
		if (policeplayervector.Length() < viewdistance)
		{
			Vector3 wallcheck = (worldposition - playerpos).Normalize();
			float length1 = wallcheck.Length(), length2 = policeplayervector.Length();
			unsigned times = 1;
			Vector3 testvector3;
			//check for walls
			while (true)
			{
				bool done = false;
				testvector3 = wallcheck * times;
				//get inworldposition
				Vector3 Pos = testvector3 + playerpos;
				Hitbox testhitbox;
				testhitbox.sethitboxcenterdimensions(Vector3(0.5, 0.5, 0.5), Pos);
				for (unsigned idx = 0; idx < numberofobject; idx++)
				{
					if (objectlist[idx].gethitbox().Checkforcollision(testhitbox))
					{
						canseeplayer = false;
						done = true;
						break;
					}
					else if (testhitbox.Checkforcollision(objectlist[idx].gethitbox()))
					{
						canseeplayer = false;
						done = true;
						break;
					}
					if (idx == numberofobject - 1)
					{
						canseeplayer = true;
						break;
					}
				}
				if (done)
					break;
				if (testvector3.Length() > length2)
					break;
				times++;
			}
		}
	}
	static bool updatedlastknownpos = false;

	//if true recalculate the movement path
	if (!canseeplayer)
	{
		static unsigned num = 1;
		if (movementnodes->size() > 0)
		{
			if (num > movementnodes->size())
			{
				//make the ai wander to the player's position
				playerlastknownlocation = playerpos;
				policepathfound = false;
				policesearching = true;
				updatedlastknownpos = true;
				num = 1;
				(*movementnodes).clear();
			}
			else
			{
				Vector3 movedirection = (*movementnodes)[movementnodes->size() - num]->getPosition() - worldposition;
				if (movedirection.Length() < 0.4)
				{
					num++;
				}
				movedirection.Normalize();
				viewdirection = movedirection;
				movedirection = movedirection * policemovementspeed * dt;
				worldposition = worldposition + movedirection;
			}
		}
		if (movementnodes->size() == 0 || updatedlastknownpos)
		{
			//convert the startpositions to the grid number by finding the nearest coords
			NavmeshNode* marker;
			marker = navmesh->getzerozeronode();
			unsigned xmove, zmove;

			//get the start node pointer
			xmove = round((worldposition.x - navmesh->getzerozeronode()->getPosition().x) / navmesh->getintervals());
			zmove = floor(((worldposition.z - navmesh->getzerozeronode()->getPosition().z) / navmesh->getintervals()) + 0.5);
			marker = navmesh->getzerozeronode();
			for (unsigned idx = 0; idx < xmove; idx++)
			{
				marker = marker->getrightpointer();
			}
			for (unsigned idx = 0; idx < zmove; idx++)
			{
				marker = marker->getuppointer();
			}
			StartNode = marker;

			//do the same for end node
			marker;
			marker = navmesh->getzerozeronode();
			xmove = floor(((playerlastknownlocation.x - navmesh->getzerozeronode()->getPosition().x) / navmesh->getintervals()) + 0.5);
			zmove = floor(((playerlastknownlocation.z - navmesh->getzerozeronode()->getPosition().z) / navmesh->getintervals()) + 0.5);
			marker = navmesh->getzerozeronode();
			for (unsigned idx = 0; idx < xmove; idx++)
			{
				marker = marker->getrightpointer();
			}
			for (unsigned idx = 0; idx < zmove; idx++)
			{
				marker = marker->getuppointer();
			}
			opennodes->clear();
			closednodes->clear();
			current = nullptr;
			TargetNode = marker;
			policepathfound = false;
			policesearching = true;
			updatedlastknownpos = false;
			num = 1;
		}
		navmesh->FindPathGrid(closednodes, opennodes, movementnodes, StartNode, TargetNode, current, &policepathfound, &policesearching);
	}
	else if (canseeplayer)
	{
		policepathfound = false;
		policesearching = false;
		Vector3 movedirectiontest;
		movedirectiontest = playerpos - worldposition;
		movedirectiontest.y = 0;
		movedirectiontest.Normalize();
		viewdirection = movedirectiontest;
		movedirectiontest = movedirectiontest * policemovementspeed * dt;
		worldposition = worldposition + movedirectiontest;
		updatedlastknownpos = true;
		movementnodes->clear();

		//update last known location
		if (playerlastknownlocation.x != playerpos.x || playerlastknownlocation.z != playerpos.z)
		{
			playerlastknownlocation = playerpos;
		}
	}
	policeplayervector2 = viewdirection.Normalize();
	player2policeangle = atan2(policeplayervector2.z * viewXZ.x - policeplayervector2.x * viewXZ.z, policeplayervector2.x * viewXZ.x + policeplayervector2.z * viewXZ.z) * (180 / Math::PI);
	player2policeangle -= 180;
	policeangle += player2policeangle;
}

bool PoliceAI::SpawnPolice(Vector3 spawnposition, float PoliceMovementSpeed, Vector3 viewdirection, Navmesh* navmesh, Vector3 playerpos)
{
	policespawned = true;
	worldposition = spawnposition;
	//set the player position to last known location
	NavmeshNode* marker;
	marker = navmesh->getzerozeronode();
	playerlastknownlocation = playerpos;
	unsigned xmove, zmove;
	xmove = floor(((playerlastknownlocation.x - navmesh->getzerozeronode()->getPosition().x) / navmesh->getintervals()) + 0.5);
	zmove = floor(((playerlastknownlocation.z - navmesh->getzerozeronode()->getPosition().z) / navmesh->getintervals()) + 0.5);
	marker = navmesh->getzerozeronode();
	for (unsigned idx = 0; idx < xmove; idx++)
	{
		marker = marker->getrightpointer();
	}
	for (unsigned idx = 0; idx < zmove; idx++)
	{
		marker = marker->getuppointer();
	}
	TargetNode = marker;

	xmove = floor(((worldposition.x - navmesh->getzerozeronode()->getPosition().x) / navmesh->getintervals()) + 0.5);
	zmove = floor(((worldposition.z - navmesh->getzerozeronode()->getPosition().z) / navmesh->getintervals()) + 0.5);
	marker = navmesh->getzerozeronode();
	for (unsigned idx = 0; idx < xmove; idx++)
	{
		marker = marker->getrightpointer();
	}
	for (unsigned idx = 0; idx < zmove; idx++)
	{
		marker = marker->getuppointer();
	}
	StartNode = marker;
	opennodes->clear();
	closednodes->clear();
	current = nullptr;
	policemovementspeed = PoliceMovementSpeed;
	this->viewdirection = viewdirection;

	Vector3 policeplayervector2 = (worldposition - playerpos).Normalize();
	Vector3 viewXZ = -Vector3(viewdirection.x, 0, viewdirection.z);
	policeplayervector2 = viewdirection.Normalize();
	float player2policeangle = atan2(policeplayervector2.z * viewXZ.x - policeplayervector2.x * viewXZ.z, policeplayervector2.x * viewXZ.x + policeplayervector2.z * viewXZ.z) * (180 / Math::PI);
	player2policeangle -= 180;
	policeangle -= player2policeangle;
	policeangle += 90;
	//do checking for whether the choosen spot is spawnable or not
	return false;
}

void PoliceAI::DespawnPolice()
{
	policespawned = false;
	policeangle = 0;
	worldposition = Vector3(100, -2, 1);
	opennodes->clear();
	closednodes->clear();
	current = nullptr;
	movementnodes->clear();
}

bool PoliceAI::GetSpawnStatus()
{
	return policespawned;
}

float PoliceAI::getangle()
{
	return policeangle;
}

bool PoliceAI::getcanseeplayer()
{
	return canseeplayer;
}

void PoliceAI::setposition(Vector3 newposition)
{
	worldposition = newposition;
}

void PoliceAI::setpolicemovementspeed(float newspeed)
{
	policemovementspeed = newspeed;
}

std::vector<NavmeshNode*>* PoliceAI::getmovementnodes()
{
	return movementnodes;
}

Vector3 PoliceAI::GetPosition()
{
	return worldposition;
}
