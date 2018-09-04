#ifndef CORE_MEM_CONTROLLER_H
#define CORE_MEM_CONTROLLER_H

Class CoreMemController {
public:
	Task *lc_task;
	Task *be_task;
	Cores *lc_cores;
	Cores *be_cores;
	LLC *lc_llc;
	LLC *be_llc;
	CoreMemController(Task *lc_task, Task *be_task);
	void init();
	void start_loop();
}

#endif
