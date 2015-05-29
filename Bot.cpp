#include <iostream>
#include <sstream>
#include <tuple>
#include <string>
#include <stdlib.h>
#include <map>
#include <queue>
#include "Bot.h"

Bot::Bot()
{
   // printf("ctor\n");
}

Bot::~Bot()
{
     //printf("dtor\n");
    //dtor
}

void Bot::playGame()
{
	parser.initParser(this);
	parser.parseInput();
}    //plays a single game of Warlight

void Bot::makeMoves()
{

}   //makes moves for a single turn
void Bot::endTurn()
{

}     //indicates to the engine that it has made its move

void Bot::addRegion(unsigned noRegion, unsigned noSuperRegion)
{
    while (  regions.size() <= noRegion)
        {
            regions.push_back(Region());
        }
    regions[noRegion]  = Region(noRegion, noSuperRegion);
    superRegions[noSuperRegion].addRegion(noRegion);
}
void Bot::addNeighbors(unsigned noRegion, unsigned neighbors)
{
    regions[noRegion].addNeighbors(neighbors) ;
    regions[neighbors].addNeighbors(noRegion) ;
}

void Bot::addSuperRegion(unsigned noSuperRegion, int reward)
{
	while (  superRegions.size() <=  noSuperRegion)
        {
            superRegions.push_back(SuperRegion());
        }
	superRegions[noSuperRegion]  = SuperRegion(reward);
}

void Bot::setBotName(string name)
{
    botName = name;
}
void Bot::setOpponentBotName(string name)
{
    opponentBotName = name;
}

void Bot::setArmiesLeft(int nbArmies)
{
    armiesLeft = nbArmies;
}

void Bot::addStartingRegion(unsigned noRegion)
{
     startingRegionsreceived.push_back(noRegion);
}

void Bot::startDelay(int delay)
{

}
void Bot::setPhase(string pPhase)
{
    phase=pPhase;
}
void Bot::updateRegion(unsigned noRegion, string playerName, int nbArmies)
{
    regions[noRegion].setArmies(nbArmies);
    regions[noRegion].setOwner(playerName);
    if (playerName == botName)
        ownedRegions.push_back(noRegion);

    //this code was added by me to update my mapGraph structure every turn.
    //visibility is reset at the end of execute action, to ensure that 
    //the only regions marked as visible are ones that the engine has told the bot about
    mapGraph.at(noRegion).updateRegionObject(true, playerName, nbArmies);
}
void Bot::addArmies(unsigned noRegion,int nbArmies)
{
    regions[noRegion].setArmies(regions[noRegion].getArmies() + nbArmies);
}
void Bot::moveArmies(unsigned noRegion,unsigned toRegion,int nbArmies)
{
    if (regions[noRegion].getOwner() == regions[toRegion].getOwner()
        && regions[noRegion].getArmies() > nbArmies)
    {
        regions[noRegion].setArmies(regions[noRegion].getArmies() - nbArmies);
        regions[toRegion].setArmies(regions[toRegion].getArmies() + nbArmies);

    }
    else if (regions[noRegion].getArmies() > nbArmies)
    {
        regions[noRegion].setArmies(regions[noRegion].getArmies() - nbArmies);
    }
}
void Bot::resetRegionsOwned()
{
    ownedRegions.clear();
}

// Initializes the graph of regionObjects 
// through which bot understands the game board
void Bot::initMapGraph()
{	
	for(int i = 0; i < regions.size(); i++)
	{
		mapGraph.push_back(RegionObject(i));
	}
	//for mapGraph at index i...
	for(int i = 0; i < regions.size(); i++)
	{
		//...loop through all neighbors of that index...
		for(int j = 0; j < regions[i].getNeighbors().size(); j++)
		{
			//...and add each neighbor to the neighbors vector of that index...
			mapGraph.at(i).addNeighbor(&mapGraph.at(regions[i].getNeighbors().at(j)));
		}
		
	}
}

// Function to reset the visibility of all regionObjects to nonvisible
// This simplifies the process of updating the graph by only having to 
// update regions explicitly mentioned by the engine at time of update.
void Bot::resetMapGraphVisibility()
{
	for(int i = 0; i < mapGraph.size(); i++)
	{
		mapGraph.at(i).resetVisibility();
	}
}
// Function to reset the visited property of all regionObjects to false
void Bot::resetMapGraphVisits()
{
	for(int i = 0; i < mapGraph.size(); i++)
	{
		mapGraph.at(i).setVisited(false);
	}
}

