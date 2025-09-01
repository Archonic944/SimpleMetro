#include <windows.h>
#include <string>

std::string GetExecutablePath() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string executablePath(path);
    return executablePath.substr(0, executablePath.find_last_of("\\/")) + "\\";
}

void Metronome::init(const std::string& soundFilePath) {
    std::string executableDir = GetExecutablePath();
    std::string fullPath = executableDir + soundFilePath;
    // ... rest of the initialization code
}