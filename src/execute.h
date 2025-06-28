#ifndef EXECUTE_H
#define EXECUTE_H

#include "util.h"

const char* get_basename(const char* path);
void execute_command_with_args(const char *cmdPath, const char *input);

#endif 