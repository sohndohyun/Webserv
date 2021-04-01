NAME = Webserv
FLAGS =	-Wall -Wextra -Werror -std=c++98
HEADER = -I./include
CC = clang++
SRCS =	srcs/Utils.cpp \
		srcs/AServer.cpp \
		srcs/WebServer.cpp \
		srcs/RequestParser.cpp \
		srcs/PathParser.cpp \
		srcs/Exception.cpp \
		srcs/Response.cpp \
		srcs/FileIO.cpp \
		srcs/ConfigParse.cpp \
		main.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAG) $(HEADER) $^ -o $(NAME)


%.o: %.cpp
	$(CC) $(FLAG) $(HEADER) -o $@ -c $<

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)
	
re: fclean all