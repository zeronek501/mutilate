#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include <cstring>
#include <string>
#include <cstdlib>
#include <assert.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <vector>

#include "CoreMemController.h"
#include "Task.h"

// libzmq is necessary
#include <zmq.hpp>

#define CAN_GROW 0
#define CANNOT_GROW 1
#define DISABLED 2

#define LOAD_MAX 250000

zmq::context_t context(1);
zmq::socket_t my_socket(context, ZMQ_REP);
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

void be_exec() {
	std::string cmd;
	for(int pid : be->pids) {
		printf("continuing pid : %d\n", pid);
		cmd = std::string("sudo kill -CONT ") + std::to_string(pid); // FIXME: preprocess pid string later
		system(cmd.c_str());
	}
}

void be_kill() {
	std::string cmd;
	for(int pid : be->pids) {
		printf("stopping pid : %d\n", pid);
		cmd = std::string("sudo kill -STOP ") + std::to_string(pid); // FIXME: preprocess pid string later
		system(cmd.c_str());
	}
}

void poll(double &_lat, double &_qps) {
	printf("polling...\n");
	zmq::message_t request;
	my_socket.recv(&request);
	s_send(my_socket, "ACK");
	memcpy(&_lat, request.data(), sizeof(double));
	my_socket.recv(&request);
	s_send(my_socket, "ACK");
	memcpy(&_qps, request.data(), sizeof(double));
	
}

void enable_be() {
	printf("enable_be\n");
	be_exec();
	g_be_status = CAN_GROW;
}

void disallow_be_growth() {
	printf("disallow_be_growth\n");
	g_be_status = CANNOT_GROW;
}

void disable_be() {
	printf("disable_be\n");
	g_be_status = DISABLED;
	be_kill();
}

void enter_cooldown() {
	printf("cooldown\n");
	sleep(300); // sleep 5 min
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
				cm->be_cores->remove(num);
				cm->lc_cores->add(num);
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

	g_be_status = CAN_GROW;

	// Init LC and BE task
	lc = new Task("lc", "lc");	
	be = new Task("be", "be");	
	
	// Init Controller instances
	cm = new CoreMemController(lc, be, g_be_status, slack, qps); 

	// Init socket settings
	port = "10123"; // arbitrarily set
	wait_time = 15;
	my_socket.bind((std::string("tcp://*:") + port).c_str());
	
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
}

int main(int argc, char **argv) {
	init();
}

	

