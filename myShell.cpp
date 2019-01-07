#include "myShell.h"

#include <dirent.h>
#include <fcntl.h>
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
#include <vector>
int main() {
  extern char ** environ;
  myShell ms;
  ms.init_envvars(environ);
  char ** envp = NULL;
  envp = ms.update_envvars();
  ms.update_paths();  //initialize the ECE551PATH
  while (true) {
    char dirbuffer[1024];
    getcwd(dirbuffer, 1024);
    std::cout << "myShell:" << dirbuffer << " $";
    fflush(stdout);
    ms.read_input();  //read from the command line
    if (ms.get_status() == EXIT) {
      ms.ext();
      free_envp(envp);
      return 0;
    }                                      //EOF exit
    ms.check_input();                      //replace the $var in input with its value
    char ** inputargv = ms.split_input();  //get the input arguments
    if (inputargv == NULL) {
      ms.cont();  //continue to the next command line
      continue;
    }
    std::string cmd = ms.set_command(inputargv[0]);  //get the first command
    if (ms.get_status() == EXIT) {
      ms.ext();  //input exit command
      free_envp(envp);
      return 0;
    }
    else if (ms.get_status() == CONTINUE) {
      ms.cont();  //continue to the next command line
      continue;
    }
    else if (ms.get_status() == CDEXE) {
      std::string curdir(dirbuffer);
      ms.execd(curdir);  //execute cd
      ms.cont();
      continue;
    }
    else if (ms.get_status() == SETEXE) {
      ms.exeset();  //execute set
      ms.cont();
      continue;
    }
    else if (ms.get_status() == EXPEXE) {
      ms.exeexp();  //execute export
      envp = ms.update_envvars();
      ms.update_paths();  //after the export operation, update the environment variable map and ECE551PATH
      ms.cont();
      continue;
    }
    else if (ms.get_status() == INCEXE) {
      ms.exeinc();  //execute inc
      ms.cont();
      continue;
    }
    else if (ms.get_status() == NOMEXE) {
      if (ms.check_redir() == -1 || ms.check_pipes() == -1) {
        ms.cont();
        continue;
      }                           //check input syntax of redirections and pipes
      if (ms.process_cnt == 1) {  //only one command
        pid_t pid = fork();
        if (pid == -1) {
          perror("Fork failure!");
          exit(EXIT_FAILURE);
        }
        else if (pid == 0) {  //child process
          int exe_st = exe_cmd(cmd, inputargv, envp);
          ms.cont();
          if (exe_st == -1) {
            exit(EXIT_FAILURE);
          }
          else {
            exit(EXIT_SUCCESS);
          }
        }
        else {  //parent process, wait
          int status = 0;
          pid_t wpid = waitpid(pid, &status, WUNTRACED | WCONTINUED);
          if (wpid == -1) {
            perror("Waitpid problem");
            exit(EXIT_FAILURE);
          }
          if (WIFEXITED(status)) {
            printf("Program exited with status %d\n", WEXITSTATUS(status));
          }
          if (WIFSIGNALED(status)) {
            printf("Program was killed by signal %d\n", WTERMSIG(status));
          }
          ms.cont();
        }
      }
      else {  //more than one commands, using pipes
        int status;
        int * fds = new int[2 * ms.process_cnt];
        pid_t * pids = new pid_t[ms.process_cnt];
        for (int i = 0; i < ms.process_cnt; i++) {
          if (pipe(fds + 2 * i) == -1) {  //build the pipes, one pipe for one process
            perror("Pipe error");
            exit(EXIT_FAILURE);
          }
        }
        for (int i = 0; i < ms.process_cnt; i++) {
          pids[i] = fork();
          if (pids[i] == -1) {
            perror("Fork failure!");
            exit(EXIT_FAILURE);
          }
          else if (pids[i] == 0) {  //child processes
            if (i != 0) {
              dup2(fds[2 * (i - 1)], STDIN_FILENO);  //set the in pipe, excluding the first process
            }
            if (i != ms.process_cnt - 1) {
              dup2(fds[2 * i + 1], STDOUT_FILENO);  //set the out pipe, excluding the last process
            }

            for (int j = 0; j < 2 * ms.process_cnt; j++) {
              close(fds[j]);
            }  //close all
            cmd = ms.set_command(ms.proc_argv[i][0]);
            int exe_st = exe_cmd(cmd, ms.proc_argv[i], envp);
            if (exe_st == -1) {
              exit(EXIT_FAILURE);
            }
            else {
              exit(EXIT_SUCCESS);
            }
          }
        }
        //parent process, wait
        for (int j = 0; j < 2 * ms.process_cnt; j++) {
          close(fds[j]);
        }  //close all
        for (int j = 0; j < ms.process_cnt; j++) {
          pid_t w = wait(&status);
          if (w == -1) {
            perror("Waitpid problem");
            exit(EXIT_FAILURE);
          }
          if (WIFEXITED(status)) {
            printf("Program exited with status %d\n", WEXITSTATUS(status));
          }
          if (WIFSIGNALED(status)) {
            printf("Program was killed by signal %d\n", WTERMSIG(status));
          }
        }
        delete[] fds;
        delete[] pids;
        ms.cont();
      }
    }
  }
  if (envp != NULL) {
    free_envp(envp);
    envp = NULL;
  }
  return 0;
}
