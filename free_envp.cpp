#include <unistd.h>

#include <cstdio>
#include <cstdlib>
//This free_envp() function is to free the envp array, called when exit my shell
void free_envp(char ** envp) {
  char ** ptr = envp;
  while (*ptr != NULL) {
    delete[] * ptr;
    ptr++;
  }

  delete[] envp;
}