// Function that prints the state of the board.  
// Mainly for testing and debugging purposes.
void Bot::printMapGraph()
{
	cerr << "Start of round:\n";
	for(int i = 0; i < mapGraph.size(); i++)
	{
		if(mapGraph.at(i).getVisibility())
		{
			cerr << "Region " << i << " is owned by " << mapGraph.at(i).getOwner() << "\n";
		}
	}
	cerr<< "Round over.\n";
}

// Function that outputs moves to the engine when requested.
void Bot::executeAction()
{
    if (phase=="")
        return;
    if (phase == "pickPreferredRegion")
    {	
	round = 0;
    	initMapGraph();
	createStartRegString();
	cout << startRegString;
    }
    if (phase == "place_armies")
    {
	//increment round counter
	round++;
	cerr << "Round: " << round << "\n";

	//call function to build this round's move
	buildMove();
	
	//output reinforcements created by buildMove()
	cout << reinString;

	//check to see if any armies are left
	if(armiesLeft != 0)
	{
		//see if reinforcement string is being extended or started
		if(reinString != "")
		{
			cout << ", ";
		}

		//if there are enemy territories bordering my territories, drop excess reinforcements there.
		for(int i = 0; i < ownedRegions.size(); i++)
		{
			if(getEnemies(ownedRegions.at(i)).size() > 0 && armiesLeft > 0)
			{	
				cout << botName << " place_armies " << ownedRegions.at(i) << " " << armiesLeft << "\n";
				setArmiesLeft(0);
			}
		}
		//failing the above condition, drop troops on territory bordering neutral
		if(armiesLeft > 0)
		{
			for(int i = 0; i < ownedRegions.size(); i++)
			{
				if(getNeutrals(ownedRegions.at(i)).size() > 0 && armiesLeft > 0)
				{	
					cout << botName << " place_armies " << ownedRegions.at(i) << " " << armiesLeft << "\n";
					setArmiesLeft(0);
				}
			}	
		}
	}
	//close out reinforcement stage
	else
	{
		cout << "\n";
	}

    }
    if (phase == "attack/transfer")
    {

	if(atkString != "")
	{
		cout << atkString;

		if(tranString != "")
		{
			cout << ", " << tranString << "\n";
		}
		else
			cout << "\n";
	}
	else
	{
		if(tranString != "")
		{
			cout << tranString << "\n";
		}
		else
			cout << "No moves\n" ;
	}
    }
	resetMapGraphVisibility();
	phase.clear();
}

// Function to find best move for idle troops
// Pretty much a slightly altered BFS over the mapGraph structure
// Returns -1 if there is no enemy to move towards
int Bot::moveToEnemy(int start)
{
	resetMapGraphVisits();
	int ret = -1;
	queue<int> frontier;
	map<int,int> parents;

	parents.insert(pair<int,int>(start, -1));
	mapGraph.at(start).setVisited(true);
	frontier.push(start);
	
	while(!frontier.empty())
	{

		//set cur to equal first element in queue, then remove it
		int cur = frontier.front();
		frontier.pop();

		//check to see if current region is goal region
		if(mapGraph.at(cur).getOwner() == opponentBotName)
		{
			//find first move in sequence that took us to enemy territory
			while(parents[cur] != start)
			{
				cur = parents[cur];
			}
			ret = cur;
		}
		else
		{	
			//loop through neighbors of current region
			for(int i = 0; i < mapGraph.at(cur).get_neighbors().size(); i++)
			{
				//check to ensure that region is both visible and not already visited.
				if((*mapGraph.at(cur).get_neighbors().at(i)).getVisibility() 
						&& !(*mapGraph.at(cur).get_neighbors().at(i)).getVisited())
				{
					//add the neighbor in question to the frontier
					frontier.push((*mapGraph.at(cur).get_neighbors().at(i)).getID());

					//set the neighbor in question to visited
					(*mapGraph.at(cur).get_neighbors().at(i)).setVisited(true);

					//set cur to be the parent of the neighbor in question
					parents.insert(pair<int,int>((*mapGraph.at(cur).get_neighbors().at(i)).getID(), cur));
				}
			}
		}
	}
	return ret;
}
// Function to find best move for idle troops (if no enemy territory exists)
// Pretty much a slightly altered BFS over the mapGraph structure
// Returns -1 if there is no neutral to move towards (usually not gonna happen)
int Bot::moveToNeutral(int start)
{
	resetMapGraphVisits();
	int ret = -1;
	queue<int> frontier;
	map<int,int> parents;

	parents.insert(pair<int,int>(start, -1));
	mapGraph.at(start).setVisited(true);
	frontier.push(start);
	
	while(!frontier.empty())
	{

		//set cur to equal first element in queue, then remove it
		int cur = frontier.front();
		frontier.pop();

		//check to see if current region is goal region
		if(mapGraph.at(cur).getOwner() == "neutral")
		{
			//find first move in sequence that took us to enemy territory
			while(parents[cur] != start)
			{
				cur = parents[cur];
			}
			ret = cur;
		}
		else
		{	
			//loop through neighbors of current region
			for(int i = 0; i < mapGraph.at(cur).get_neighbors().size(); i++)
			{
				//check to ensure that region is both visible and not already visited.
				if((*mapGraph.at(cur).get_neighbors().at(i)).getVisibility() 
						&& !(*mapGraph.at(cur).get_neighbors().at(i)).getVisited())
				{
					//add the neighbor in question to the frontier
					frontier.push((*mapGraph.at(cur).get_neighbors().at(i)).getID());

					//set the neighbor in question to visited
					(*mapGraph.at(cur).get_neighbors().at(i)).setVisited(true);

					//set cur to be the parent of the neighbor in question
					parents.insert(pair<int,int>((*mapGraph.at(cur).get_neighbors().at(i)).getID(), cur));
				}
			}
		}
	}
	return ret;
}

//finds territories that have idle troops, finds the best place to move them and outputs a transfer string
void Bot::createTransferString()
{
	vector<int> idleTerr;	
	int toRegion;
	int fromRegion;
	int transferArmies;
	string ret = "";

	//finding all the territories that have extra troops and do not border an enemy or neutral
	for(int i = 0; i < mapGraph.size(); i++)
	{	
		
		if(mapGraph.at(i).getOwner() == botName && mapGraph.at(i).getArmies() > 1 && !getEnemies(mapGraph.at(i).getID()).size() > 0 && !getNeutrals(mapGraph.at(i).getID()).size() > 0)
		{
			idleTerr.push_back(mapGraph.at(i).getID());	
		}
	}

	for(int i = 0; i < idleTerr.size(); i++)
	{
		fromRegion = idleTerr.at(i);
		toRegion = moveToEnemy(fromRegion);
		transferArmies = mapGraph.at(fromRegion).getArmies() - 1;
		
		if(toRegion == -1)
		{
			toRegion = moveToNeutral(fromRegion);
		}
		if(toRegion != -1)
		{
			if(ret != "")
			{
				ret += ", ";
			}

			ret += botName + " attack/transfer " + static_cast<ostringstream*>( &(ostringstream() << fromRegion) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << toRegion) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << transferArmies ) )->str();
		}

	}
	tranString = ret;
}


void Bot::createStartRegString()
{
        unsigned i, nbAns=0;
	vector<int> startRegions;
	string ret = "";

        for (i = 0; i< startingRegionsreceived.size() && nbAns<6; i++)
        {
		//attempt to take regions in smallest superRegions
		if(regions[startingRegionsreceived[i]].getSuperRegion() == 2 || regions[startingRegionsreceived[i]].getSuperRegion() == 6)
		{	
			ret += static_cast<ostringstream*>( &(ostringstream() << startingRegionsreceived[i]) )->str();
			startRegions.push_back(startingRegionsreceived[i]);
			nbAns++;
			if (nbAns < 6)
				ret += " ";
		}//attempt to take regions in second smallest superRegion

		else if(regions[startingRegionsreceived[i]].getSuperRegion() == 4)
		{
			ret += static_cast<ostringstream*>( &(ostringstream() << startingRegionsreceived[i]) )->str();
			startRegions.push_back(startingRegionsreceived[i]);
			nbAns++;
			if (nbAns < 6)
				ret += " ";
		}
		if(nbAns > 5)
		{
			ret += "\n";
			break;
		}
	}
	startReg = startRegions;
	startRegString = ret;
}

