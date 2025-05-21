CXX = g++
CXXFLAGS = -std=c++17 -O2 -I./bliss
LDFLAGS = -L. -lbliss_static

TARGET = kneissler_gen
SRC = kneissler_gen.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)