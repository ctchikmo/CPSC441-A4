#ifndef SIMULATION_H
#define SIMULATION_H

#include <string>

struct Station
{
	bool active = false;
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
};

#endif