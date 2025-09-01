#include "Metronome.h"
#include <ao/ao.h>
#include <sndfile.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

class MetronomeLinuxImpl {
public:
    SNDFILE* sndfile = nullptr;
    SF_INFO sfinfo{};
    ao_device* device = nullptr;
    ao_sample_format format{};
    std::thread playbackThread;
    std::atomic<bool> playRequested{ false };
    std::atomic<bool> exitThread{ false };

    static constexpr int bufferFrames = 1024;
    short buffer[bufferFrames * 2];  // assuming max 2 channels

    void playbackLoop() {
        while (!exitThread.load(std::memory_order_acquire)) {
            if (playRequested.exchange(false, std::memory_order_acq_rel)) {
                // Reset to start of file
                sf_seek(sndfile, 0, SEEK_SET);

                sf_count_t framesRead;
                while ((framesRead = sf_readf_short(sndfile, buffer, bufferFrames)) > 0) {
                    int bytes = static_cast<int>(framesRead * sfinfo.channels * sizeof(short));
                    ao_play(device, (char*)buffer, bytes);
                }
            }

            // Prevent busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
};

Metronome::Metronome() {
    impl = new MetronomeLinuxImpl();
    ao_initialize();
}

Metronome::~Metronome() {
    auto* pimpl = static_cast<MetronomeLinuxImpl*>(impl);
    if (pimpl) {
        pimpl->exitThread.store(true, std::memory_order_release);

        if (pimpl->playbackThread.joinable()) {
            pimpl->playbackThread.join();
        }

        if (pimpl->device) {
            ao_close(pimpl->device);
        }

        if (pimpl->sndfile) {
            sf_close(pimpl->sndfile);
        }

        ao_shutdown();
        delete pimpl;
    }
}

bool Metronome::init(const std::string& filePath) {
    auto* pimpl = static_cast<MetronomeLinuxImpl*>(impl);

    pimpl->sndfile = sf_open(filePath.c_str(), SFM_READ, &pimpl->sfinfo);
    if (!pimpl->sndfile) {
        std::cerr << "Failed to open sound file: " << filePath << std::endl;
        return false;
    }

    pimpl->format.bits = 16;
    pimpl->format.channels = pimpl->sfinfo.channels;
    pimpl->format.rate = pimpl->sfinfo.samplerate;
    pimpl->format.byte_format = AO_FMT_LITTLE;
    pimpl->format.matrix = nullptr;

    pimpl->device = ao_open_live(ao_default_driver_id(), &pimpl->format, nullptr);
    if (!pimpl->device) {
        std::cerr << "Failed to open audio device." << std::endl;
        sf_close(pimpl->sndfile);
        pimpl->sndfile = nullptr;
        return false;
    }

    // Start playback thread
    pimpl->playbackThread = std::thread(&MetronomeLinuxImpl::playbackLoop, pimpl);

    return true;
}

void Metronome::playClick() {
    auto* pimpl = static_cast<MetronomeLinuxImpl*>(impl);
    if (!pimpl->device || !pimpl->sndfile) {
        std::cerr << "Audio device or sound file not initialized." << std::endl;
        return;
    }

    pimpl->playRequested.store(true, std::memory_order_release);
}
