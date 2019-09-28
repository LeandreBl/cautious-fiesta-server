##
## Makefile for *** in /home/leandre/***
## 
## Made by Léandre Blanchard
## Login   <leandre.blanchard@epitech.eu>
## 
## Started on  Thu Dec 15 20:24:11 2016 Léandre Blanchard
## Last update Sat Jan  6 19:05:29 2018 Léandre Blanchard
##

NAME	= test

CC	= gcc

LIB	= -llserver
LIB += -llstr
LIB	+= -llsocket
LIB	+= -llcbuffer
LIB	+= -lreadline
LIB += -lpthread

LIB	+= -lm -lncurses
LIB	+= -lcsfml-graphics -lcsfml-system -lcsfml-network -lcsfml-audio -lcsfml-window

SRCS	= src/main.c

OBJ	= $(SRCS:.c=.o)

RM	= rm -f

CFLAGS	= -I ./include -Wextra -Wall -Werror

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) $(LIB) -o $(NAME) $(CFLAGS)
	@       tput setaf 2; cat include/signature; tput sgr0

debug: CFLAGS += -g3
debug:	re

opti: CFLAGS += -O3
opti:	re

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
