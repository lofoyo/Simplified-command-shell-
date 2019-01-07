#ifndef __MY_SHELL__H__
#define __MY_SHELL__H__

#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
void free_envp(char ** envp);
int exe_cmd(std::string & cmd, char **& inputargv, char **& envp);
enum status {
  DEFAULT,
  EXIT,
  CONTINUE,
  CDEXE,   //execute cd
  SETEXE,  //execute set
  EXPEXE,  //execute export
  INCEXE,  //execute inc
  NOMEXE   //execute normal command
};         //different status of my Shell
class myShell
{
 private:
  char * inputbuffer;  //user input command line
  std::string
      varcmdbuffer;  //input command line stored in string type for executing command "set", "export", "inc"
  char ** argv;       //dividing input command line into arguments according to white spaces
  char ** envp;       //the environment variables passing in execve
  char * pathbuffer;  //the value of PATH
  char ** paths;  //the ECE551PATH, an array of directories getting from dividing the PATH by ":"
  std::map<std::string, std::string> usrvars;  //map of user defined variables
  std::map<std::string, std::string> envvars;  //map of environment variables
  std::string cmd;                             //the command name
  status ST;                                   //execution status

 public:
  int process_cnt;     //the number of processes in pipeline
  char *** proc_argv;  //an array of process arguments
  myShell() {
    inputbuffer = NULL;
    argv = NULL;
    envp = NULL;
    paths = NULL;
    proc_argv = NULL;
    process_cnt = 1;
    cmd = "";
    pathbuffer = NULL;
    ST = DEFAULT;
  }
  void update_paths();  //the method is to update the program searching paths for my Shell
  void init_envvars(char ** environ);
  //the method is to initialize the environment variables for my Shell,
  //put all the pair (var, value) in the environment variable map

  void read_input();  //the method is to read the inputs from stdin and put them into a inputbuffer
  char ** split_input();  //the method is to split the inputbuffer and to get the input arguments
  void check_input();     //the method is to check the $ mark in inputbuffer
  void ext();  //the method is to free all the spaces allocated in the heap when my Shell exits
  void cont();
  //the method is to to free all the spaces for input arguments allocated in the heap
  //when my Shell continues to execute next command

  std::string set_command(char * cmdname);
  //the method is to set the command execution status and return the full command name

  std::string findcmd(std::string & dir, std::string & cmd);
  //the method is to search the each direct in enviornment path to find the full command name

  void execd(std::string curdir);  //the method is to  execute "cd"
  void exeset();                   //the method is to  execute "set"
  void exeexp();                   //the method is to  execute "export"
  void exeinc();                   //the method is to  execute "inc"
  status get_status() { return ST; }
  char ** update_envvars();  //the method is to update the environment variables for my Shell
  void clearpaths();         //the method is to free the exist ECE551PATH paths array for updating
  std::string replace_var(std::string & in);
  //the method is to replace the var after $ with its variable

  int check_pipes();  //the method is to check whether pipe marks in input arguments are valid or not; and divide arguments into process arguments array according to pipe marks
  int check_redir();  //the method is to check whether redirections in input arguments are valid or not
  ~myShell() {}
};

#endif
