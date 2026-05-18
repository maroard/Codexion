# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maroard <maroard@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/01 11:16:55 by maroard           #+#    #+#              #
#    Updated: 2026/05/18 10:34:37 by maroard          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

AR			= ar rcs
NAME		= codexion
CC			= gcc
CFLAGS		= -I. -Wall -Wextra -Werror -pthread -g
RM			= rm -f

SRC_DIR		= src
OBJ_DIR		= obj

SRC_FILES	= $(shell find $(SRC_DIR) -type f -name "*.c")
OBJ_FILES	= $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

all: $(NAME)

$(NAME): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -lm -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(OBJ_FILES)

fclean: clean
	$(RM) $(NAME)
	$(RM) -r $(OBJ_DIR)

re: fclean all

.PHONY: all clean fclean re
