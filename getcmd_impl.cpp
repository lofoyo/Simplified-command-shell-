#include "myShell.h"
//The set_command() function is to set the command execution status and return the full command name
std::string myShell::set_command(char * cmdname) {
  cmd.clear();
  ST = DEFAULT;  //status DEFAULT
  std::string inputcmd(cmdname);
  char ** index = paths;
  if (inputcmd.empty()) {
    cmd = inputcmd;
    ST = CONTINUE;  //status CONTINUE
    return cmd;
  }
  else if (inputcmd == "exit") {
    cmd = inputcmd;
    ST = EXIT;  //status EXIT
    return cmd;
  }
  else if (inputcmd == "cd") {
    cmd = inputcmd;
    ST = CDEXE;  //status CDEXE
    return cmd;
  }
  else if (inputcmd == "set") {
    cmd = inputcmd;
    ST = SETEXE;  //status SETEXE
    return cmd;
  }
  else if (inputcmd == "export") {
    cmd = inputcmd;
    ST = EXPEXE;  //status EXPEXE
    return cmd;
  }
  else if (inputcmd == "inc") {
    cmd = inputcmd;
    ST = INCEXE;  //status INCEXE
    return cmd;
  }
  else if (inputcmd.find('/') != std::string::npos) {
    struct stat filestat;
    if (access(inputcmd.c_str(), X_OK) != 0 || lstat(inputcmd.c_str(), &filestat) != 0 ||
        S_ISDIR(
            filestat.st_mode)) {  //check whether the command with slash in it is executable or not
      perror("Cannot execute");
      cmd = inputcmd;
      ST = CONTINUE;  //status CONTINUE
    }
    else {
      cmd = inputcmd;
      ST = NOMEXE;  //status NOMEXE
    }
    return cmd;
  }
  else {
    while (*index != NULL) {
      std::string searchpath(*index);
      cmd = findcmd(searchpath, inputcmd);  //get fullname
      if (cmd != " ") {                     //found
        ST = NOMEXE;
        break;
      }
      index++;
    }
    if (cmd == " ") {  //not found
      std::cout << "Command " << inputcmd << " is not found or cannot be executed" << std::endl;
      cmd = inputcmd;
      ST = CONTINUE;
      return cmd;
    }
    else {
      return cmd;
    }
  }
  return cmd;
}
//The findcmd function is to search the each directory in ECE551PATH to find the full command name
std::string myShell::findcmd(std::string & dir, std::string & cmd) {
  DIR * D;
  struct dirent * ent;
  if ((D = opendir(dir.c_str())) != 0) {
    while ((ent = readdir(D)) != NULL) {
      std::string filename(ent->d_name);
      if (filename == "." || filename == "..") {
        continue;
      }
      //subdirectory dir/subdir recursively
      else if (ent->d_type == DT_DIR) {
        std::string subdir = dir + "/" + filename;
        findcmd(subdir, cmd);
      }
      else {
        if (filename == cmd) {
          std::string path = dir + "/" + filename;
          struct stat filestat;  //check the new file with full path is executable or not
          if (access(path.c_str(), X_OK) != 0 || lstat(path.c_str(), &filestat) != 0 ||
              S_ISDIR(filestat.st_mode)) {
            path = " ";
          }
          closedir(D);
          return path;
        }
      }
    }
    closedir(D);
  }
  else {
    perror("Path directory error");
    closedir(D);
    return " ";
  }
  return " ";  //cannot find this command ,return " "
}
