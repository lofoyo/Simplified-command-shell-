#include "myShell.h"
//The check_redir() function is to check whether redirections in input arguments are valid or not
int myShell::check_redir() {
  int i = 0;
  int in_pos = 0;
  int out_pos = 0;
  int err_pos = 0;
  int input_mark = 0;
  int output_mark = 0;
  int error_mark = 0;
  for (; argv[i] != NULL; i++) {
    if (strcmp("<", argv[i]) == 0) {  //input redirection mark found
      input_mark++;
      in_pos = i;
    }
    if (strcmp(">", argv[i]) == 0) {  //output redirection mark found
      output_mark++;
      out_pos = i;
    }
    if (strcmp("2>", argv[i]) == 0) {  //errout redirection mark found
      error_mark++;
      err_pos = i;
    }
  }
  //more than one redirection mark of the same type, such as > out1 > out2, is not illegal in my Shell; the redirection mark should be followed by a filename
  if (input_mark || output_mark || error_mark) {
    if (input_mark > 1 || output_mark > 1 || error_mark > 1) {
      printf("Syntax error: More than one redirection mark\n");
      return -1;
    }
    if (argv[in_pos + 1] == NULL || argv[out_pos + 1] == NULL || argv[err_pos + 1] == NULL) {
      printf("Syntax error: Filename needed for redirections\n");
      return -1;
    }
  }
  if (input_mark) {  //check the input file is accessible and readable
    if (access(argv[in_pos + 1], F_OK) == -1 || access(argv[in_pos + 1], R_OK) == -1) {
      perror("Input file error");
      return -1;
    }
  }
  if (output_mark || error_mark) {
    if ((access(argv[out_pos + 1], F_OK) == 0 && access(argv[out_pos + 1], W_OK) == -1) ||
        (access(argv[err_pos + 1], F_OK) == 0 && access(argv[err_pos + 1], W_OK) == -1)) {
      perror("Output file error");  //check the output file is accessible and writable
      return -1;
    }
  }
  return 0;
}
//The check_pipes() function is to check whether pipe marks in input arguments are valid or not; and divides arguments into process arguments array according to pipe marks
//For the processes in pipeline, only the first process can have input redirection, and only the last process can have output redirection
int myShell::check_pipes() {
  process_cnt = 1;
  int i = 0;
  std::vector<int> pos;
  for (; argv[i] != NULL; i++) {
    if (strcmp("|", argv[i]) == 0) {
      argv[i] = NULL;
      pos.push_back(i);  //record the position of "|"
      process_cnt++;
    }
  }
  if (proc_argv != NULL) {
    delete[] proc_argv;
    proc_argv = NULL;
  }  //free the proc_argv array of last command line
  proc_argv = new char **[process_cnt];
  proc_argv[0] = argv;
  int j = 1;
  int index = 0;
  for (j = 1; j < process_cnt; j++) {
    index = pos[j - 1];
    if (argv[index + 1] == NULL) {
      std::cout << "Syntax error: Invalid pipes input" << std::endl;
      return -1;
    }  //divide process arguments according to "|"
    proc_argv[j] = argv + index + 1;
  }
  if (process_cnt > 1) {  //check redirections in pipes
    for (char ** ptr = proc_argv[0]; *ptr != NULL; ptr++) {
      if (strcmp(">", *ptr) == 0 || strcmp("2>", *ptr) == 0) {
        std::cout << "Syntax error: The first process cannot have output redirection" << std::endl;
        return -1;
      }
    }
    for (char ** ptr = proc_argv[process_cnt - 1]; *ptr != NULL; ptr++) {
      if (strcmp("<", *ptr) == 0) {
        std::cout << "Syntax error: The last process cannot have input redirection" << std::endl;
        return -1;
      }
    }
    for (int i = 1; i < process_cnt - 1; i++) {
      for (char ** ptr = proc_argv[i]; *ptr != NULL; ptr++) {
        if (strcmp("<", *ptr) == 0 || strcmp(">", *ptr) == 0 || strcmp("2>", *ptr) == 0) {
          std::cout << "Syntax error: The processes in middle cannot have redirections"
                    << std::endl;
          return -1;
        }
      }
    }
  }
  return process_cnt;
}
