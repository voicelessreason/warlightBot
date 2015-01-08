/*
 * RegionObject.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: Arlen
 */

#include "RegionObject.h"

RegionObject::RegionObject(int pID)
{
	visible = false;
	visited = false;
	owner = "neutral";
	armies = 2;
	ID = pID;
}

RegionObject::~RegionObject()
{

}

void RegionObject::updateRegionObject(bool visibility, string playerName, int numArmies)
{
	armies = numArmies;
	visible = visibility;
	owner = playerName;
}

void RegionObject::addNeighbor(RegionObject* nbr)
{
	neighbors.push_back(nbr);
}

void RegionObject::resetVisibility()
{
	visible = false;
}
void RegionObject::setVisited(bool vis)
{
	visited = vis;
}
bool RegionObject::getVisited()
{
	return visited;
}
vector<RegionObject*> RegionObject::get_neighbors()
{
	return neighbors;
}
int RegionObject::getID()
{
	return ID;
}
bool RegionObject::getVisibility()
{
	return visible;
}
string RegionObject::getOwner()
{
	return owner;
}
int RegionObject::getArmies()
{
	return armies;
}