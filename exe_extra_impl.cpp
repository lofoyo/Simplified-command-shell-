#include "myShell.h"
//The execd() function is to execute cd command
void myShell::execd(std::string curdir) {
  if (argv[1] == NULL) {  //only cd, go to home dir
    chdir("/home/fl94");
    return;
  }
  else if (argv[2] != NULL) {
    printf("Syntax error: To many arguments\n");
    return;
  }
  else if (strcmp(argv[1], ".") == 0) {  //not change
    return;
  }
  else if (strcmp(argv[1], "..") == 0) {  //go up
    std::size_t found = curdir.find_last_of("/");
    std::string updir;
    if (found == 0) {
      updir = curdir;
    }
    else {
      updir = curdir.substr(0, found);
    }
    chdir(updir.c_str());
    return;
  }
  else if (argv[1][0] == '/') {
    std::string todir(argv[1]);
    if (chdir(todir.c_str()) == -1) {
      perror("chdir");  //invalid directory
      return;
    }
  }
  else {
    std::string todir(argv[1]);
    todir = curdir + "/" + todir;
    if (chdir(todir.c_str()) == -1) {
      perror("chdir");  //invalid directory
      return;
    }
  }
}

//The exeset() function is to execute set command
void myShell::exeset() {
  //erase white spaces before "set"
  if (!varcmdbuffer.empty()) {
    varcmdbuffer.erase(0, varcmdbuffer.find_first_not_of(" "));
  }
  //erase "set" and white spaces before var
  varcmdbuffer = varcmdbuffer.substr(3);
  if (!varcmdbuffer.empty()) {
    varcmdbuffer.erase(0, varcmdbuffer.find_first_not_of(" "));
  }
  else {
    std::cout << "Syntax error: Invalid input" << std::endl;  //lack of var
    return;
  }
  //get the split between var and value
  size_t split = varcmdbuffer.find_first_of(" ");
  if (split == std::string::npos) {
    std::cout << "Syntax error: Invalid input" << std::endl;  //lack of value
    return;
  }
  std::string var = varcmdbuffer.substr(0, split);
  if (!var.empty()) {
    for (std::string::iterator it = var.begin(); it != var.end(); ++it) {
      if (*it < 48 || (*it > 57 && *it < 65) || (*it > 90 && *it != 95 && *it < 97) || *it > 122) {
        std::cout << "Syntax error: Invalid var" << std::endl;  //invalid var name
        return;
      }
    }
  }
  else {
    std::cout << "Syntax error: Invalid var" << std::endl;
    return;
  }
  std::string value = varcmdbuffer.substr(split + 1);
  std::map<std::string, std::string>::iterator var_it = usrvars.find(var);
  if (var_it != usrvars.end()) {
    var_it->second = value;  //update
  }
  else {
    usrvars.insert(std::pair<std::string, std::string>(var, value));  //insert new
  }
  std::cout << "Set $" << var << " as " << usrvars.at(var) << std::endl;
  return;
}

//The exeexp() function is to execute export command. Values of environment variables only change after export operation
void myShell::exeexp() {
  //erase white spaces before "export"
  if (!varcmdbuffer.empty()) {
    varcmdbuffer.erase(0, varcmdbuffer.find_first_not_of(" "));
  }
  //erase "export" and white spaces before var
  varcmdbuffer = varcmdbuffer.substr(6);
  if (!varcmdbuffer.empty()) {
    varcmdbuffer.erase(0, varcmdbuffer.find_first_not_of(" "));
  }
  else {
    std::cout << "Syntax error: Invalid input" << std::endl;  //lack of var
    return;
  }
  std::string var = varcmdbuffer;
  if (!var.empty()) {
    for (std::string::iterator it = var.begin(); it != var.end(); ++it) {
      if (*it < 48 || (*it > 57 && *it < 65) || (*it > 90 && *it != 95 && *it < 97) || *it > 122) {
        std::cout << "Syntax error: Invalid input" << std::endl;
        return;
      }
    }
  }
  else {
    std::cout << "Syntax error: Invalid input" << std::endl;
    return;
  }
  std::map<std::string, std::string>::iterator var_it = usrvars.find(var);
  if (var_it != usrvars.end()) {
    std::map<std::string, std::string>::iterator env_it = envvars.find(var);
    if (env_it != envvars.end()) {
      env_it->second = var_it->second;
    }
    else {
      envvars.insert(std::pair<std::string, std::string>(var_it->first, var_it->second));
    }
  }
  else {
    std::cout << "The value of " << var << " is none. Cannot be exported" << std::endl;
    return;
  }
  std::cout << "Export " << var << " as environment variable." << std::endl;
  return;
}

//The exeinc() function is to execute inc command
void myShell::exeinc() {
  //erase white spaces before "inc"
  if (!varcmdbuffer.empty()) {
    varcmdbuffer.erase(0, varcmdbuffer.find_first_not_of(" "));
  }
  //erase "inc" and white spaces before var
  varcmdbuffer = varcmdbuffer.substr(3);
  if (!varcmdbuffer.empty()) {
    varcmdbuffer.erase(0, varcmdbuffer.find_first_not_of(" "));
  }
  else {
    std::cout << "Syntax error: Invalid input" << std::endl;
    return;
  }
  std::string var = varcmdbuffer;
  if (!var.empty()) {
    for (std::string::iterator it = var.begin(); it != var.end(); ++it) {
      if (*it < 48 || (*it > 57 && *it < 65) || (*it > 90 && *it != 95 && *it < 97) || *it > 122) {
        std::cout << "Syntax error: Invalid input" << std::endl;
        return;
      }
    }
  }
  else {
    std::cout << "Syntaxt error: Invalid input" << std::endl;
    return;
  }
  std::map<std::string, std::string>::iterator inc_it = usrvars.find(var);
  if (inc_it != usrvars.end()) {  //check the value is number or not, not number, set to 1
    std::string to_inc = inc_it->second;
    std::stringstream sin(to_inc);
    double inc_number;
    char not_number;
    if (!(sin >> inc_number)) {
      inc_it->second = "1";  //not number, set to 1
    }
    else if (sin >> not_number) {
      inc_it->second = "1";  //not number, set to 1
    }
    else {
      double inc_number = atof(inc_it->second.c_str());

      inc_number = inc_number + 1;
      std::ostringstream sout;
      sout << inc_number;
      inc_it->second = sout.str();
    }
  }
  else {
    usrvars.insert(std::pair<std::string, std::string>(var, "1"));
  }
  return;
}
