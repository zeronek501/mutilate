#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <time.h>
#include <cstring>
#include <string>
#include <cstdlib>
#include <assert.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fstream>
#include <iostream>
#include <vector>

#include <vector>

#include "CoreMemController.h"
#include "Task.h"
#include "Util.h"

// libzmq is necessary
#include <zmq.hpp>

#define CAN_GROW 0
#define CANNOT_GROW 1
#define DISABLED 2

#define LOAD_MAX 250000

zmq::context_t *context = NULL;
zmq::socket_t *my_socket = NULL;
std::string port;
int wait_time;

int n = 95;
double nth;
double lat = -1, load = -1;
double qps = -1;
double target_lat = 2000, slack = -1;
CoreMemController *cm;
Task *lc, *be;

int g_be_status;

// pthread
pthread_mutex_t l_mutex; // mutex for latency poll
pthread_cond_t cond;

//  Convert string to 0MQ string and send to socket
static bool s_send (zmq::socket_t &_socket, const std::string &str) {
  zmq::message_t message(str.size());
  memcpy(message.data(), str.data(), str.size());

  return _socket.send(message);
}

void be_unpause() {
	system(std::string("docker unpause inmemory").c_str());
}

void be_pause() {
	std::string cmd;
	int core_num = cm->be_cores->size() - 1; 
	int llc_num = cm->be_llc->size() - 1; 
	if(core_num > 0) {
		cm->be_cores->remove(core_num);
		cm->lc_cores->add(core_num);
	}
	if(llc_num > 0) {
		cm->be_llc->remove(llc_num);
		cm->lc_llc->add(llc_num);
	}
	system(std::string("docker pause inmemory").c_str());
}

void be_exec() {
	printf("executing be\n");
	int pid;
	pid = fork();
	if(pid == 0) { // child process
		int mypid = getpid();		
		s_write("/sys/fs/cgroup/cpuset/be/tasks", std::to_string(mypid));
		system("./setting/set_inmemory_solo.sh");
		execl("/home/janux/mutilate/Heracles/setting/run_inmemory_solo.sh", "run_inmemory_solo.sh", NULL);
	}
	else if(pid == -1) { // fork error
		printf("fork error!\n");
		exit(1);
	}
}


void poll(double &_lat, double &_qps) {
	printf("polling...\n");
	zmq::message_t request;
	my_socket->recv(&request);
	s_send(*my_socket, "ACK");
	memcpy(&_lat, request.data(), sizeof(double));
	my_socket->recv(&request);
	s_send(*my_socket, "ACK");
	memcpy(&_qps, request.data(), sizeof(double));
}

void enable_be() {
	printf("enable_be\n");
	be_unpause();
	g_be_status = CAN_GROW;
}

void disallow_be_growth() {
	printf("disallow_be_growth\n");
	g_be_status = CANNOT_GROW;
}

void disable_be() {
	printf("disable_be\n");
	g_be_status = DISABLED;
	be_pause();
}

void enter_cooldown() {
	printf("cooldown\n");
	sleep(300); // sleep 5 min
}

void lc_init() {
	
	printf("lc_init..\n");

	// create cgroup cos
	s_sudo_cmd("mkdir -p /sys/fs/cgroup/cpuset/lc");
	//s_sudo_cmd("mkdir -p /sys/fs/resctrl/lc");

	// allocate cores and ways 
	lc = new Task("lc", "lc");	

	// fork and exec process and attach the task pids to cgroup and resctrl
	printf("executing lc\n");
	int pid;
	int status;
	pid = fork();
	if(pid == 0) { // child process
		int mypid = getpid();		
		printf("mypid: %d\n", mypid);
		s_write("/sys/fs/cgroup/cpuset/lc/cpuset.cpus", "0");
		s_write("/sys/fs/cgroup/cpuset/lc/cpuset.mems", "0");
		s_write("/sys/fs/cgroup/cpuset/lc/tasks", std::to_string(mypid));
		//s_write("/sys/fs/resctrl/lc/tasks", std::to_string(mypid));
		system("sudo pkill -9 memcached");
		execlp("memcached", "memcached", "-t", "4", "-c", "32768", "-p", "11212", NULL);
	}
	else if(pid == -1) { // fork error
		printf("fork error!\n");
		exit(1);
	}
	else {
		sleep(5);
		std::string value;
		std::fstream fs;
		std::string filepath = std::string("/sys/fs/cgroup/cpuset/lc/tasks");
		fs.open(filepath, std::fstream::in);	
		while(!fs.eof()) {
			fs >> value;
			printf("%s\n",value.c_str());
			lc->pids.push_back(std::stoi(value));
		}
		//waitpid(pid, &status, 0);
	}
}

