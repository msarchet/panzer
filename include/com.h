#pragma once
#include <fstream>

namespace Panzer
{
	namespace Com
	{
		static bool printDebug = false;
		void OpenDebugFile();
		void CloseDebugFile();
		void OutputDebugFile(std::string output);
		void SendMessageToUI(std::string message);
		void SetDebug(bool debug);
		inline bool GetDebug() { return printDebug; }
	}
}