#include "Metronome.h"
#include <ao/ao.h>
#include <sndfile.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>

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
    short buffer[bufferFrames * 2];

    void playbackLoop() {
        while (!exitThread.load(std::memory_order_acquire)) {
            if (playRequested.exchange(false, std::memory_order_acq_rel)) {
                sf_seek(sndfile, 0, SEEK_SET);
                sf_count_t framesRead;
                while ((framesRead = sf_readf_short(sndfile, buffer, bufferFrames)) > 0) {
                    int bytes = static_cast<int>(framesRead * sfinfo.channels * sizeof(short));
                    ao_play(device, (char*)buffer, bytes);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
};

static std::string getExecutableDir() {
    char exePath[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len != -1) {
        exePath[len] = '\0';
        return std::string(dirname(exePath));
    }
    return "";
}

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

    std::string exeDir = getExecutableDir();
    std::string fullPath = exeDir + "/" + filePath;

    pimpl->sndfile = sf_open(fullPath.c_str(), SFM_READ, &pimpl->sfinfo);
    if (!pimpl->sndfile) {
        std::cerr << "Failed to open sound file: " << fullPath << std::endl;
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
