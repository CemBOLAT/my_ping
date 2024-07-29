
NAME := ft_ping

SRC := main.c \
		./srcs/parser.c \
		./utils/superArray.c \
		./utils/perfectExit.c \

OBJ := $(SRC:.c=.o)


all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean

re: fclean all

.PHONY: all clean fclean re