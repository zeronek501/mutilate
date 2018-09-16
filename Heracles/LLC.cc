
#include <iostream>
#include <string>
#include <sstream>

#include "LLC.h"
#include "Util.h"
#include "Task.h"



using namespace std;

LLC::LLC(int _wmin, int _wmax, Task * _task) : wmin(_wmin), wmax(_wmax), task(_task) {
	string ways;
	ways = string("\x4c\x33\x3a\x30\x3d") + calc_ways();
	alloc_ways(task->cos, ways);
}

void LLC::alloc_ways(string cos, string value) { 
	string ways_path;
	ways_path = string("/sys/fs/resctrl/") + cos + string("/schemata");
	if(s_write(ways_path, value)) {
		fprintf(stderr, "alloc_ways error\n");
		exit(1);
	}
}

void LLC::add(int amt) {
	string newways;
	if(wmin == 0) { // change wmax
		wmax = MIN(LLC_WAYS, wmax + amt);
	}
	else { // change wmin
		wmin = MAX(0, wmin - amt);
	}
	newways = string("L3:0=") + calc_ways();
	alloc_ways(task->cos, newways);
}

int LLC::size() {
	return wmax - wmin + 1;
}

void LLC::remove(int amt) {
	string newways;
	if(wmin == 0) { // change wmax
		wmax = MAX(wmin, wmax - amt);
	}
	else { // change wmin
		wmin = MIN(wmax, wmin + amt);
	}
	newways = string("L3:0=") + calc_ways();
	alloc_ways(task->cos, newways);
}

string LLC::read_ways() {
	string ways_path;
	string ways;
	ways_path = string("/sys/fs/resctrl/") + task->cos + string("/schemata");
	if(s_read(ways_path, ways)) {
		fprintf(stderr, "ways read error\n");
		exit(1);
	}
	return ways;
}
string LLC::calc_ways() {
	int bits = 0, i;
	for(i = 0;i < LLC_WAYS; i++) { 
		bits*=2;
		if(i >= wmin && i <= wmax) {
			bits += 1;
		}
	}
	stringstream sstream;
	sstream << std::hex << bits;
	return sstream.str();
}


