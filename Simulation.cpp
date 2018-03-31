#include <algorithm>    // std::random_shuffle
#include <iostream>

#include "Simulation.h"

std::string Simulation::run()
{
	int levelCount = 1;
	int levelWidth = 1;
	
	// This loop determines the total number of levels for the tree. I could do this with the square root method, but thats more accuracy than needed for this.
	while(levelWidth < stationsN)
	{
		levelCount++;
		levelWidth <<= 1; // *= 2, but better
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
	std::vector<Station> stations(stationsN);
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
			stations[k].active = true; // Note that readyStationsK is always less than or equal to stationsN
		
		// Start probing
		int nodesToProbe = 1 << probeLevelI;
		if(nodesToProbe > stationsN) 
			nodesToProbe = stationsN;
		int shuffle = levelCount - 1 - probeLevelI;
		
		if(useBasicAlg)
			basicProbeWalkthrough(&stations, nodesToProbe, shuffle, 0);
		
		// Add to the percentages
		double totalProbes = successProbes + collisionProbes + idleProbes; // This will always be greater than 0, as there is always one node.
		cumulativeSuccessPercentage += (double)(successProbes) / totalProbes;
		cumulativeCollisionPercentage += (double)(collisionProbes) / totalProbes;
		cumulativeIdlePercentage += (double)(idleProbes) / totalProbes;
		
		// Reset the counters
		successProbes = 0;
		collisionProbes = 0;
		idleProbes = 0;
		
		if((x + 1) % 25 == 0)
			std::cout << "Scenarios: " << (x - 24) << " - " << (x) << " complete." << std::endl;
	}
	
	return returnMessage;
}

void Simulation::basicProbeWalkthrough(std::vector<Station>* stations, int nodesToProbe, int shuffle, int nodeOffset)
{
	for(int node = 0; node < nodesToProbe; node++)
	{
		int hitActiveIndex = 0;
		bool hitActive = false;
		bool collision = false;
		
		for(int n = 0; n < stationsN; n++)
		{
			if(((*stations)[n].number >> shuffle) == (node + nodeOffset) && (*stations)[n].active == true)
			{
				if(hitActive == true)
				{
					collision = true;
					break;
				}
				
				hitActive = true;
				hitActiveIndex = n;
			}
		}
		
		if(collision == true) // If this happens, we need to go into the causing node, which is the current one.
		{
			collisionProbes++;
			basicProbeWalkthrough(stations, 2, shuffle - 1, node * 2); // Always probe for 2 nodes, since we are binary a collision means only 2 branches to go through from this level. (works fine in case of 1 leaf node)
		}
		else if(hitActive == true)
		{
			(*stations)[hitActiveIndex].active = false;
			successProbes++;
		}
		else // No actives found, so wasted probe (idle).
		{
			idleProbes++;
		}
	}
}

double Simulation::getSuccessProbesPercent()
{
	return cumulativeSuccessPercentage / (double)scenariosX * 100;
}

double Simulation::getCollisionProbesPercent()
{
	return cumulativeCollisionPercentage / (double)scenariosX * 100;
}

double Simulation::getIdleProbesPercent()
{
	return cumulativeIdlePercentage / (double)scenariosX * 100;
}






