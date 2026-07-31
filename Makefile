NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -Iincludes
SRCS = \
	src/main.cpp \
	src/server.cpp \
	src/server_runtime.cpp \
	src/commands_tools/server_buffer.cpp \
	src/commands_tools/server_commands.cpp \
	src/commands_tools/server_send.cpp \
	src/commands/server_ping.cpp \
	src/commands/server_registration.cpp \
	src/commands/server_join.cpp \
	src/commands/server_part.cpp \
	src/commands/server_privmsg.cpp \
	src/commands/server_topic.cpp \
	src/commands/server_kick.cpp \
	src/commands/server_invite.cpp \
	src/commands/server_mode.cpp \
	src/client.cpp \
	src/channel.cpp \
	src/tools.cpp
OBJS = $(SRCS:.cpp=.o)
all: $(NAME)
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
clean:
	rm -f $(OBJS)
fclean: clean
	rm -f $(NAME)
re: fclean all
.PHONY: all clean fclean re
