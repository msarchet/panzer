#pragma once
#include <fstream>

namespace Panzer {
namespace Com {
void OpenDebugFile();
void CloseDebugFile();
void OutputDebugFile(std::string output);
void SendMessageToUI(std::string message);
void FlushUI();
void SetDebug(bool debug);
bool GetDebug();
} // namespace Com
} // namespace Panzer