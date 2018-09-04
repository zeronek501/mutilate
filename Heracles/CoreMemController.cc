#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>

#include "CoreMemController.h"
#include "Cores.h"

#define GROW_CORES 0
#define GROW_LLC 1

using namespace std;

CoreMemController::CoreMemController(Task *_lc_task, Task *_be_task) : lc_task(_lc_task), be_task(_be_task) {
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

CoreMemController::init() {
}
	
