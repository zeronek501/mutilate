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

zmq::context_t context(1);
zmq::socket_t my_socket(context, ZMQ_REP);
std::string port;
int wait_time;

int n = 95;
double nth;
double lat = -1, load = -1;
double target_lat = 100, slack = -1;
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
		cmd = std::string("sudo kill -CONT ") + std::to_string(pid); // FIXME: preprocess pid string later
		system(cmd.c_str());
	}
}

void be_kill() {
	std::string cmd;

	for(int pid : be->pids) {
		cmd = std::string("sudo kill -STOP ") + std::to_string(pid); // FIXME: preprocess pid string later
		system(cmd.c_str());
	}
}

void poll(double &_lat, double &_load) {
	printf("polling...\n");
	zmq::message_t request;
	my_socket.recv(&request);
	s_send(my_socket, "ACK");
	memcpy(&_lat, request.data(), sizeof(double));
	my_socket.recv(&request);
	s_send(my_socket, "ACK");
	memcpy(&_load, request.data(), sizeof(double));
	
	printf("measured nth latency is %f\n", _lat);
	printf("measured load is %f\n", _load);
}

void enable_be() {
	printf("enable_be\n");
	be_exec();
	cm->set_be(be);
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
	delete be;
}

void enter_cooldown() {
	printf("cooldown\n");
	sleep(300); // sleep 5 min
}

void *top_control(void *threadid) {
	printf("top_controlling...\n");
	while (true) {
		poll(lat, load);
		printf("lat: %f, qps: %f\n", lat, load);
		slack = (target_lat - lat) / target_lat;
		printf("target_lat: %f, slack: %f\n", target_lat, slack);
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
	cm = new CoreMemController(lc, be, g_be_status, slack, load); 

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
	/*
	if(pthread_create(&core_mem, NULL, core_mem_control, NULL) < 0) {
		perror("thread create error:");
		exit(0);
	}

	pthread_join(core_mem, (void **)&status);
	printf("Core Thread End %d\n", status);
	*/

	pthread_join(top_level, (void **)&status);
	printf("Top Thread End %d\n", status);
}

int main(int argc, char **argv) {
	init();
}

	

