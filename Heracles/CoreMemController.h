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
	int prev_grow_status;
	double prev_be_bw;
	double total_bw_deriv;
	double be_bw_core_deriv;
	int &be_status;
	int grow_status;
	double &slack;
	double &qps;
	double prev_total_bw;
	double total_bw;
	double be_bw;
	double offline_model[30];
	CoreMemController(Task *_lc_task, Task *_be_task, int &_be_status, double &_slack, double &_qps);
	~CoreMemController();
	void init();
	void start_loop();
	void measure_dram_bw();
	void grow_llc_for_be();
	void remove_llc_for_be();
	void set_be(Task* _be);
	double lc_bw_model();
};

#endif
