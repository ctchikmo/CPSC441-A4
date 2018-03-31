#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <vector>

#include "Simulation.h"

// Some key terms: 
// a "scenario" refers to each individual run using N stations, K ready stations, and I probe start level
// a "simulation" refers to the running of X scenarios using N stations, K ready stations, and I probe start level
// a "session" refers to the combination of C simulations. When a session is printed it tabulates and plots all of the simulations that have been run under it. Starting a new session means there are 0 simulation results saved

/////////////////////////////////
// General
/////////////////////////////////

void userLoop();
bool consumeCommand(std::string input);

/////////////////////////////////
// Commands
/////////////////////////////////

bool help();
bool setTotalStations(std::string input);
bool setNumReadyStations(std::string input);
bool setStartingLevel(std::string input);
bool setTotalScenariosToRun(std::string input);
bool viewSimulationParameters();
bool runSimulation(); // Runs the simulation given the parameters for scenarioCount times, averages the results and stores in memory (to be printed later)
bool printSession();
bool newSession();
bool setSaveFileName(std::string input);
bool showSaveFileName();
bool showSaveDirectory();
bool quit(); // Session is NOT automatically printed, do it manually before quitting

/////////////////////////////////
// Helpers
/////////////////////////////////
bool stringToInt(std::string string, int* storeValue);

/////////////////////////////////
// Variables
/////////////////////////////////

std::string directory;
std::string filename("ATW_Session_Results.txt");
std::vector<Simulation> session;

/////////////////////////////////
// Implementation
/////////////////////////////////

// There is one command line argument: the place to save any outputs generated during the programs run
int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cout << "Missing save location. Start program as: ./ATW.exe <full path to directory to save files, or 'cd' for the current directory>" << std::endl;
		exit(1);
	}
	
	directory.assign(argv[1]);
	
	if(directory[0] == 'c' && directory[1] == 'd' && directory.length() == 2) // Assign the current directory to the variable "directory"
	{
		char buffer[100];
		char* currentDir = getcwd(buffer, sizeof(buffer)); // Thanks @ https://stackoverflow.com/questions/2868680/what-is-a-cross-platform-way-to-get-the-current-directory
		if(currentDir)
			directory = currentDir;
	}
	else // Check that the location entered is actually a directory here
	{
		DIR *dpdf;

		dpdf = opendir(directory.c_str());
		if(dpdf != NULL)
			closedir(dpdf);
		else
		{
			std::cout << "Check the directory you entered, it may be invalid." << std::endl;
			exit(1);
		}
	}
	
	session.push_back(Simulation()); // We always start with one simulation ready
	userLoop();
	
	return 0;
}

void userLoop()
{
	std::cout << "Please enter a command. Enter 'help' to see the key terms and commands" << std::endl;
	
	std::string input;
	while(std::getline(std::cin, input))
	{
		std::cout << std::endl;
		
		if(consumeCommand(input) == false) // This should only happen when exiting, or if something goes really wrong (which I don't think anything will cause)
			break;
		
		std::cout << std::endl << std::endl << "Ready for another command." << std::endl;
	}
}

bool consumeCommand(std::string input)
{
	if(input.size() == 4 && input[0] == 'h' && input[1] == 'e' && input[2] == 'l' && input[3] == 'p')
	{
		return help();
	}
	else
	{
		if(input.size() >= 4 && input[0] == 't' && input[1] == 's')
			return setTotalStations(input);
		
		else if(input.size() >= 4 && input[0] == 'r' && input[1] == 's')
			return setNumReadyStations(input);
		
		else if(input.size() >= 4 && input[0] == 's' && input[1] == 'l')
			return setStartingLevel(input);
		
		else if(input.size() >= 4 && input[0] == 's' && input[1] == 'c')
			return setTotalScenariosToRun(input);
		
		else if(input.size() == 2 && input[0] == 'v' && input[1] == 's')
			return viewSimulationParameters();
		
		else if(input.size() == 2 && input[0] == 'r' && input[1] == 'r')
			return runSimulation();
		
		else if(input.size() == 2 && input[0] == 'p' && input[1] == 's')
			return printSession();
		
		else if(input.size() == 2 && input[0] == 'n' && input[1] == 's')
			return newSession();
		
		else if(input.size() >= 4 && input[0] == 'f' && input[1] == 'n')
			return setSaveFileName(input);
		
		else if(input.size() == 2 && input[0] == 'v' && input[1] == 'f')
			return showSaveFileName();
		
		else if(input.size() == 2 && input[0] == 'v' && input[1] == 'd')
			return showSaveDirectory();
		
		else if(input.size() == 2 && input[0] == 'q' && input[1] == 'q')
			return quit();
		
		else
			std::cout << "Command not recognized. Enter 'help' to see all the commands." << std::endl;
	}
	
	return true;
}

