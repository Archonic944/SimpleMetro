#include <string>
#include <iostream>
using namespace std;
#include <thread>
#include <chrono>
#include "Metronome.h"

uint64_t getEpochMs()
{
    auto now = chrono::system_clock::now();
    auto epoch = now.time_since_epoch();
    return chrono::duration_cast<chrono::milliseconds>(epoch).count();
}

int main(int argc, char *argv[])
{
    Metronome sound;
    if (!sound.init("sounds/metronome.wav"))
    {
        cerr << "Failed to initialize metronome sound.\nThe sound might not exist: does the executable share a root with the resources folder?" << endl;
        return 1;
    }
    int bpm = 120;
    if (argc <= 1)
    {
        cout << "No BPM argument. Usage: " << argv[0] << " <bpm>" << endl
             << "Defaulting to 120 BPM." << endl;
    }
    else
    {
        bpm = stoi(argv[1]);
        if (bpm <= 0)
        {
            cerr << "Invalid BPM. BPM must be positive." << endl;
            cerr << "Usage: " << argv[0] << " <bpm>" << endl;
            return 1;
        }
    }
    int interval = 60000 / bpm; // Milliseconds
    auto ms = getEpochMs();
    unsigned long nextBeat = ms + (ms % interval);
    cout << "Next beat is " << (nextBeat - ms) << " ms away" << endl;
    cout << ms << endl;
    // sound.playClick();
    // this_thread::sleep_for(chrono::milliseconds(interval));
    while (true)
    {
        auto ms = getEpochMs();
        if (ms >= nextBeat)
        {
            sound.playClick();
            nextBeat += interval;
            cout << "Next beat is " << (nextBeat - ms) << " ms away" << endl;
        }
    }

    return 0;
}