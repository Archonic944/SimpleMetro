#pragma once
#include <string>

class Metronome
{
public:
    Metronome();
    ~Metronome();
    bool init(const std::string &filePath);
    void playClick();

private:
    void *impl; // Opaque pointer to hide Objective-C++ implementation on MacOz
};