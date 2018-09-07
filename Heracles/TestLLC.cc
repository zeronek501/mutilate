#include <iostream>
#include <string>
#include "LLC.h"
#include "Task.h"

using namespace std;

int main() {
	Task* lc_task = new Task("lc", "lc");
	LLC* lc_llc = new LLC(0, 1, lc_task);
	cout<< "calc_ways:" << lc_llc->calc_ways() << endl;
	cout<< "initial ways:" << lc_llc->read_ways() <<"\n";
	cout<< "add ways" << endl;
	lc_llc->add(1);
	cout<< lc_llc->read_ways() << endl;
	cout<< "remove ways" << endl;
	lc_llc->remove(1);
	cout<< lc_llc->read_ways() << endl;
}

