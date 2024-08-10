
NAME := ft_ping

SRC := main.c \
		./srcs/parser.c \
		./utils/superArray.c \
		./utils/perfectExit.c \
		./srcs/ping.c \
		./srcs/socket.c \
		./utils/tokenize_utils.c \
		./utils/packetUtils.c \
		./utils/packet_calc.c \
		./utils/init.c \
		./utils/libc.c \


OBJ := $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -g -ggdb -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re