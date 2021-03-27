NAME = Webserv
FLAGS =	-Wall -Wextra -Werror
HEADER = -I.
CC = clang++
SRCS = Utils.cpp AServer.cpp main.cpp EchoServer.cpp RequestParser.cpp PathParser.cpp Exception.cpp Response.cpp FileIO.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)
$(NAME): $(SRCS)
	$(CC) $(FLAGS) $(HEADER) -c $(SRCS)
	clang++ $(FLAGS) $(HEADER) $(OBJS) -o $(NAME)
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)
	
re: fclean all