#import <Cocoa/Cocoa.h>
#include "Metronome.h"

// Private implementation class
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
    
    // Test if file exists
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
        [playingSounds addObject:sound]; // Retain reference
        [sound play];
        [sound release]; // Set still retains it
    }
}

// Delegate method - called when sound finishes
- (void)sound:(NSSound *)sound didFinishPlaying:(BOOL)finishedPlaying {
    [playingSounds removeObject:sound]; // Release reference
}

- (void)dealloc {
    [soundPath release];
    [playingSounds release];
    [super dealloc];
}
@end

Metronome::Metronome() : impl(nullptr) {
}

Metronome::~Metronome() {
    if (impl) {
        MetronomeImpl* implPtr = static_cast<MetronomeImpl*>(impl);
        [implPtr release];
    }
}

bool Metronome::init(const std::string& filePath) {
    NSString* nsPath = [NSString stringWithUTF8String:filePath.c_str()];
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