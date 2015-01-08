/*
 * RegionObject.h
 *
 *  Created on: Dec 4, 2014
 *      Author: Arlen
 */

#include <string>
#include <vector>

using namespace std;

class RegionObject {
	bool visible;
	bool visited;
	string owner;
	int armies;
	int ID;
	vector<RegionObject*> neighbors;

public:
	RegionObject(int pID);
	virtual ~RegionObject();
	void updateRegionObject(bool visibility, string playerName, int numArmies);
	void addNeighbor(RegionObject* nbr);
	void resetVisibility();
	void setVisited(bool vis);
	vector<RegionObject*> get_neighbors();
	bool getVisibility();
	bool getVisited();
	string getOwner();
	int getID();
	int getArmies();
};


