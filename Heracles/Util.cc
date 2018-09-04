
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include "Util.h"

int s_write(std::string filepath, std::string value) {
	int fd = open(filepath.c_str(), O_WRONLY);
	char *str;
	value.copy(str, value.length(), 0); // string copy
	if(fd == NULL) {
		fprintf(stderr, "Unable to open path for writing\n");
		return 1;
	}
	if(write(fd, str, value.length()) < 0) {
		fprintf(stderr, "cgroup write failure\n");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}
	
int s_read(std::string filepath, std::string &value) {
	int fd = open(filepath.c_str(), O_RDONLY);
	char buffer[100];
	if(fd == NULL) {
		fprintf(stderr, "Unable to open path for writing\n");
		return 1;
	}
	
	if(read(fd, buffer, 100) < 0) {
		fprintf(stderr, "cgroup read failure\n");
		close(fd);
		return -1;
	}
	value = std::string(buffer, 0, strlen(buffer)-1);
	close(fd);
	return 0;
}

