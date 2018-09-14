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
#define NO_GROW 2

// BE status
#define CAN_GROW 0
#define CANNOT_GROW 1
#define DISABLED 2

#define DRAM_LIMIT 34.56 // 0.9 * 38.4GB/s

using namespace std;

CoreMemController::CoreMemController(Task *_lc_task, Task *_be_task, int &_be_status, double &_slack, double &_qps) : lc_task(_lc_task), be_task(_be_task), be_status(_be_status), slack(_slack), qps(_qps)  {
	lc_cores = new Cores(0, CORE_NUM - 2, lc_task);
	be_cores = new Cores(CORE_NUM - 1, CORE_NUM - 1, be_task);
	lc_llc = new LLC(0, LLC_WAYS - 3, lc_task);
	be_llc = new LLC(LLC_WAYS - 2, LLC_WAYS - 1, be_task);
	total_bw = 0;
	be_bw = 0;
	prev_be_bw = 0;
	total_bw_deriv = 0;
	be_bw_core_deriv = 1000;
	prev_grow_status = NO_GROW;
	for(int i=0;i<30;i++) {
		offline_model[i] = 0.1 * i; // temporary model;
	}
}
CoreMemController::~CoreMemController() {
	delete lc_cores;
	delete be_cores;
	delete lc_llc;
	delete be_llc;
}
double CoreMemController::lc_bw_model() {
	return offline_model[(int)(qps / 10000)];
}

void CoreMemController::measure_dram_bw() {
	FILE *fp = NULL;
	char line[1000];
	string strline;
	double read_bw, write_bw;

	if((fp = popen("sudo nohup ./pcm.x 2 -i=1", "r")) == NULL) {
		printf("popen error\n");
		exit(1);
	}

	while(fgets(line, 1000, fp) != NULL) {
		char marker[4] = "MEM";
		if(!strncmp(line, marker, 3)) {
			fgets(line, 1000, fp);
			fgets(line, 1000, fp);
			printf("%s\n", line);
			break;
		}
	}
	pclose(fp);
	strline = string(line);
	strline = strline.substr(4);
	char *saveptr = NULL;
	char *chunk = NULL;
	string::size_type sz;
	stod(strline, &sz);
	strline = strline.substr(sz);
	read_bw = stod(strline, &sz);
	strline = strline.substr(sz);
	write_bw = stod(strline);

	prev_total_bw = total_bw;
	total_bw = (read_bw + write_bw) / 2; // pcm.x measured time = 2
	prev_be_bw = be_bw;
	be_bw = total_bw - lc_bw_model();

	if(prev_grow_status == GROW_LLC || prev_grow_status == GROW_CORES) {
		total_bw_deriv = (total_bw_deriv + (total_bw - prev_total_bw)) / 2;
	}
	else if(prev_grow_status == GROW_CORES) {
		be_bw_core_deriv = (be_bw - prev_be_bw);
	}
	prev_grow_status = NO_GROW;
}

void CoreMemController::grow_llc_for_be() {
	be_llc->add(1);
	lc_llc->remove(1);
	prev_grow_status = GROW_LLC;
}

void CoreMemController::remove_llc_for_be() {
	be_llc->remove(1);
	lc_llc->add(1);
}

void CoreMemController::set_be(Task* _be) {
	be_task = _be;
}

void CoreMemController::start_loop() {
	int local_be_status;
	double overage;

	while(true) {
		local_be_status = be_status;
		if(local_be_status == DISABLED) {
			break;
		}
		else {
			measure_dram_bw();
			if(total_bw > DRAM_LIMIT) {
				overage = total_bw - DRAM_LIMIT;
				be_cores->remove(overage/be_bw_core_deriv);
				continue; 
			}
			if(local_be_status == CANNOT_GROW) {
				continue; 
			}
			//local_be_status == CAN_GROW
			if(grow_status == GROW_LLC) {
				if(total_bw + total_bw_deriv > DRAM_LIMIT) {
					grow_status = GROW_CORES;	
				}
				else {
					grow_llc_for_be();
					measure_dram_bw();
					if(total_bw_deriv >= 0) {
						remove_llc_for_be();
						grow_status = GROW_CORES;
					}
					if(be_bw - total_bw < 0) {
						grow_status = GROW_CORES;
					}
				}
			}
			else if(grow_status == GROW_CORES) {
				double needed = total_bw + be_bw_core_deriv;
				if(needed > DRAM_LIMIT) {
					grow_status = GROW_LLC;
				}
				else if(slack > 0.1) {
					be_cores->add(1);
					lc_cores->remove(1);
					prev_grow_status = GROW_CORES;
				}
			}
			sleep(2);
		}
	}
}
	

