#include <iostream>
#include <unistd.h>
#include <dirent.h>

// There is one command line argument: the place to save any outputs generated during the programs run
int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cout << "Missing save location. Start program as: ./ATW.exe <full path to directory to save files, or 'cd' for the current directory>" << std::endl;
		exit(1);
	}
	
	std::string directory(argv[1]);
	
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
	
	return 0;
}