
NAME := ft_ping

SRC := main.c \
		./srcs/parser.c \
		./utils/superArray.c \
		./utils/perfectExit.c \
		./srcs/ping.c \
		./srcs/socket.c \
		./utils/tokenize_utils.c \
		./srcs/packetUtils.c \
		./srcs/packet_calc.c \
		./utils/init.c \
		./srcs/packet_header.c \
		./utils/libc.c \

Headers := ./includes/*.h


OBJ := $(SRC:.c=.o)

all: $(NAME) $(Headers)

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