CXX = g++
CXXFLAGS = -std=c++11 -g -pthread
LD = $(CXX)
LDFLAGS = -lpthread

all : Heracles

Heracles : TopLevelController.o CoreMemController.o CPUPowerController.o NetworkController.o Cores.o
	$(LD) $(LDFLAGS) -o $@ $^

TopLevelController.o : TopLevelController.cc CoreMemController.h CPUPowerController.h NetworkController.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

CoreMemController.o : CoreMemController.cc Cores.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

CPUPowerController.o : CPUPowerController.cc Cores.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

NetworkController.o : NetworkController.cc Cores.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean :
	rm -f *.o
	rm -f Heracles
