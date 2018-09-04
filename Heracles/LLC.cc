
#include <string>
#include <cstdlib>

#include "LLC.h"
#include "Util.h"
#include "Task.h"

#define LLC_WAYS 20

using namespace std;

LLC::LLC(int _cmin, int _cmax, Task * _task) : cmin(_cmin), cmax(_cmax), task(_task) {
	string ways;
	ways = string("L3:0=") + calc_ways();
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
	if(cmin == 0) // change cmax
		cmax = MIN(CORE_NUM, cmax + amt);
	else // change cmin
		cmin = MAX(0, cmin - amt);
	newways = string("L3:0=") + calc_ways();
	alloc_ways(task->cos, newways);
}

void LLC::remove(int amt) {
	string newcpus;
	if(cmin == 0) // change cmax
		cmax = MAX(cmin, cmax - amt);
	else // change cmin
		cmin = MIN(cmax, cmin + amt);
	newways = string("L3:0=") + calc_ways();
	alloc_ways(task->cos, newways);
}

string LLC::read_ways() {
	string cpus_path;
	string cpus;
	cpus_path = string("/sys/fs/resctrl/") + cgroup + string("/schemata");
	if(s_read(cpus_path, cpus)) {
		fprintf(stderr, "ways read error\n");
		exit(1);
	}
	return cpus;
}
string LLC::calc_ways() {
	int bits = 0, i;
	char buffer[10];
	for(i = 0;i < LLC_WAYS; i++) { 
		if(i >= cmin && i <= cmax) {
			bits += 1;
		}
		bits << 1;
	}
	itoa(bits, buffer, 16);
	return string(buffer);
}


