#include "Simulation.h"

std::string Simulation::run()
{
	
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






