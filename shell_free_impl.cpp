#include "myShell.h"
//The ext() function is to free all the spaces allocated in the heap when my Shell exits
void myShell::ext() {
  printf("Exit successfully\n");
  clearpaths();
  if (argv != NULL) {
    free(argv);

    argv = NULL;
  }
  if (proc_argv != NULL) {
    delete[] proc_argv;
    proc_argv = NULL;
  }
  free(inputbuffer);
  inputbuffer = NULL;
}
//The cont() function is to free all the spaces for input arguments allocated in the heap when my Shell continues to execute next command
void myShell::cont() {
  if (argv != NULL) {
    free(argv);
    argv = NULL;
  }
  if (proc_argv != NULL) {
    delete[] proc_argv;
    proc_argv = NULL;
  }
  free(inputbuffer);
  inputbuffer = NULL;
}
