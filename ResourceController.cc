#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>

using namespace std;

int s_write(string filepath, string value) {
	int fd = open(filepath.c_str(), O_WRONLY);
	char *str;
	str = strdup(value.c_str());	
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
	
int s_read(string filepath, string &value) {
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
	value = string(buffer, 0, strlen(buffer)-1);
	close(fd);
	return 0;
}

int inc_cpuset(string cgroup, int rightward) {
	string cpus_path;
	string cpus;
	string new_cpus;
	int pos;
	int new_cpu;
	cpus_path = string("/sys/fs/cgroup/cpuset/")+cgroup+string("/cpuset.cpus");
	if(s_read(cpus_path, cpus)) {
		fprintf(stderr, "inc_cpuset read error\n");
		return 1;
	}
	if(pos = cpus.find("-") != string::npos) {
		if(rightward) 
			new_cpu = atoi(cpus.substr(pos+1).c_str()) + 1;
		else
			new_cpu = atoi(cpus.substr(0,pos).c_str()) - 1;	
	}
	else {
		if(rightward)
			new_cpu = atoi(cpus.c_str()) + 1;
		else
			new_cpu = atoi(cpus.c_str()) - 1;
	}
	new_cpus = cpus + string(", ") + to_string(new_cpu);		

	if(s_write(cpus_path, new_cpus)) {
		fprintf(stderr, "inc_cpuset write error\n");
		return 1;
	}
	return 0;
}	

int dec_cpuset(string cgroup, int rightward) {
	string cpus_path;
	string cpus;
	string new_cpus;
	int pos;
	int new_cpu;
	cpus_path = string("/sys/fs/cgroup/cpuset/")+cgroup+string("/cpuset.cpus");
	if(s_read(cpus_path, cpus)) {
		fprintf(stderr, "dec_cpuset read error\n");
		return 1;
	}
	if(pos = cpus.find("-") != string::npos) {
		if(rightward) {
			new_cpu = atoi(cpus.substr(pos+1).c_str()) - 1;
			new_cpus = cpus.substr(0,pos+1) + to_string(new_cpu);
		}
		else {
			new_cpu = atoi(cpus.substr(0,pos).c_str()) + 1;	
			new_cpus = to_string(new_cpu) + cpus.substr(pos);
		}
		if(s_write(cpus_path, new_cpus)) {
			fprintf(stderr, "dec_cpuset write error\n");
			return 1;
		}
		return 0;
	}
	else {
		fprintf(stderr, "it has already a single cpu\n");
		return 2;
	}
}	

int main(int argc, char **argv) {
	string s;
	s_read("/sys/fs/cgroup/cpuset/test1/cpuset.cpus", s);
	printf("%s\n", s.c_str());
	dec_cpuset("test1", 1);
	dec_cpuset("test1", 0);
	//s_write("/sys/fs/cgroup/cpuset/test1/cpuset.cpus", "0-3");

	s_read("/sys/fs/cgroup/cpuset/test1/cpuset.cpus", s);
	printf("%s\n", s.c_str());
}	
