SRC = src/main.cpp \
    	  src/Server.cpp \
		  src/Client.cpp 
		

NAME = ircserv
CC = c++
CFLAGS = -std=c++98 -Wall -Wextra -Werror
OBJ = $(SRC:.cpp=.o)

all : $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

re : fclean all

clean :
	rm -f $(OBJ)

fclean : clean
	rm -rf $(NAME)

.PHONY : all re clean fclean