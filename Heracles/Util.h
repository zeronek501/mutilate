#ifndef UTIL_H
#define UTIL_H

#define MAX(a, b) (((a)>(b))?(a):(b));
#define MIN(a, b) (((a)<(b))?(a):(b));

#include <string>

int s_write(std::string filepath, std::string value);
int s_read(std::string filepath, std::string &value);
int s_sudo_cmd(std::string cmd);

#endif
