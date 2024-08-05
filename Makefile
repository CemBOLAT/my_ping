
NAME := ft_ping

SRC := main.c \
		./srcs/parser.c \
		./utils/superArray.c \
		./utils/perfectExit.c \
		./srcs/ping.c \
		./srcs/socket.c \
		./utils/tokenize_utils.c \

OBJ := $(SRC:.c=.o)


all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -g -ggdb -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean

re: fclean all

.PHONY: all clean fclean re