bool help()
{
	std::cout << "Key Terms:" << std::endl;
	std::cout << "'scenario' refers to an individual run using N stations, K ready stations, and I probe start level" << std::endl;
	std::cout << "'simulation' refers to the running of X tests using N stations, K ready stations, and I start level" << std::endl;
	std::cout << "'session' refers to the cumulation of all simulation results run (before a new session is started), C" << std::endl;
	std::cout << std::endl;
	
	std::cout << "Commands:" << std::endl;
	std::cout << "Enter 'help' to see the key terms and commands." << std::endl;
	std::cout << "Enter 'ts <Number of stations>' to set the total number of stations (default 1024)." << std::endl;
	std::cout << "Enter 'rs <Number of ready stations>' to set the number of ready stations (default 1)." << std::endl;
	std::cout << "Enter 'sl <Level to start at>' to set the starting probe level (default 0)." << std::endl;
	std::cout << "Enter 'sc <Scenario count>' to set the number of scenario runs (default 100)." << std::endl;
	std::cout << "Enter 'vs' to start a view the simulation parameters" << std::endl;
	std::cout << "Enter 'rr' to run the a simulation." << std::endl;
	std::cout << "Enter 'ps' to print the results of the current session to file." << std::endl;
	std::cout << "Enter 'ns' to start a new session, will clear all the data from the previous session." << std::endl;
	std::cout << "Enter 'fn <Filename.txt>' to set the filename to save to (defaults to ATW_Session_Results.txt)." << std::endl;
	std::cout << "Enter 'vf' to see what the save filename is set to." << std::endl;
	std::cout << "Enter 'vd' to see what the directory the save file is being saved in is." << std::endl;
	std::cout << "Enter 'qq' to quit. Note that the session is NOT printed here, do so manually before quitting." << std::endl;
	
	return true;
}

bool setTotalStations(std::string input)
{
	int stationsN;
	if(stringToInt(input.substr(3, std::string::npos), &stationsN) == true)
	{
		if(stationsN <= 0)
			std::cout << "Please enter a value greater than 0." << std::endl;
		else if(stationsN > 1048576) // This is 2^20
			std::cout << "Please enter a value less than or equal to 1048576 (2^20)" << std::endl;
		else
			session.back().stationsN = stationsN;
	}
	else
	{
		std::cout << "Please enter an integer value greater than 0." << std::endl;
	}
	
	return true;
}

bool setNumReadyStations(std::string input)
{
	int readyStationsK;
	if(stringToInt(input.substr(3, std::string::npos), &readyStationsK) == true)
	{
		if(readyStationsK <= 0)
			std::cout << "Please enter a value greater than 0." << std::endl;
		else
			session.back().readyStationsK = readyStationsK;
	}
	else
	{
		std::cout << "Please enter an integer value greater than 0." << std::endl;
	}
	
	return true;
}

bool setStartingLevel(std::string input)
{
	int probeLevelI;
	if(stringToInt(input.substr(3, std::string::npos), &probeLevelI) == true)
	{
		if(probeLevelI < 0)
			std::cout << "Please enter a value greater than or equal to 0." << std::endl;
		else
			session.back().probeLevelI = probeLevelI;
	}
	else
	{
		std::cout << "Please enter an integer value greater than or equal to 0." << std::endl;
	}
	
	return true;
}

bool setTotalScenariosToRun(std::string input)
{
	int scenariosX;
	if(stringToInt(input.substr(3, std::string::npos), &scenariosX) == true)
	{
		if(scenariosX <= 0)
			std::cout << "Please enter a value greater than 0." << std::endl;
		else
			session.back().scenariosX = scenariosX;
	}
	else
	{
		std::cout << "Please enter an integer value greater than 0." << std::endl;
	}
	
	return true;
}

bool viewSimulationParameters()
{
	std::cout << "Simulation will run with: " << std::endl;
	std::cout << session.back().stationsN << " Station(s)." << std::endl;
	std::cout << session.back().readyStationsK << " Ready station(s)." << std::endl;
	std::cout << session.back().probeLevelI << " As the starting probe level." << std::endl;
	std::cout << session.back().scenariosX << " Scenario(s)." << std::endl;
	
	return true;
}

bool runSimulation()
{
	viewSimulationParameters();
	std::cout << std::endl;
	std::cout << session.back().run();
	
	session.push_back(Simulation()); // We are starting a new simulation to collect data on now in this session.
	
	std::cout << std::endl;
	std::cout << "Done Saving data to session, you are on a new simulation with default values now." << std::endl;
	return true;
}

bool printSession()
{
	return true;
}

bool newSession()
{
	session.clear();
	session.push_back(Simulation());
	return true;
}

bool setSaveFileName(std::string input)
{
	filename.assign(input.substr(3, std::string::npos));
	return true;
}

bool showSaveFileName()
{
	std::cout << "Save filename is: " << filename << std::endl;
	return true;
}

bool showSaveDirectory()
{
	std::cout << "Directory being saved to is: " << directory << std::endl;
	return true;
}

bool quit()
{
	return false;
}

bool stringToInt(std::string string, int* storeValue)
{
	try
	{
		int rv = std::stoi(string); // The exception will be thrown here if there is one
		*storeValue = rv;
	}
	catch(...)
	{
		return false;
	}
	
	return true;
}




















