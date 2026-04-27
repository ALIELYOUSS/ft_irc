NAME = ircserv

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -Iincludes

SRCS = \
	src/main.cpp \
	src/server.cpp \
	src/server_runtime.cpp \
	src/client.cpp \
	src/channel.cpp \
	src/tools.cpp \
	src/commands/auth.cpp \
	src/commands/invite.cpp \
	src/commands/join.cpp \
	src/commands/kick.cpp \
	src/commands/mode.cpp \
	src/commands/privmsg.cpp \
	src/commands/topic.cpp

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
