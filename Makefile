SRCS=myShell.cpp myShell.h env_path.cpp parse_input.cpp getcmd_impl.cpp shell_free_impl.cpp free_envp.cpp exe_impl.cpp exe_extra_impl.cpp check_redir_pipes.cpp
OBJS=$(patsubst %.c, %.o, $(SRCS))
CPPFLAGS=-ggdb3 -Wall -Werror -pedantic -std=gnu++03

myShell: $(OBJS)
	g++ $(CPPFLAGS) -o myShell $(OBJS)
%.o: %.cpp
	g++ $(CPPFLAGS) -c $<
clean:
	rm  *~
