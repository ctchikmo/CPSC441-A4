#include <algorithm>    // std::random_shuffle

#include "Simulation.h"

std::string Simulation::run()
{
	int levelCount = 1;
	int levelWidth = 1;
	
	// This loop determines the total number of levels for the tree. I could do this with the square root method, but thats more accuracy than needed for this.
	while(levelWidth < stationsN)
	{
		levelCount++;
		levelWidth <<= 1; // *= 2 
	}
	
	std::string returnMessage = "Finished simulation.\r\n";
	if(readyStationsK > stationsN)
	{
		readyStationsK = stationsN;
		returnMessage += " Changed readyStationsK to equal stationsN as it was greater.\r\n";
	}
	
	if(probeLevelI > levelCount - 1)
	{
		probeLevelI = levelCount - 1;
		returnMessage += " Changed probeLevelI to be the last level as it was set to past this level.\r\n"; 
	}
	
	// Each node is a simple true or false, true for 'will transmit', false for 'will not'. 
	std::vector<Station> stations[stationsN];
	for(int n = 0; n < stationsN; n++)
		stations[n].number = n;
	
	// We do the following for every scenario
	for(int x = 0; x < scenariosX; x++)
	{
		// Reset stations
		for(int i = 0; i < stationsN; i++)
			stations[i].active = false;
		
		// Random equal activation of stations. The stations which were randomly shuffled to the front are picked, although chance to be selected goes up as spots are taken, thats okay becuase the only ones with a
		// now unequal chance are the ones that were picked. After each suffle to the front the remaining stations all have an equal chance. Not done using rand() % number, as must check if the station has already 
		// been selected, potentially causing a huge busy loop if we want all stations to be ready.
		std::random_shuffle(stations.begin(), stations.end());
		for(int k = 0; k < readyStationsK; k++)
			stations[k] = true; // Note that readyStationsK is always less than or equal to stationsN
	}
	
	return returnMessage;
}

double Simulation::getSuccessProbesPercent()
{
	return (double)successProbes / (double)scenariosX;
}

double Simulation::getCollisionProbesPercent()
{
	return (double)collisionProbes / (double)scenariosX;
}

double Simulation::getIdleProbesPercent()
{
	return (double)idleProbes / (double)scenariosX;
}






