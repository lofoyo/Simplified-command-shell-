#include "myShell.h"
//The function read_input() is to read the inputs from stdin and put them into inputbuffer and varcmdbuffer
void myShell::read_input() {
  int in = 0;
  int buffersize = 0;
  varcmdbuffer = "";
  while ((in = fgetc(stdin)) != '\n') {
    if (in == EOF) {
      ST = EXIT;
      return;
    }
    else {
      inputbuffer = (char *)realloc(inputbuffer, (buffersize + 1) * sizeof(char));
      inputbuffer[buffersize] = (char)in;
      varcmdbuffer = varcmdbuffer + (char)in;
      buffersize++;
    }
  }
  inputbuffer = (char *)realloc(inputbuffer, (buffersize + 1) * sizeof(*inputbuffer));
  inputbuffer[buffersize] = '\0';
  ST = DEFAULT;
  return;
}
//The function split_input() is to split the inputbuffer and to get the input arguments
char ** myShell::split_input() {
  //erase white spaces at the rear of inputbuffer
  if (varcmdbuffer.find_first_not_of('\t') == std::string::npos ||
      varcmdbuffer.find_first_not_of(' ') == std::string::npos) {
    argv = NULL;
    return argv;
  }
  for (int rear = strlen(inputbuffer) - 1; rear >= 0; rear--) {
    if (isspace(*(inputbuffer + rear)) && *(inputbuffer + rear - 1) != '\\')
      *(inputbuffer + rear) = '\0';
    else
      break;
  }

  //erase white spaces at the head of inputbuffer, put the first argument in array
  char * splitptr = inputbuffer;
  if (isspace(*splitptr)) {
    while (isspace(*splitptr) && *splitptr != '\0') {
      splitptr++;
    }
  }
  size_t i = 1;
  argv = (char **)realloc(argv, i * sizeof(char *));
  argv[0] = splitptr;
  while (*splitptr != '\0') {
    if (*splitptr == ' ') {
      while (isspace(*splitptr)) {  //set each space to '\0'
        *splitptr = '\0';
        splitptr++;
      }
      argv = (char **)realloc(argv, (i + 1) * sizeof(char *));
      argv[i] = splitptr;
      i++;
    }
    else if (*splitptr == '\\' &&
             *(splitptr + 1) !=
                 '\0') {  //the escape symbol will make the white space after it a plain " "
      char * shift = splitptr - 1;  //shift to erase escape symbol
      while (shift >= argv[i - 1] && shift != NULL) {
        *(shift + 1) = *shift;
        shift--;
      }
      argv[i - 1]++;
      splitptr = splitptr + 2;
    }
    else {
      splitptr++;
    }
  }
  argv = (char **)realloc(argv, (i + 1) * sizeof(char *));
  argv[i] = NULL;
  return argv;
}
//The check_input() function is to check the $ mark in inputbuffer
void myShell::check_input() {
  std::string curinput(inputbuffer);  //current input
  size_t pos = curinput.find("$");
  if (pos == std::string::npos) {
    return;  //no "$"
  }
  else {
    std::vector<std::string> subarr;
    std::string substr = curinput.substr(0, pos);  //string before "$"
    std::string newinput = "";                     //new command line
    newinput = newinput + substr;
    curinput = curinput.erase(0, pos);
    pos = curinput.find("$", 1);  //find next "$"
    while (pos != std::string::npos) {
      substr = curinput.substr(0, pos);  //get the string between two "$"s
      curinput = curinput.erase(0, pos);
      if (!substr.empty()) {
        subarr.push_back(substr);  //put in substring array
      }
      pos = curinput.find("$", 1);  //find next "$"
    }
    subarr.push_back(curinput);

    for (std::vector<std::string>::iterator it = subarr.begin(); it != subarr.end(); ++it) {
      *it = replace_var(*it);  //replace var
      newinput = newinput + *it;
    }
    varcmdbuffer.clear();
    varcmdbuffer = newinput;  //update varcmdbuffer
    free(inputbuffer);
    inputbuffer = NULL;
    inputbuffer = (char *)realloc(inputbuffer, (newinput.size() + 1) * sizeof(char));
    for (size_t i = 0; i < newinput.size(); i++) {
      inputbuffer[i] = newinput.at(i);
    }
    inputbuffer[newinput.size()] = '\0';  //update the inputbuffer
    return;
  }
}
//The replace_var() is to replace the var after $ with its variable
std::string myShell::replace_var(std::string & in) {
  std::string out = "";
  if (in.empty()) {
    return in;
  }
  else {
    size_t pos = 0;
    for (std::string::iterator it = in.begin() + 1; it != in.end(); ++it) {
      if (*it < 48 || (*it > 57 && *it < 65) || (*it > 90 && *it != 95 && *it < 97) || *it > 122) {
        break;  //stop at invalid variable name char
      }
      pos++;
    }
    std::string to_check = in.substr(1, pos);
    if (to_check.empty()) {
      return in;
    }
    else {
      std::string remain_str = in.substr(pos + 1);
      std::map<std::string, std::string>::iterator var_it = usrvars.find(to_check);
      std::map<std::string, std::string>::iterator env_it = envvars.find(to_check);
      if (var_it != usrvars.end()) {
        out = out + var_it->second;
      }
      else if (env_it != envvars.end()) {
        out = out + env_it->second;
      }
      else {
        std::cout << "The value of " << to_check << " is none" << std::endl;
        out = out + "";  //not found this var, return the original
      }
      out = out + remain_str;
    }
  }
  return out;
}
