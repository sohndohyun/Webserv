NAME = Webserv
NAME_BONUS = Webserv_bonus
FLAG =	-Wall -Wextra -Werror -std=c++98
HEADER = -I./include
HEADER_BONUS = -I./include_bonus
CC = clang++
SRCS =	srcs/Utils.cpp \
		srcs/AServer.cpp \
		srcs/WebServer.cpp \
		srcs/Exception.cpp \
		srcs/FileIO.cpp \
		srcs/Response.cpp \
		srcs/ConfigParse.cpp \
		srcs/Request.cpp \
		srcs/ConfigCheck.cpp \
		srcs/main.cpp
SRCS_BONUS = \
		srcs_bonus/Utils_bonus.cpp \
		srcs_bonus/AServer_bonus.cpp \
		srcs_bonus/WebServer_bonus.cpp \
		srcs_bonus/Exception_bonus.cpp \
		srcs_bonus/FileIO_bonus.cpp \
		srcs_bonus/Response_bonus.cpp \
		srcs_bonus/ConfigParse_bonus.cpp \
		srcs_bonus/Request_bonus.cpp \
		srcs_bonus/ConfigCheck_bonus.cpp \
		srcs_bonus/main_bonus.cpp

OBJS = $(SRCS:.cpp=.o)
OBJS_BONUS = $(SRCS_BONUS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAG) $(HEADER) $^ -o $(NAME)

bonus : $(OBJS_BONUS)
	$(CC) $(FLAG) $(HEADER_BONUS) $^ -o $(NAME_BONUS)

%.o: %.cpp
	$(CC) $(FLAG) $(HEADER) $(HEADER_BONUS) -o $@ -c $<

clean:
	rm -f $(OBJS)
	rm -f $(OBJS_BONUS)

fclean: clean
	rm -f $(NAME)
	rm -f $(NAME_BONUS)

re: fclean all
