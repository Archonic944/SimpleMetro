CXX = g++
CXXFLAGS = -std=c++17 -I/opt/homebrew/include
LIBS = -L/opt/homebrew/lib -lsfml-audio -lsfml-system
TARGET = main
SOURCE = main.cc

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(SOURCE) $(LIBS) -o $(TARGET).out

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET).out

.PHONY: clean run