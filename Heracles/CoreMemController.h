#ifndef CORE_MEM_CONTROLLER_H
#define CORE_MEM_CONTROLLER_H

#include "Task.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define CORE_NUM 28

using namespace std;

int s_write(string filepath, string value);

int s_read(string filepath, string &value);

class Cores {
public:
	int cmin;
	int cmax;
	Task *task;
	void add(int amt);
	void remove(int amt);
	string cpus();

private:
	void alloc_cpuset(string cgroup, string value);
}


void start_loop();

/*				
int main(int argc, char **argv) {
	string s;
	s_read("/sys/fs/cgroup/cpuset/test1/cpuset.cpus", s);
	printf("%s\n", s.c_str());
	dec_cpuset("test1", 1);
	dec_cpuset("test1", 0);
	//s_write("/sys/fs/cgroup/cpuset/test1/cpuset.cpus", "0-3");

	s_read("/sys/fs/cgroup/cpuset/test1/cpuset.cpus", s);
	printf("%s\n", s.c_str());
}	
*/

#endif
