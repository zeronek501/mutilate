
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include "Util.h"

int s_write(std::string filepath, std::string value) {
	/*
	try {
		std::cout << "filepath: " << filepath << " value: " << value << std::endl;
		std::fstream fs;
		fs.open(filepath, std::fstream::out);	
		fs << value;
		fs.close();
	}
	catch(std::ios_base::failure& e) {
		std::cerr << "error: " << e.what() << std::endl;
	}
	*/

	//int fd = open(filepath.c_str(), O_WRONLY);
	FILE* fd = fopen(filepath.c_str(), "w");
	printf("filepath: %s\n", filepath.c_str());
	char *str;
	//value.copy(str, value.length(), 0); // string copy
	//str = strdup(value.c_str());
	if(fd == NULL) {
		fprintf(stderr, "Unable to open path for writing\n");
		return 1;
	}
	//printf("string: %s\n", str);
	//fprintf(stdout, "before failure%d\n", errno);
	fprintf(fd, "%s", value.c_str());
	/*
	if(write(fd, str, value.length()) < 0) {
		fprintf(stderr, "write failure%d\n", errno);
		close(fd);
		return -1;
	}
	*/
	//close(fd);
	fclose(fd);
	return 0;
}
	
int s_read(std::string filepath, std::string &value) {
	std::fstream fs;
	fs.open(filepath, std::fstream::in);	
	fs >> value;
	fs.close();
	return 0;
}

