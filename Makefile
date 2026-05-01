CXX = c++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
LDFLAGS = 

SRCS = main.cpp terminal.cpp fs.cpp panel.cpp 
OBJS = $(SRCS:.cpp=.o)
TARGET = fileman

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

