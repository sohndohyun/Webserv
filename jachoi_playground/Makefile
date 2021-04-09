NAME = Webserv
FLAGS = -Wall -Werror -Wextra -g
HEADER = -I.
CC = clang++
SRCS = Utils.cpp Client.cpp AServer.cpp main.cpp EchoServer.cpp RequestParser.cpp PathParser.cpp Exception.cpp Response.cpp FileIO.cpp ConfigParse.cpp ChunkParser.cpp CGIStub.cpp
OBJS = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CC) $(FLAGS) $(HEADER) -c $<

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(HEADER) $(OBJS) -o $(NAME)
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
