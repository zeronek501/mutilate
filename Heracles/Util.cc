
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include "Util.h"

int s_write(std::string filepath, std::string value) {
	try {
		std::cout << filepath << " " << value << std::endl;
		std::fstream fs;
		fs.open(filepath, std::fstream::out);	
		fs << value;
		fs.close();
	}
	catch(std::ios_base::failure& e) {
		std::cerr << e.what() << std::endl;
	}
	/*
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
	*/
	return 0;
}
	
int s_read(std::string filepath, std::string &value) {
	std::cout << filepath << " " << value << std::endl;
	std::fstream fs;
	fs.open(filepath, std::fstream::in);	
	fs >> value;
	fs.close();
	return 0;
}

