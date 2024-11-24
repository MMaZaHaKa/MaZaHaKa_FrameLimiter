#pragma once
#include "Windows.h"
#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
#include <ctime>

#include "INIReader.h"
#define CFG_LOG_ERROR

namespace nsCfgUtils
{
	bool FileExists(std::string filePath)
	{
		//if (is_valid_utf8(filePath)) { filePath = utf8_to_cp1251(filePath); }
		std::ifstream file(filePath);
		return file.good();
	}

	void FileWrite(std::string filePath, std::string text)
	{
		//if (is_valid_utf8(filePath)) { filePath = utf8_to_cp1251(filePath); }

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
		//if (is_valid_utf8(filePath)) { filePath = utf8_to_cp1251(filePath); }
		std::ofstream file(filePath, std::ios::out | std::ios::trunc);
		if (file.is_open()) { file.close(); }
	}

	void FileWriteAllLines(std::string filePath, std::vector<std::string> lines)
	{
		//if (is_valid_utf8(filePath)) { filePath = utf8_to_cp1251(filePath); }
		if (!FileExists(filePath)) { FileClearMake(filePath); }
		std::ofstream file(filePath);
		if (!file.is_open()) { return; }
		for (const std::string& line : lines) { file << line << '\n'; }

		file.close();
	}
}

class Config // REAL EXAMPLE: val = 1.2
{
public:
	INIReader* m_pReader = nullptr;
	std::string m_sPath = "";

	// vars [m Member]
	int m_iFPSLimit = 60;
	bool m_bNeedLock = true;
	int m_iLockKeyBind = 115; // VK_F4
	bool m_bNeedKeyBind = true;

	void SetLocale()
	{
		//std::ifstream infile(settingsFile);
		//std::locale loc("C");
		//infile.imbue(loc);
		//std::locale::global(std::locale("en_US.UTF-8"));
		//setlocale(LC_ALL, "Russian");
		setlocale(LC_NUMERIC, "C"); // без него не парсит real (float, double)
	}

	//Config() { } // no default ctor
	Config(std::string cfg_path)
	{
		if (!nsCfgUtils::FileExists(cfg_path)) { this->MakeStandartIni(cfg_path); }
		this->SetLocale();
		this->m_sPath = cfg_path;
		this->m_pReader = new INIReader(this->m_sPath);
	}

	void ReInit(std::string cfg_path)
	{
		delete this->m_pReader;
		if (!nsCfgUtils::FileExists(cfg_path)) { this->MakeStandartIni(cfg_path); }
		this->SetLocale();
		this->m_sPath = cfg_path;
		this->m_pReader = new INIReader(this->m_sPath);
#ifdef CFG_LOG_ERROR
		//if (reader->ParseError() < 0) { std::cout << "Can't load ini file\n"; return; }
#endif
		IniParse();
	}

	bool IniParse()
	{
		if (this->m_pReader->ParseError() < 0) { return false; }

		// example
		//m_bPodlyanki = this->m_pReader->GetBoolean("MAIN", "Podlyanki", false);
		//m_iSoundVolume = this->m_pReader->GetInteger("MAIN", "SoundVolume", 0); // 0-100%
		//m_fSpinErrorDegreeRange = this->m_pReader->GetReal("MAIN", "SpinErrorDegreeRange", 0.0); // 7.0

		this->m_iFPSLimit = this->m_pReader->GetInteger("MAIN", "FPS", 60); // 60 default
		this->m_bNeedLock = this->m_pReader->GetBoolean("MAIN", "NeedLock", true); // true
		this->m_iLockKeyBind = this->m_pReader->GetInteger("MAIN", "LockKeyBind", 115); // 115 default
		this->m_bNeedKeyBind = this->m_pReader->GetBoolean("MAIN", "NeedKeyBind", true); // true

		return true;
	}

	void Dump()
	{
		std::cout << "m_iFPSLimit: " << this->m_iFPSLimit << "\n";
		std::cout << "m_bNeedLock: " << this->m_bNeedLock << "\n";
		std::cout << "m_iLockKeyBind: " << this->m_iLockKeyBind << "\n";
		std::cout << "m_bNeedKeyBind: " << this->m_bNeedKeyBind << "\n";
		std::cout << "\n";
	}

	void MakeStandartIni(std::string sIniPath)
	{
		nsCfgUtils::FileWriteAllLines(sIniPath,
		{
			"[MAIN]",
			"",
			"FPS = 60",
			"NeedLock = 1",
			"LockKeyBind = 115",
			"NeedKeyBind = 1",
		});
	}
};