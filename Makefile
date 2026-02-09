CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET = main
SRC = compiler.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run:
	./$(TARGET) $(FILE)

clean:
	rm -f $(TARGET)