//Returns int vector containing IDs of neighbors of a given region that are controlled by enemies
std::vector<int>& Bot::getEnemies(int noRegion)
{
	std::vector<int> ret;
	for(int i = 0; i < regions[noRegion].getNeighbors().size(); i++)
	{
		if(regions[regions[noRegion].getNeighbors().at(i)].getOwner() == opponentBotName)
			ret.push_back(noRegion);
	}
	return ret;
}


//Returns int vector containing IDs of neighbor regions controlled by no one
std::vector<int>& Bot::getNeutrals(int noRegion)
{
	std::vector<int> ret;
	for(int i = 0; i < regions[noRegion].getNeighbors().size(); i++)
	{
		if(regions[regions[noRegion].getNeighbors().at(i)].getOwner() == "neutral")
			ret.push_back(noRegion);
	}
	return ret;
}

//find superRegion which will be easiest to take
int Bot::findEasiestSuper()
{
	vector<int> membersOwned;
	int ret = 0;	

	//Initialize vector to 0's for each superRegion
	for(int i = 0; i < superRegions.size(); i++)
	{
		membersOwned.push_back(0);
	}

	//for each territory, increment the index of its superRegion
	for(int i = 0; i < ownedRegions.size(); i++)
	{
		int sRegion = regions[ownedRegions[i]].getSuperRegion();
		int oldValue = membersOwned.at(sRegion);
		membersOwned.at(sRegion) = oldValue + 1;
	}
	
	//find which superRegion contains the most owned regions
	for(int i = 0; i < membersOwned.size(); i++)
	{
		if(membersOwned.at(i) > ret && membersOwned.at(i) != superRegions[i].getNbRegions())
			ret = i;
	}
	
	return ret;
}


//return whether a victory is likely in a specific combat
//returns: 0 indicates a likely loss, 1 a likely victory
int Bot::isWinnable(int atkArmies, int defArmies)
{	
	int ret;
	
	while(atkArmies > 0 && defArmies > 0)
	{
		atkArmies -= atkArmies * .7;
		defArmies -= defArmies * .6;
	}
	//if atkArmies are depleted first, return 0 to rep likely loss.
	if(atkArmies == 0 && defArmies != 0)
		ret = 0;
	//if atkArmies are depleted simultaneously to defArmies, return 0 to rep likely loss.
	if(atkArmies == 0 && defArmies == 0)
		ret = 0;
	//if defArmies are depleted first, return 1 to rep likely victory.
	if(atkArmies != 0 && defArmies == 0)
		ret = 1;

	return ret;
}

