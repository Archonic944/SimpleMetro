#include <Windows.h>
#include <mmsystem.h>
#include <string>
#include <libgen.h>
#include "Metronome.h"

class MetronomeWinImpl {
    public:
        std::string filename;
};

std::string getExecutableDir() {
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string exeDir = exePath;
    size_t pos = exeDir.find_last_of("\\/");
    if (pos != std::string::npos) {
        exeDir = exeDir.substr(0, pos);
    }
    return exeDir;
}

Metronome::Metronome() {
    impl = new MetronomeWinImpl();
}

bool Metronome::init(const std::string& soundFile) {
    std::string exeDir = getExecutableDir();
    std::string fullPath = exeDir + "\\" + soundFile;
    ((MetronomeWinImpl*)impl)->filename = fullPath;
    return true;
}

void Metronome::playClick() {
    PlaySoundA(((MetronomeWinImpl*)impl)->filename.c_str(), NULL, SND_FILENAME | SND_ASYNC);
}

Metronome::~Metronome() {
    delete (MetronomeWinImpl*)impl;
}
