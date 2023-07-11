NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

INCLUDES_DIR = includes
SRCS_DIR = srcs
OBJS_DIR = objs

SRCS = $(addprefix $(SRCS_DIR)/, main.cpp Server.cpp Client.cpp Parsing.cpp Command.cpp)
OBJS = $(addprefix $(OBJS_DIR)/, $(notdir $(SRCS:.cpp=.o)))

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -I $(INCLUDES_DIR) -o $(NAME)

$(OBJS_DIR)/%.o : $(SRCS_DIR)/%.cpp
	mkdir -p $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) -I $(INCLUDES_DIR) -c $< -o $@

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)
	rm -rf $(OBJS_DIR)

re :
	make fclean
	make all

.PHONY : all clean fclean re
