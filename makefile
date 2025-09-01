CXX := g++

CXXFLAGS := -Wall -std=c++17 -I/mingw64/include
LDFLAGS := -L/mingw64/lib -lglfw3 -lopengl32

SRC := src/main.cpp src/shader.cpp src/worldgen.cpp src/chunk.cpp src/camera.cpp src/glad.c
TARGET := FreakCraft.exe

OBJDIR := obj
OBJECTS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(filter %.cpp,$(SRC))) $(patsubst src/%.c,$(OBJDIR)/%.o,$(filter %.c,$(SRC)))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: src/%.c
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET)
	rm -rf $(OBJDIR)

.PHONY: all clean
