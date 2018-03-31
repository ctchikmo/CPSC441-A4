#include <algorithm>    // std::random_shuffle
#include <iostream>
#include <math.h>

#include "Simulation.h"

Simulation::Simulation()
{
	
}

Simulation::Simulation(int n, int k, int i, int x, bool basic):
stationsN(n),
readyStationsK(k),
probeLevelI(i),
scenariosX(x),
useBasicAlg(basic)
{
	
}

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
		{
			stations[k].active = true; // Note that readyStationsK is always less than or equal to stationsN
		}
		
		int tempLevel = probeLevelI;
		if(useBasicAlg == false)
			tempLevel = (int)round(log2((double)readyStationsK));
		
		// Start probing
		int nodesToProbe = 1 << tempLevel;
		if(nodesToProbe > stationsN) 
			nodesToProbe = stationsN;
		int shuffle = levelCount - 1 - tempLevel;
		
		if(useBasicAlg)
		{
			basicProbeWalkthrough(&stations, nodesToProbe, shuffle, 0);
		}
		else
		{
			readyStationsLeft = readyStationsK;
			advancedProbeWalkthrough(&stations, nodesToProbe, shuffle, 0, false);
		}
		
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
			}
		}
		
		if(collision == true) // If this happens, we need to go into the causing node, which is the current one.
		{
			collisionProbes++;
			basicProbeWalkthrough(stations, 2, shuffle - 1, (node + nodeOffset) * 2); // Always probe for 2 nodes, since we are binary a collision means only 2 branches to go through from this level. (works fine in case of 1 leaf node)
		}
		else if(hitActive == true)
		{
			successProbes++;
		}
		else // No actives found, so wasted probe (idle).
		{
			idleProbes++;
		}
	}
}

// Something very important to note with this: When parentHadCollision == true, it is guranteed that nodesToProbe == 2. As such that call of this function will properly use the bool at the top
void Simulation::advancedProbeWalkthrough(std::vector<Station>* stations, int nodesToProbe, int shuffle, int nodeOffset, bool parentHadCollision)
{
	// If this is true, than we don't need to probe the node that has not yet been checked as we know it causes the collison, so skip to its children.
	// This value only gets used when parentHadCollision == true.
	bool otherNodeWasIdle = false;

	for(int node = 0; node < nodesToProbe && readyStationsLeft != 0; node++)
	{
		bool hitActive = false;
		bool collision = false;
		
		// When shuffle == 0 that means we are at the leaf level as we are checking each stations full exact number. If we are at this point there will never be a collision.
		if(parentHadCollision == true && otherNodeWasIdle == true && shuffle != 0)
		{
			collision = true; // We know, without probing, that this node has the collision as the parent had a collision, and between these two nodes the other node has no send attempts.
		}
		else
		{
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
				}
			}
		}
		
		if(collision == true) // If this happens, we need to go into the causing node, which is the current one.
		{
			// Only increment collisionProbes if we actually probed for that collision. See above for the case where we avoid the probe, but know its a collison. 
			if(parentHadCollision == false || otherNodeWasIdle == false)
				collisionProbes++;
			
			advancedProbeWalkthrough(stations, 2, shuffle - 1, (node + nodeOffset) * 2, true); // See comment here in basicProbeWalkthrough
		}
		else if(hitActive == true)
		{
			successProbes++;
			readyStationsLeft--;
		}
		else // No actives found, so wasted probe (idle).
		{
			otherNodeWasIdle = true;
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






