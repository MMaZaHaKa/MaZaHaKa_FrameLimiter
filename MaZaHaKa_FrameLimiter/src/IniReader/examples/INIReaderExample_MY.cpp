#pragma once
#include "Windows.h";
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
//#include <thread>
//#include <random>
#include "XMemory\XMemory.h"
#include "IniReader\IniReader.h"


#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))




//------------------------------------------------------------------
HANDLE InitConsole() // with proto
{
	AllocConsole();

	//SetConsoleOutputCP(866);
	setlocale(LC_ALL, "Russian");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);


	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

	return hConsole;
}

//void LeaveConsole(HANDLE hConsole = nullptr)
void LeaveConsole(HANDLE hConsole) // with proto
{
	if (hConsole != nullptr) { SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); } // Reset to default color
	FreeConsole();
}




//-----------------------------------------------------------------
bool FileExists(std::string filePath)
{
	std::ifstream file(filePath);
	return file.good();
}

void FileWrite(std::string filePath, std::string text)
{

	// Open the file for writing. If it doesn't exist, create it.
	std::ofstream file(filePath, std::ios::out | std::ios::trunc);

	if (file.is_open()) {
		// Write the text to the file.
		file << text;

		// Close the file.
		file.close();
		//std::cout << "File written successfully." << std::endl;
	}
	//else {
	//	std::cerr << "Error opening or creating the file." << std::endl;
	//}
}
void FileClearMake(std::string filePath) // or make empty file
{
	std::ofstream file(filePath, std::ios::out | std::ios::trunc);
	if (file.is_open()) { file.close(); }
}
void FileWriteAllLines(std::string filePath, std::vector<std::string> lines)
{
	if (!FileExists(filePath)) { FileClearMake(filePath); }
	std::ofstream file(filePath);
	if (!file.is_open()) { return; }
	for (const std::string& line : lines) { file << line << '\n'; }

	file.close();
}






int g_fps_max = 60;



void IniInit()
{ // NOT Unicode path settings!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	const char* ini_path = "fps_limiter.ini";
	if (!FileExists(ini_path)) { FileWriteAllLines(ini_path, { "[MAIN]", "FPSlimit = 60" }); } // mk ini

	INIReader ini(ini_path);
	if (ini.ParseError() < 0) { std::cout << "Can't load 'fps_limiter.ini'\n"; return; }
	g_fps_max = ini.GetInteger("MAIN", "FPSlimit", 60); // read [] dont need

	std::cout << "g_fps_max: " << g_fps_max << "\n";
}

void Patch(void* core_base_pointer)
{
	IniInit();
	//PatchBefore(core_base_pointer);
	//PatchAfter(core_base_pointer); // ??
}



void AsyncPatch()
{
	const char* module_name = "e2mfc.dll";
	HMODULE hmod = NULL;
	while (!hmod)
	{
		hmod = GetModuleHandleA(module_name);
		Sleep(50);
	}
	//--------------------------------------
	//std::cout << "AsyncPatch() Modules Loaded!!" << "\n";

	Patch(hmod);
}



void OnStart()
{
	InitConsole();
	AsyncPatch();
	//Patch();
	//TestPatch();
}



BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{ // https://github.com/T1MOXA/CSSv90Spoofer/blob/master/src/SidSpoofer.cpp
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstDLL);
		//CreateThread(NULL, 0, SpooferEntry, NULL, 0, NULL);
		OnStart();
	}

	return TRUE;
}


