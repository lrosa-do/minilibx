# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lrosa-do <lrosa-do@student.42lisboa>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/08 09:30:00 by lrosa-do          #+#    #+#              #
#    Updated: 2022/06/10 11:49:46 by lrosa-do         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= libmlx.a

SRCS	=	minilibx.c



OBJS	= ${SRCS:.c=.o}


CC		= gcc
AR = ar rcs
RM = rm -f


CFLAGS	= -DUSEASSERT -g -ggdb -Wall -Wextra
CFLAGS += -I. 



.c.o:
			${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

${NAME}:	${OBJS}
			ar -rcs ${NAME} ${OBJS}
			ranlib $(NAME)

all:		${NAME}

clean:
	rm -f ${OBJS}


fclean: clean
	rm -f ${NAME}

re: fclean all

	

.PHONY: all,  clean, fclean, re
