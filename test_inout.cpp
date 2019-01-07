#include <string.h>

#include <iostream>
#include <vector>
using namespace std;

int main(int argc, char ** argv) {
  char name[50];
  vector<char *> name_array;
  if (argc > 2) {
    if (!strcmp(argv[1], "in")) {
      while (cin >> name) {
        if (!strcmp(argv[2], "out")) {
          cout << name << endl;
        }
        else {
          cout << "Need second argument out" << endl;
        }
      }
    }
    else {
      cout << "Need first argument in" << endl;
    }
  }
  else {
    cout << "Need two arguments: in and out" << endl;
  }
  return 0;
}
