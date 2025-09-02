#import <Cocoa/Cocoa.h>
#include "Metronome.h"
#include <mach-o/dyld.h>
#include <unistd.h>
#include <libgen.h>

@interface MetronomeImpl : NSObject <NSSoundDelegate> {
    NSString* soundPath;
    NSMutableSet* playingSounds;
}
- (BOOL)initWithPath:(NSString*)path;
- (void)play;
@end

@implementation MetronomeImpl
- (BOOL)initWithPath:(NSString*)path {
    soundPath = [path retain];
    playingSounds = [[NSMutableSet alloc] init];
    NSSound* testSound = [[NSSound alloc] initWithContentsOfFile:path byReference:NO];
    if (testSound) {
        [testSound release];
        return YES;
    }
    return NO;
}
- (void)play {
    if (soundPath) {
        NSSound* sound = [[NSSound alloc] initWithContentsOfFile:soundPath byReference:NO];
        [sound setDelegate:self];
        [playingSounds addObject:sound];
        [sound play];
        [sound release];
    }
}
- (void)sound:(NSSound *)sound didFinishPlaying:(BOOL)finishedPlaying {
    [playingSounds removeObject:sound];
}
- (void)dealloc {
    [soundPath release];
    [playingSounds release];
    [super dealloc];
}
@end

std::string getExecutableDir() {
    char exePath[PATH_MAX];
    uint32_t size = sizeof(exePath);
    if (_NSGetExecutablePath(exePath, &size) == 0) {
        return std::string(dirname(exePath));
    }
    return "";
}

Metronome::Metronome() : impl(nullptr) {}

Metronome::~Metronome() {
    if (impl) {
        MetronomeImpl* implPtr = static_cast<MetronomeImpl*>(impl);
        [implPtr release];
    }
}

bool Metronome::init(const std::string& filePath) {
    std::string exeDir = getExecutableDir();
    std::string fullPath = exeDir + "/" + filePath;
    NSString* nsPath = [NSString stringWithUTF8String:fullPath.c_str()];
    MetronomeImpl* implPtr = [[MetronomeImpl alloc] init];
    if ([implPtr initWithPath:nsPath]) {
        impl = static_cast<void*>(implPtr);
        return true;
    } else {
        [implPtr release];
        return false;
    }
}

void Metronome::playClick() {
    if (impl) {
        MetronomeImpl* implPtr = static_cast<MetronomeImpl*>(impl);
        [implPtr play];
    }
}
