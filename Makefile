# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Werror -pedantic -Iheader -Icommon

# Source and object files
SRCS := tanks_game.cpp $(wildcard source/*.cpp)
OBJS := $(SRCS:.cpp=.o)

# Output binary name
TARGET := tanks_game

# Default target
all: $(TARGET)

# Link objects into final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the game with arguments: make run ARGS="..."
run: $(TARGET)
	./$(TARGET) $(ARGS)

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean
