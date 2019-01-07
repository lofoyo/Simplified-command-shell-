#include <cstdio>
#include <cstdlib>
#
int main() {
  const char * nonexist = " ";
  FILE * F = fopen(nonexist, "r");
  if (F == NULL) {
    perror("ERROR");
    exit(EXIT_FAILURE);
  }
}
