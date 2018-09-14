#ifndef LLC_H
#define LLC_H

#include <string>
#include "Task.h"

#define LLC_WAYS 20

class LLC {
public:
	int cmin;
	int cmax;
	Task *task;
	LLC(int _cmin, int _cmax, Task *_task);
	void add(int amt);
	void remove(int amt);
	std::string calc_ways();
	std::string read_ways();

private:
	void alloc_ways(std::string cos, std::string value);
};
#endif
