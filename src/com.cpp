#include "com.h"
#include <fstream>
#include <iostream>
#include <syncstream>

namespace Panzer
{
	namespace Com
	{
		std::ofstream debugFileStream;
		bool isDebugFileOpen;
		void OpenDebugFile() 
		{ 
			if (!isDebugFileOpen)
			{
				debugFileStream.open("log.txt", std::ios_base::app); 
				isDebugFileOpen = true;
			}
		}

		void CloseDebugFile()
		{
			debugFileStream.close();
			isDebugFileOpen = false;
		}

		void SetDebug(bool debug)
		{
			printDebug = debug;
		}

		void OutputDebugFile(std::string output)
		{
			if (printDebug && isDebugFileOpen)
			{
				debugFileStream << output << std::endl;
			}
		}

		void SendMessageToUI(std::string message)
		{
			std::osyncstream syncout(std::cout);
			syncout << message << std::endl;
		}

	}
}