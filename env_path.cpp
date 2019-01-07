#include "myShell.h"
//The init_envvars() function is to initialize the environment variables for my Shell, put all the pair (var, value) in the environment variable map
void myShell::init_envvars(char ** environ) {
  char ** ptr = environ;
  while (*ptr != NULL) {
    std::string curenv(*ptr);
    size_t pos = curenv.find("=");
    std::string var = curenv.substr(0, pos);
    std::string value = curenv.substr(
        pos + 1);  //get var and value from both sides of "=", and then put this pair in map
    envvars.insert(std::pair<std::string, std::string>(var, value));
    ptr++;
  }
  std::string ece551path = "ECE551PATH";
  std::map<std::string, std::string>::iterator path_it = envvars.find("PATH");
  std::string curpath = path_it->second;
  envvars.insert(std::pair<std::string, std::string>(ece551path, curpath));
}
//The update_envvars() function is to update the environment variables for my Shell after export operation
char ** myShell::update_envvars() {
  if (envp != NULL) {
    char ** ptr = envp;
    while (*ptr != NULL) {
      delete[] * ptr;
      ptr++;
    }
    delete[] envp;
    envp = NULL;
  }
  envp = new char *[envvars.size() + 1];
  size_t i = 0;
  std::map<std::string, std::string>::iterator env_it = envvars.begin();
  for (; env_it != envvars.end(); ++env_it) {
    std::string curenv;
    curenv = env_it->first + "=" + env_it->second;  //use "=" to connect var and value

    envp[i] = new char[curenv.size() + 1];
    for (size_t j = 0; j < curenv.size(); j++) {
      envp[i][j] = curenv.at(j);
    }
    envp[i][curenv.size()] = '\0';
    i++;
  }
  envp[envvars.size()] = NULL;
  return envp;
}
//The update_paths() function is to update the program searching paths for my Shell after export operation
void myShell::update_paths() {
  if (paths) {
    clearpaths();
  }
  std::map<std::string, std::string>::iterator path_it = envvars.find("ECE551PATH");
  std::string curpath = path_it->second;
  pathbuffer = new char[curpath.size() + 1];
  for (size_t i = 0; i < curpath.size(); i++) {
    pathbuffer[i] = curpath.at(i);
  }
  pathbuffer[curpath.size()] = '\0';
  paths = new char *[strlen(pathbuffer) + 1];
  paths[0] = pathbuffer;
  char * splitptr = pathbuffer;
  size_t i = 1;  //split the ECE551PATH variable into path arrays according to ":"
  while (*splitptr) {
    if ((*splitptr) == ':') {
      *splitptr = '\0';
      splitptr++;
      paths[i] = splitptr;
      i++;
    }
    else {
      splitptr++;
    }
  }
  paths[i] = NULL;
}
//The clearpaths() function is to free the exist paths array
void myShell::clearpaths() {
  delete[] pathbuffer;
  pathbuffer = NULL;
  delete[] paths;
  paths = NULL;
}
