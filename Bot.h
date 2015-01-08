#include <iostream>
#include <stdio.h>
#include <tuple>
#include <fstream>
#include <iostream>
#include "Parser.h"
#include "Region.h"
#include "SuperRegion.h"
#include "RegionObject.h"

using namespace std;

class Bot
{
    ifstream in;
    vector<Region> regions;
    vector<SuperRegion> superRegions;
    string botName;
    string opponentBotName;
    vector<int> startingRegionsreceived;
    vector<int> ownedRegions;
    string atkString;
    string reinString;
    string tranString;
    string startRegString;
    vector<int> targets1;
    vector<int> targets2;
    vector<int> targets3;
    vector<int> targets4;
    vector<int> startReg;
    vector<RegionObject> mapGraph;
    int armiesLeft;
    int round;
    Parser parser;
    string phase;

    public:
        Bot();
        virtual ~Bot();
	vector<int>& getEnemies(int noRegion);
        vector<int>& getNeutrals(int noRegion);
	vector<int>& findBestReinforceRegions();
	int findEasiestSuper();
	int isWinnable(int atkArmies, int defArmies);
	tuple<int, int, int> findFeasibility(int noRegion, int numRein, int priFlag);
	int findBestAtkTerrHiPri(int noRegion);
	int findBestAtkTerrLoPri(int noRegion);
	int moveToEnemy(int start);
	int moveToNeutral(int start);
	void updateBonusesOwned();
	void findOppStart();
	void createTransferString();
	void createStartRegString();
	void updateTargets();
	void buildMove();
	void initMapGraph();
	void printMapGraph();
	void resetMapGraphVisibility();
	void resetMapGraphVisits();

    void playGame();    //plays a single game of Warlight
    void makeMoves();   //makes moves for a single turn
    void endTurn();     //indicates to the engine that it has made its moves
    void addRegion(unsigned noRegion, unsigned noSuperRegion);
    void addSuperRegion(unsigned noSuperRegion, int reward);
    void addNeighbors(unsigned noRegion, unsigned Neighbors);
    void setBotName(string name);
    void setOpponentBotName(string name);
    void setArmiesLeft(int nbArmies);
    void addArmies(unsigned noRegion,int nbArmies);
    void moveArmies(unsigned noRegion,unsigned toRegion,int nbArmies);
    void addStartingRegion(unsigned noRegion);
    void startDelay(int delay);
    void setPhase(string pPhase);
    void executeAction();
    void updateRegion(unsigned noRegion, string playerName, int nbArmies);
    void resetRegionsOwned();

    private:
};
