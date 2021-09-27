#include "com.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <syncstream>

namespace Panzer
{
	namespace Com
	{
		std::ofstream debugFileStream;
		bool isDebugFileOpen;
		bool printDebug;
		void OpenDebugFile() 
		{ 
			if (!isDebugFileOpen)
			{
				if (!std::filesystem::exists("./debugging-output") && !std::filesystem::create_directory("./debugging-output"))
				{
					Panzer::Com::SendMessageToUI("DEBUG: failed to create debug output directory");
					return;
				}

				debugFileStream.open("./debugging-output/log.txt", std::ios_base::app); 
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

		bool GetDebug() { return printDebug; }

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