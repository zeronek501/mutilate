#include <string>
#include <cstdlib>
#include <cstring>

#include "Cores.h"
#include "Util.h"
#include "Task.h"


using namespace std;

Cores::Cores(int _cmin, int _cmax, Task *_task) : cmin(_cmin), cmax(_cmax), task(_task) {
	string cpus;
	cpus = to_string(cmin) + "-" + to_string(cmax);
	alloc_cpuset(task->cgroup, cpus);
}

void Cores::alloc_cpuset(string cgroup, string value) {
	string cpus_path;
	string mems_path;
	cpus_path = string("/sys/fs/cgroup/cpuset/") + cgroup + string("/cpuset.cpus");
	printf("allocating cpuset cores : %s\n", cpus_path.c_str());
	if(s_write(cpus_path, value)) {
		fprintf(stderr, "alloc_cpuset error\n");
		exit(1);
	}
	mems_path = string("/sys/fs/cgroup/cpuset/") + cgroup + string("/cpuset.mems");
	printf("allocating cpuset mems : %s\n", cpus_path.c_str());
	if(s_write(mems_path, "0")) {
		fprintf(stderr, "alloc_cpuset error\n");
		exit(1);
	}
}

void Cores::add(int amt) {
	string newcpus;
	if(cmin == 0) { // change cmax
		cmax = MIN(CORE_NUM - 1, cmax + amt);
	}
	else { // change cmin
		cmin = MAX(0, cmin - amt);
	}
	newcpus = to_string(cmin) + "-" + to_string(cmax); 
	alloc_cpuset(task->cgroup, newcpus);
}

int Cores::size() {
	return cmax - cmin + 1;
}

void Cores::remove(int amt) {
	if(amt <= 0) return;
	string newcpus;
	if(cmin == 0) { // change cmax
		cmax = MAX(cmin, cmax - amt);
	}
	else { // change cmin
		cmin = MIN(cmax, cmin + amt);
	}
	newcpus = to_string(cmin) + "-" + to_string(cmax);
	alloc_cpuset(task->cgroup, newcpus);
}

string Cores::cpus() {
	string cpus_path;
	string cpus;
	cpus_path = string("/sys/fs/cgroup/cpuset/") + task->cgroup + string("/cpuset.cpus");
	if(s_read(cpus_path, cpus)) {
		fprintf(stderr, "cpus read error\n");
		exit(1);
	}
	return cpus;
}
