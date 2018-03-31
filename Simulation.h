#ifndef SIMULATION_H
#define SIMULATION_H

#include <string>
#include <vector>

struct Station
{
	bool active = false;
	int number = 0;
};

class Simulation
{
	public:
		int stationsN = 1024;
		int readyStationsK = 1;
		int probeLevelI = 0;
		int scenariosX = 100;
		bool useBasicAlg = true;
		
		std::string run(); // Returns a message from running.
		
		double getSuccessProbesPercent();
		double getCollisionProbesPercent();
		double getIdleProbesPercent();
		
	private:
		int successProbes = 0;
		int collisionProbes = 0;
		int idleProbes = 0;
		int readyStationsLeft = 1; // Used to reduce idle probes
		
		double cumulativeSuccessPercentage = 0;
		double cumulativeCollisionPercentage = 0;
		double cumulativeIdlePercentage = 0;
		
		void basicProbeWalkthrough(std::vector<Station>* stations, int nodesToProbe, int shuffle, int nodeOffset);
		void advancedProbeWalkthrough(std::vector<Station>* stations, int nodesToProbe, int shuffle, int nodeOffset, bool parentHadCollision); // parentHadCollision arg used to reduce collision probes
};

#endif