#include <fstream>
#include <iostream>
#include <string>
int main() {
  std::ofstream ofs;
  ofs.open("largeout", std::ofstream::out);
  std::string s = "Hello, world!";
  for (size_t i = 0; i < 100000; i++) {
    ofs << s << std::endl;
    std::cout << s << std::endl;
  }

  return 0;
}
