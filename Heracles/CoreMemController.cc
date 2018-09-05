#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>
#include <pthread.h>

#include "CoreMemController.h"
#include "Cores.h"
#include "LLC.h"
#include "Task.h"

// grow status
#define GROW_CORES 0
#define GROW_LLC 1

// BE status
#define CAN_GROW 0
#define CANNOT_GROW 1
#define DISABLED 2

#define DRAM_LIMIT 10000 // arbitrary value

using namespace std;

CoreMemController::CoreMemController(Task *_lc_task, Task *_be_task, pthread_mutex_t &_mutex, int &_be_status) : lc_task(_lc_task), be_task(_be_task), mutex(_mutex), be_status(_be_status)  {
	lc_cores = new Cores(0, 13, lc_task);
	be_cores = new Cores(14, 27, be_task);
	lc_llc = new LLC(0, 9, lc_task);
	be_llc = new LLC(10, 19, be_task);
}
CoreMemController::~CoreMemController() {
	delete lc_cores;
	delete be_cores;
	delete lc_llc;
	delete be_llc;
}
double CoreMemController::measure_dram_bw() {
	return 10000;
}

void CoreMemController::start_loop() {
	int local_be_status;
	double overage;

	while(true) {
		pthread_mutex_lock(&mutex);
		local_be_status = be_status;
		pthread_mutex_unlock(&mutex);
		if(local_be_status == DISABLED) {
			//disable_be()
		}
		else {
			total_bw = measure_dram_bw();
			if(total_bw > DRAM_LIMIT) {
				overage = total_bw - DRAM_LIMIT;
				be_cores->remove(1);
				continue; // FIXME: sleep(2) might be necessary here
			}
			if(local_be_status == CANNOT_GROW) {
				continue; // FIXME: sleep(2) might be necessary here
			}
			//local_be_status == CAN_GROW
			if(grow_status == GROW_LLC) {
				
			}
			else if(grow_status == GROW_CORES) {

			}
			sleep(2);
		}
	}
}
	

