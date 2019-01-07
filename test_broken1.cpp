#include <cstdio>
#include <cstdlib>
int main() {
  char array[15];
  char * ptr = array;
  ptr += 17;
  *ptr = 10;
  return 1;
}
