#include <Windows.h>
#include <mmsystem.h>
#include <string>
#include <filesystem>
#include "Metronome.h"

std::string filename;

class MetronomeWinImpl {
	public:
		std::string filename;
};

Metronome::Metronome() {
	impl = new MetronomeWinImpl();
}

bool Metronome::init(const std::string& soundFile) {
	// Get the executable's directory path
	char exePath[MAX_PATH];
	DWORD result = GetModuleFileNameA(NULL, exePath, MAX_PATH);
	if (result == 0) {
		return false; // Failed to get executable path
	}
	
	// Extract directory from executable path
	std::filesystem::path executablePath(exePath);
	std::filesystem::path executableDir = executablePath.parent_path();
	
	// Join with the relative sound file path
	std::filesystem::path soundFilePath = executableDir / soundFile;
	
	// Store the absolute path
	((MetronomeWinImpl*)impl)->filename = soundFilePath.string();
	return true;
}

void Metronome::playClick() {
	PlaySoundA(((MetronomeWinImpl*)impl)->filename.c_str(), NULL, SND_FILENAME | SND_ASYNC);
}

Metronome::~Metronome() {
	delete (MetronomeWinImpl*)impl;
}
