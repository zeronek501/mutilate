#ifndef TASK_H
#define TASK_H

#include <vector>
#include <string>

using namespace std;

class Task {
	vector<int> pids;
	string cgroup;
	string resctrl;
	Task(vector<int> _pids, _cgroup, _resctrl) : pids(_pids), cgroup(_cgroup), resctrl(_resctrl)
	{
	}
}

#endif
