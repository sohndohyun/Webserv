NAME = Webserv
FLAGS =	-Wall -Wextra -Werror -g
HEADER = -I.
CC = g++
SRCS = Utils.cpp AServer.cpp main.cpp EchoServer.cpp RequestParser.cpp PathParser.cpp Exception.cpp Response.cpp FileIO.cpp ConfigParse.cpp ChunkParser.cpp CGIStub.cpp
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