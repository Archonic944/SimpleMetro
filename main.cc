#include <string>
#include <iostream>
using namespace std;
#include <SFML/Audio.hpp>
#include <thread>

unsigned long getEpochMs()
{
    auto now = chrono::system_clock::now();
    auto epoch = now.time_since_epoch();
    return chrono::duration_cast<chrono::milliseconds>(epoch).count();
}

int main(int argc, char *argv[])
{
    sf::SoundBuffer buffer;
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
    if (buffer.loadFromFile("sounds/metronome.wav"))
    {
        sf::Sound sound(buffer);
        while (true)
        {
            auto ms = getEpochMs();
            if (ms >= nextBeat)
            {
                sound.play();
                nextBeat += interval;
                cout << "Next beat is " << (nextBeat - ms) << " ms away" << endl;
            }
        }
    }
    else
    {
        cerr << "Error loading sound file" << endl;
    }

    return 0;
}