BUILD_TYPE=-g3
# BUILD_TYPE=-O3 -mtune=native -march=native


INCLUDE=-I/home/nakira/boost/include/
LIBRARY=-L/home/nakira/boost/lib/ -L/opt/rh/devtoolset-8/root/usr/lib/gcc/x86_64-redhat-linux/8/

CXX=g++
CXXFLAGS=-c -std=c++17 $(INCLUDE) -I./toml11/ $(BUILD_TYPE)
LDFLAGS=-static -lstdc++ -lpthread $(LIBRARY) -lboost_date_time -lboost_program_options -lboost_regex
SOURCES=Source.cpp Executor.cpp ExecutorLinux.cpp EnvironmentVariable.cpp EnvironmentVariableLinux.cpp VCS.cpp Mercurial.cpp Replacer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=suji

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

.cpp:.o
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm $(EXECUTABLE) $(OBJECTS)

