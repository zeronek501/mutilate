#ifndef CORE_MEM_CONTROLLER_H
#define CORE_MEM_CONTROLLER_H

#include "Task.h"
#include "Cores.h"
#include "LLC.h"
#include <pthread.h>

class CoreMemController {
public:
	Task *lc_task;
	Task *be_task;
	Cores *lc_cores;
	Cores *be_cores;
	LLC *lc_llc;
	LLC *be_llc;
	pthread_mutex_t &mutex;
	int &be_status;
	int grow_status;
	double total_bw;
	CoreMemController(Task *_lc_task, Task *_be_task, pthread_mutex_t &_mutex, int &_be_status);
	~CoreMemController();
	void init();
	void start_loop();
	double measure_dram_bw();
};

#endif
