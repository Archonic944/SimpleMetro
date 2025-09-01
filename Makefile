CXX = clang++
CXXFLAGS = -std=c++17
LDFLAGS_MAC = -framework AudioToolbox -framework CoreFoundation -framework Cocoa

MAIN_SOURCE = main.cc
MAC_SOURCE = MetronomeMac.mm
TARGET = metronome

out:
	mkdir -p out

mac: out
	$(CXX) $(CXXFLAGS) -c $(MAC_SOURCE) -o out/MetronomeMac.o -DMAC
	$(CXX) $(CXXFLAGS) $(MAIN_SOURCE) out/MetronomeMac.o $(LDFLAGS_MAC) -o out/$(TARGET)_mac -DMAC

clean:
	rm -f out/$(TARGET)_mac out/*.o

run: mac
	./out/$(TARGET)_mac

.PHONY: clean run mac