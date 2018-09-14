#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>
using namespace std;

int main(int argc, char **argv) {
	FILE *fp = NULL;
	char line[1000];
	string strline;
	double read_bw, write_bw;

	if((fp = popen("sudo nohup ./pcm.x 5 -i=1", "r")) == NULL) {
		return 1;
	}

	while(fgets(line, 1000, fp) != NULL) {
		char marker[4] = "MEM";
		if(!strncmp(line, marker, 3)) {
			fgets(line, 1000, fp);
			fgets(line, 1000, fp);
			printf("%s\n", line);
			break;
		}
	}
	pclose(fp);
	strline = string(line);
	strline = strline.substr(4);
	char *saveptr = NULL;
	char *chunk = NULL;
	string::size_type sz;
	stod(strline, &sz);
	strline = strline.substr(sz);
	read_bw = stod(strline, &sz);
	strline = strline.substr(sz);
	write_bw = stod(strline);
	printf("%f %f\n", read_bw, write_bw);
	/*
	chunk = strtok_r(line, " ", &saveptr);
	chunk = strtok_r(NULL, " ", &saveptr);
	chunk = strtok_r(NULL, " ", &saveptr);
	read_bw = new string(chunk);
	printf("%s\n", read_bw.c_str());
	chunk = strtok_r(NULL, " ", &saveptr);
	write_bw = new string(chunk);
	printf("%s\n", write_bw.c_str());
	*/
	return 0;
}
