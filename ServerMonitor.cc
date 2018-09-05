#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <arpa/inet.h>

#include <vector>

#include "config.h"
#include "Connection.h"

#ifdef HAVE_LIBZMQ
#include <zmq.hpp>
#endif

int n = 95;
double nth;
double lat;

//  Convert string to 0MQ string and send to socket
static bool s_send (zmq::socket_t &socket, const std::string &string) {
  zmq::message_t message(string.size());
  memcpy(message.data(), string.data(), string.size());

  return socket.send(message);
}

int main(int argc, char **argv) {
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);
	string port, wait_time;
	if(argc < 2) {
		printf("Port number is required.\n");
		printf("Default port is 10123.\n");
		port = "10123";
		//exit(1);
	}
	else {
		port = string(argv[1]);
	}
	if(argc < 3) {
		printf("Wait time is required.\n");
		printf("Default wait time is 5sec.\n");
		wait_time = "5";
		//exit(1);
	}
	else {
		wait_time = string(argv[2]);
	}
	socket.bind((string("tcp://*:")+port).c_str());
	
	while (true) {
		double lat;
		double qps;

		zmq::message_t request;
		zmq::message_t request2;

		socket.recv(&request);

		s_send(socket, "ACK");
		
		memcpy(&lat, request.data(), sizeof(double));
		
		printf("measured nth latency is %f\n", lat);
		
		socket.recv(&request2);

		s_send(socket, "ACK");
		
		memcpy(&qps, request2.data(), sizeof(double));
		
		printf("measured qps is %f\n", qps);

		sleep(atoi(wait_time.c_str()));
	}
}

