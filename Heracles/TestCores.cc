#include <iostream>
#include <string>
#include "Cores.h"
#include "Task.h"

using namespace std;

int main() {
	Task* lc_task = new Task("lc", "lc");
	Cores* lc_cores = new Cores(0, 1, lc_task);
	cout<< "initial cores:" << lc_cores->cpus() << endl;
	cout<< "add cores" << endl;
	lc_cores->add(1);
	cout<< lc_cores->cpus() << endl;
	cout<< "remove cores" << endl;
	lc_cores->remove(1);
	cout<< lc_cores->cpus() << endl;
		
}

