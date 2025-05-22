CXX = g++
CXXFLAGS = -std=c++17 -O2 -I./bliss -MMD -MP
LDFLAGS = -L. -lbliss_static

TARGET = kneissler_gen
SRC = kneissler_gen.cpp
DEP = $(SRC:.cpp=.d)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS) 
	
-include $(DEP)

clean:
	rm -f $(TARGET)