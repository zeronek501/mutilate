#ifndef CORES_H
#define CORES_H

#include <string>
#include "Task.h"

class Cores {
public:
	int cmin;
	int cmax;
	Task *task;
	Cores(int _cmin, int _cmax, Task *_task);
	void add(int amt);
	void remove(int amt);
	int size();
	std::string cpus();

private:
	void alloc_cpuset(std::string cgroup, std::string value);
}
#endif
