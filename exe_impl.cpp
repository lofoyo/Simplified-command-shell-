#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
//The exe_cmd function is to execute commands with full pathname
int exe_cmd(std::string & cmd, char **& inputargv, char **& envp) {
  int i = 0;
  int in_pos = 0;
  int out_pos = 0;
  int err_pos = 0;
  int input_mark = 0;
  int output_mark = 0;
  int error_mark = 0;
  for (; inputargv[i] != NULL; i++) {
    if (strcmp("<", inputargv[i]) == 0) {
      input_mark = 1;
      in_pos = i;
    }
    if (strcmp(">", inputargv[i]) == 0) {
      output_mark = 1;
      out_pos = i;
    }
    if (strcmp("2>", inputargv[i]) == 0) {
      error_mark = 1;
      err_pos = i;
    }
  }
  if (input_mark) {
    close(0);
    inputargv[in_pos] = NULL;
    int fd = open(inputargv[in_pos + 1], O_RDONLY, 0666);
    if (fd == -1) {
      perror("Cannot open file");
      return -1;
    }
    inputargv[in_pos + 1] = NULL;
    dup2(0, fd);  //replace stdin with fd
  }
  if (output_mark) {
    inputargv[out_pos] = NULL;
    close(1);
    int fd = open(inputargv[out_pos + 1],
                  O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
                  0666);
    inputargv[out_pos + 1] = NULL;
    dup2(1, fd);  //replace stdout with fd
  }
  if (error_mark) {
    inputargv[err_pos] = NULL;
    close(2);
    int fd = open(inputargv[err_pos + 1],
                  O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    inputargv[err_pos + 1] = NULL;
    dup2(2, fd);  //replace stderr with fd
  }
  if (input_mark || output_mark || error_mark) {
    int j = 0;
    for (j = 0; j < i; j++) {
      int k = j + 1;
      if (inputargv[j] == NULL) {
        for (k = j + 1; k < i; k++) {
          if (inputargv[k] != NULL) {
            inputargv[j] = inputargv[k];
            inputargv[k] = NULL;
            break;
          }
        }
        if (k == i - 1) {
          break;
        }
      }
    }
  }  //remove the redirection part from the arguments and then execute the command
  if (execve(cmd.c_str(), inputargv, envp) == -1) {
    perror("Cannot execute");
    return -1;
  }
  return 1;
}
