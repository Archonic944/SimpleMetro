#include <Windows.h>
#include <mmsystem.h>
#include "Metronome.h"

std::string filename;

class MetronomeWinImpl {
	public:
		std::string filename;
};

Metronome::Metronome() {
	impl = new MetronomeWinImpl();
}

bool Metronome::init(std::string soundFile) {
	((MetronomeWinImpl*)impl)->filename = soundFile;
	return true;
}

void Metronome::playClick() {
	PlaySoundA(((MetronomeWinImpl*)impl)->filename.c_str(), NULL, SND_FILENAME | SND_ASYNC);
}

Metronome::~Metronome() {
	delete (MetronomeWinImpl*)impl;
}
