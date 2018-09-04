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

#include "config.h"
#include "Connection.h"
#include "CoreMemController.h"
#include "Task.h"

// libzmq is necessary
#include <zmq.hpp>

#define CAN_GROW 0
#define CANNOT_GROW 1
#define DISABLED 2

int n = 95;
double nth;
double lat = -1, load = -1;
double target_lat = 100, slack = -1;
CoreMemController *cm;
Task *lc, *be;

int g_be_status = CAN_GROW;

// pthread
pthread_mutex_t l_mutex; // mutex for latency poll
pthread_cond_t cond;
pthread_mutex_t s_mutex; // mutex for status poll

//  Convert string to 0MQ string and send to socket
static bool s_send (zmq::socket_t &socket, const std::string &str) {
  zmq::message_t message(str.size());
  memcpy(message.data(), str.data(), str.size());

  return socket.send(message);
}

void init() {
	int status;
	// Init LC and BE task
	lc = new Task("lc", "lc");	
	be = new Task("be", "be");	
	
	// Init Controller instances
	cm = new CoreMemController(lc, be, s_mutex, g_be_status); 

	// Init socket settings
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);
	std::string port;
	int wait_time;
	port = "10123"; // arbitrarily set
	wait_time = 15;
	socket.bind((std::string("tcp://*:") + port).c_str());
	
	// Create pthreads	
	pthread_t top_level, core_mem;
	if(pthread_create(&top_level, NULL, top_control, NULL) < 0) {
		perror("thread create error:")
		exit(0);
	}
	if(pthread_create(&core_mem, NULL, core_mem_control, NULL) < 0) {
		perror("thread create error:")
		exit(0);
	}
	
	pthread_join(top_level, (void **)&status);
	printf("Top Thread End %d\n", status);
	
	pthread_join(core_mem, (void **)&status);
	printf("Core Thread End %d\n", status);

}

void poll(double &_lat, double &_load) {
	zmq::message_t request;
	socket.recv(&request);
	s_send(socket, "ACK");
	memcpy(&_lat, request.data(), sizeof(double));
	socket.recv(&request);
	s_send(socket, "ACK");
	memcpy(&_load, request.data(), sizeof(double));
	
	printf("measured nth latency is %f\n", _lat);
	printf("measured load is %f\n", _load);
}

void enable_be() {
	pthread_mutex_lock(&s_mutex);
	g_be_status = CAN_GROW;
	pthread_mutex_unlock(&s_mutex);
}

void disallow_be_growth() {
	pthread_mutex_lock(&s_mutex);
	g_be_status = CANNOT_GROW;
	pthread_mutex_unlock(&s_mutex);
}

void disable_be() {
	pthread_mutex_lock(&s_mutex);
	g_be_status = DISABLED;
	pthread_mutex_unlock(&s_mutex);
}

void top_control() {
	while (true) {
		poll(lat, load);
		slack = (target_lat - lat) / target_lat;
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
				cm->be_cores->remove(cm->be_cores->size() - 2);
			}
		}
		sleep(wait_time);
	}
}

void core_mem_control() {
	pthread_mutex_lock(&l_mutex);
	while(lat == -1) {
		pthread_cond_wait(&cond, &l_mutex);
	}
	pthread_mutex_unlock(&l_mutex);

	cm->start_loop();
}
	
int main(int argc, char **argv) {
	init();
}

	

