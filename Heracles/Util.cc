
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
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

	std::string mycmd;
	int res;
	res = s_sudo_cmd("/bin/echo " + value + " > " + filepath);
	return res;
	
}

int s_read(std::string filepath, std::string &value) {
	std::fstream fs;
	fs.open(filepath, std::fstream::in);	
	fs >> value;
	fs.close();
	return 0;
}

int s_sudo_cmd(std::string cmd) {
	std::string mycmd;
	int res;
	mycmd = "sudo su -c \"" + cmd + "\"";
	res = system(mycmd.c_str());

	return res;
}


