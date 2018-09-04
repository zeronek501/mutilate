#include <vector>
#include <string>
#define CORE_NUM 28

using namespace std;

class Cores
{
public:
	int min;
	int max;
	string cgroup;
	Cores(int _min, int _max, string cgroup):min(_min), max(_max), cgroup(_cgroup);
	int size() {
		return max - min + 1;
	}
	void add(int amt) {
	
	}

	void remove(int amt) {

	}
}


