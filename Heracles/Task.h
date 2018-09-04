#ifndef TASK_H
#define TASK_H

#include <vector>
#include <string>

class Task {
	std::vector<int> *pids;
	std::string cgroup;
	std::string cos; // cos: class of service
	Task(std::string _cgroup, std::string _cos) : cgroup(_cgroup), cos(_cos)
	{
	}
};

#endif
