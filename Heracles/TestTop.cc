#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Util.h"

using namespace std;

int main(int argc, char **argv) {
	printf("executing lc\n");
	int pid;
	int status;
	pid = fork();
	if(pid == 0) { // child process
		int mypid = getpid();		
		printf("mypid: %d\n", mypid);
		//s_write("/sys/fs/cgroup/cpuset/lc/tasks", std::to_string(mypid));
		//s_write("/sys/fs/resctrl/lc/tasks", std::to_string(mypid));
		execlp("memcached", "memcached", "-t", "4", "-c", "32768", "-p", "11212", NULL);
	}
	else if(pid == -1) { // fork error
		printf("fork error!\n");
		exit(1);
	}
	else {
	}
}
