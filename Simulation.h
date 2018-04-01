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
		int probeLevelActuallyUsed = 0; // two probe levels for copying and display purposes. 
		int scenariosX = 100;
		bool useBasicAlg = true;
		
		Simulation();
		Simulation(int n, int k, int i, int x, bool basic);
		
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
		
		// Comparing these two algorithms:
		// If the active stations is known, and the start level is simply being guessed for basic, than the advanced will outpreform the basic alg.
		// If the basic is given the same start level as advanced, the advanced algorithm performs better than the basic algorithm the less total% of active stations there are.
		// 		As active station count approaches total stations the two algorithms become identical in performance as the advanced algorithm can not make the slight adjustments that increase performance. 
		void basicProbeWalkthrough(std::vector<Station>* stations, int nodesToProbe, int shuffle, int nodeOffset);
		void advancedProbeWalkthrough(std::vector<Station>* stations, int nodesToProbe, int shuffle, int nodeOffset, bool parentHadCollision); // parentHadCollision arg used to reduce collision probes
};

#endif