void be_init() {
	std::string be_cgroup;
	std::fstream fs;
	std::string value;

	printf("be_init..\n");

	// docker run and pause -> cgroup is created based on their cid.
	//s_sudo_cmd("mkdir -p /sys/fs/resctrl/be");
	system(std::string("./setting/set_inmemory_solo.sh").c_str());
	s_read("./cids.txt", be_cgroup);
	printf("be_cgroup: %s\n", be_cgroup.c_str());

	be = new Task(std::string("docker/") + be_cgroup, "be");	

	// attach task pids. copy to resctrl tasks

	std::string filepath = std::string("/sys/fs/cgroup/cpuset/") + be->cgroup + std::string("/tasks");
	std::string resctrlpath = std::string("/sys/fs/resctrl/be/tasks");
	fs.open(filepath, std::fstream::in);	

	while(!fs.eof()) {
		fs >> value;
		printf("%s\n",value.c_str());
		be->pids.push_back(std::stoi(value));
		//s_write(resctrlpath, value);
	}
	fs.close();

	enable_be();
	
}

void *top_control(void *threadid) {
	printf("top_controlling...\n");
	while (true) {
		poll(lat, qps);
		load = qps / LOAD_MAX;
		slack = (target_lat - lat) / target_lat;
		printf("nth lat: %f, slack: %f, qps: %f, load: %f\n", lat, slack, qps, load);
		if(slack < 0) {
			disable_be();	
			enter_cooldown();
		}
		else if(load > 0.85) {
			disable_be();
		}
		else if(load < 0.8) {
			enable_be();
		}
		else if(slack < 0.1) {
			disallow_be_growth();
			if(slack < 0.05) {
				int num = cm->be_cores->size() - 2; 
				if(num > 0) {
					cm->be_cores->remove(num);
					cm->lc_cores->add(num);
				}
			}
		}
		// else: enable_be()?
		pthread_cond_signal(&cond);
		sleep(wait_time);
	}
}

void *core_mem_control(void *threadid) {
	printf("core_mem_controlling...\n");
	pthread_mutex_lock(&l_mutex);
	while(lat == -1) {
		pthread_cond_wait(&cond, &l_mutex);
	}
	pthread_mutex_unlock(&l_mutex);

	printf("core_mem_repeating...\n");
	cm->start_loop();
}
	
void init() {
	printf("initializing...\n");
	int status;

	// Init LC and BE task
	lc_init();
	be_init();

	// Init Controller instances
	cm = new CoreMemController(lc, be, g_be_status, slack, qps); 

	// Init socket settings
	context = new zmq::context_t(1);
	my_socket = new zmq::socket_t(*context, ZMQ_REP);
	port = "10123"; // arbitrarily set
	wait_time = 15;
	my_socket->bind((std::string("tcp://*:") + port).c_str());

	sleep(500);
	
	/*
	// Create pthreads	
	pthread_t top_level, core_mem;
	if(pthread_create(&top_level, NULL, top_control, NULL) < 0) {
		perror("thread create error:");
		exit(0);
	}
	
	if(pthread_create(&core_mem, NULL, core_mem_control, NULL) < 0) {
		perror("thread create error:");
		exit(0);
	}

	pthread_join(core_mem, (void **)&status);
	printf("Core Thread End %d\n", status);
	

	pthread_join(top_level, (void **)&status);
	printf("Top Thread End %d\n", status);
	*/
}

int main(int argc, char **argv) {
	init();
}

	