///////////////////////////////////////////////////////////
//return ranked lists of desirable attack targets........//
//trying to decide how tiers should work...should regions//
//owned by the enemy and regions that are part of a      //
//continent be valued equally? does this depend more on  //
//what part of the game we're in? Currently:             //
//1 = owned by enemy + part of easiest super region	 //
//2 = owned by enemy 					 //
//3 = part of easiest super region			 //				
//4 = any territory that is not owned by me		 //
///////////////////////////////////////////////////////////
void Bot::updateTargets()
{
	vector<int> tar1;
	vector<int> tar2;
	vector<int> tar3;
	vector<int> tar4;

	int easySuper = findEasiestSuper();
	
	for(int i = 0; i < regions.size(); i ++)
	{
		if(regions[i].getOwner() == opponentBotName)
		{
			if(regions[i].getSuperRegion() == easySuper)
				tar1.push_back(i);
			else
				tar2.push_back(i);
		}
		else if(regions[i].getSuperRegion() == easySuper && regions[i].getOwner() == "neutral")
		{
			tar3.push_back(i);
		}
		else if(regions[i].getOwner() == "neutral")
		{
			tar4.push_back(i);
		}
	}
	targets1 = tar1;
	targets2 = tar2;
	targets3 = tar3;
	targets4 = tar4;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//finding feasibility of taking a single target territory				   //
//takes region ID of target region, number of reinforcements available			   //
//returns tuple containing feasibility, atkRegion, and number of reinforcements necessary  //
/////////////////////////////////////////////////////////////////////////////////////////////
tuple<int, int, int> Bot::findFeasibility(int noRegion, int numRein, int priFlag)		   
{
	tuple<int, int, int> ret = std::make_tuple(0,0,0);
	int atkRegion;
	int successFlag;
	int reinUsed = 0;

	//find best atk terr,
	if(priFlag == 1)
	{
		atkRegion = findBestAtkTerrHiPri(noRegion);
	}
	else
		atkRegion = findBestAtkTerrLoPri(noRegion);

	//check feasibility in current situation
	successFlag = isWinnable(regions[atkRegion].getArmies() - 1, regions[noRegion].getArmies());

		//if already winning, return 1 for feasible, 0 for reinforcements necessary
		if(successFlag == 1)
		{
			ret = std::make_tuple(1,atkRegion,0);
		}
		//else, check with incrementing amount of available reinforcements. 
		else
		{	
			while(reinUsed < numRein && successFlag != 1)
			{
				reinUsed++;
				successFlag = isWinnable(regions[atkRegion].getArmies() - 1 + reinUsed, regions[noRegion].getArmies());		
			}

			//if successful in any of these situations, return 1 for feasible, the atkTerr, and # of reinforcements
			if(successFlag == 1)
				ret = std::make_tuple(1, atkRegion, reinUsed);

			//else return 0 for infeasible, 0 for reinforcements
			else
				ret = std::make_tuple(0,atkRegion,0);
		}
	return ret;
}

//helper function for find feasibility...determines the best territory to attack a given target
int Bot::findBestAtkTerrHiPri(int noRegion)
{
	vector<int> posAtkTerr;
	int bestAtkTerr = -1;

	for(int i = 0; i < ownedRegions.size(); i++)
	{
		int temp = ownedRegions.at(i);
		for(int j = 0; j < regions[temp].getNeighbors().size(); j++)
		{
			if(regions[temp].getNeighbors().at(j) == noRegion)
				posAtkTerr.push_back(temp);
		}
	}
	
	if(!posAtkTerr.empty())
	{
		bestAtkTerr = posAtkTerr.at(0);
		for(int i = 1; i < posAtkTerr.size(); i++)
		{
			if(regions[posAtkTerr.at(i)].getArmies() > regions[bestAtkTerr].getArmies())
				bestAtkTerr = posAtkTerr.at(i);
		}

	}
	return bestAtkTerr;
}
//helper function for find feasibility...determines the best territory to attack a given target
int Bot::findBestAtkTerrLoPri(int noRegion)
{
	vector<int> posAtkTerr;
	int bestAtkTerr = -1;

	for(int i = 0; i < ownedRegions.size(); i++)
	{
		int temp = ownedRegions.at(i);
		for(int j = 0; j < regions[temp].getNeighbors().size(); j++)
		{
			if(regions[temp].getNeighbors().at(j) == noRegion && !(getEnemies(temp).size() > 0))
				posAtkTerr.push_back(temp);
		}
	}
	
	if(!posAtkTerr.empty())
	{
		bestAtkTerr = posAtkTerr.at(0);
		for(int i = 1; i < posAtkTerr.size(); i++)
		{
			if(regions[posAtkTerr.at(i)].getArmies() > regions[bestAtkTerr].getArmies())
				bestAtkTerr = posAtkTerr.at(i);
		}

	}
	return bestAtkTerr;
}


// Constructs the strings that represent reinforcement placement
// as well as attack moves. Calls createTransferString() to dictate other transfers.
void Bot::buildMove()
{
	string atkStr = "";
	string reinStr = "";
	tuple<int,int,int> result;
	
	updateTargets();
	
	if(!targets1.empty())
	{
		for(int i = 0; i < targets1.size(); i++)
		{
			result = findFeasibility(targets1.at(i), armiesLeft, 1);
			if(get<0>(result) == 1)
			{	
			
				if(atkStr != "")
				{
					atkStr = atkStr + ", ";
				}

				int atkArmies = (regions[get<1>(result)].getArmies() - 1 + get<2>(result));
				atkStr += botName + " attack/transfer " + static_cast<ostringstream*>( &(ostringstream() << get<1>(result)) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << targets1.at(i)) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << atkArmies ) )->str();
				if(get<2>(result) != 0)
				{
					if(reinStr != "")
					{
						reinStr = reinStr + ", ";
					}
					setArmiesLeft(armiesLeft - get<2>(result));
					reinStr += botName + " place_armies " + static_cast<ostringstream*>( &(ostringstream() << std::get<1>(result)) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << get<2>(result)) )->str();
				}

			}
		}
	}
	if(!targets2.empty())
	{
		for(int i = 0; i < targets2.size(); i++)
		{
			result = findFeasibility(targets2.at(i), armiesLeft, 1);
			if(get<0>(result) == 1 && get<1>(result) != -1)
			{	
			
				if(atkStr != "")
				{
					atkStr = atkStr + ", ";
				}

				int atkArmies = (regions[(get<1>(result))].getArmies() - 1 + (get<2>(result)));
				atkStr += botName + " attack/transfer " + static_cast<ostringstream*>( &(ostringstream() << get<1>(result)) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << targets2.at(i)) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << atkArmies ) )->str();
				if(get<2>(result) != 0)
				{
					if(reinStr != "")
					{
						reinStr = reinStr + ", ";
					}
					setArmiesLeft(armiesLeft - get<2>(result));
					reinStr += botName + " place_armies " + static_cast<ostringstream*>( &(ostringstream() << std::get<1>(result)) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << get<2>(result)) )->str();
				}
			}
		}
	}
	if(!targets3.empty())
	{
		for(int i = 0; i < targets3.size(); i++)
		{
			result = findFeasibility(targets3.at(i), armiesLeft, 0);
			if(get<0>(result) == 1 && get<1>(result) != -1)
			{	
			
				if(atkStr != "")
				{
					atkStr = atkStr + ", ";
				}

				int atkArmies = (regions[get<1>(result)].getArmies() - 1 + get<2>(result));
				atkStr += botName + " attack/transfer " + static_cast<ostringstream*>( &(ostringstream() << get<1>(result)) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << targets3.at(i)) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << atkArmies ) )->str();
				if(get<2>(result) != 0)
				{
					if(reinStr != "")
					{
						reinStr = reinStr + ", ";
					}
					setArmiesLeft(armiesLeft - get<2>(result));
					reinStr += botName + " place_armies " + static_cast<ostringstream*>( &(ostringstream() << std::get<1>(result)) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << get<2>(result)) )->str();
				}
			}
		}
	}
	if(!targets4.empty())
	{
		for(int i = 0; i < targets4.size(); i++)
		{
			result = findFeasibility(targets4.at(i), armiesLeft, 0);
			if(get<0>(result) == 1 && get<1>(result) != -1)
			{	
				if(atkStr != "")
				{
					atkStr = atkStr + ", ";
				}

				int atkArmies = regions[get<1>(result)].getArmies() - 1 + get<2>(result);

				atkStr += botName + " attack/transfer " + static_cast<ostringstream*>( &(ostringstream() << get<1>(result)) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << targets4.at(i)) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << atkArmies ))->str();
				if(get<2>(result) != 0)
				{
					if(reinStr != "")
					{
						reinStr = reinStr + ", ";
					}
					setArmiesLeft(armiesLeft - get<2>(result));
					reinStr += botName + " place_armies " + static_cast<ostringstream*>( &(ostringstream() << std::get<1>(result)) )->str() + " " + static_cast<ostringstream*>( &(ostringstream() << get<2>(result)) )->str();		
				}
			}
		}
	}
	atkString = atkStr;
	reinString = reinStr;
	createTransferString();
}

//function that checks owned regions against superregions to determine number of bonuses that each bot holds
void Bot::updateBonusesOwned()
{	
	//loop through all superregions
	for(int i = 0; i < superRegions.size(); i++)
	{	
		//loop through all territories in each superregion
	//	for(int j = 0; j < superRegions.at(i).nbRegions; j++)
	//	{

	//	}
	}
}

//function that checks to see if opponent has taken any of my starting regions
void Bot::findOppStart()
{
	//this algorithm will actually take some thinking...
}


/*

Potential problems:

*/







