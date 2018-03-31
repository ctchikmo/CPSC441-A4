#ifndef SIMULATION_H
#define SIMULATION_H

#include <string>
#include <vector>

struct Station
{
	bool active = false;
	bool sent = false;
	int number = -1;
};

class Simulation
{
	public:
		int stationsN = 1024;
		int readyStationsK = 1;
		int probeLevelI = 0;
		int scenariosX = 100;
		
		std::string run(); // Returns a message from running.
		
		double getSuccessProbesPercent();
		double getCollisionProbesPercent();
		double getIdleProbesPercent();
		
	private:
		int successProbes = 0;
		int collisionProbes = 0;
		int idleProbes = 0;
		
		double cumulativeSuccessPercentage = 0;
		double cumulativeCollisionPercentage = 0;
		double cumulativeIdlePercentage = 0;
		
		void basicProbeWalkthrough(std::vector<Station>* stations, int nodesToProbe, int shuffle);
};

#endif