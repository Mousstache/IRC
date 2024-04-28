NAME = ircserv

INCLUDE_DIR := include
OBJ_DIR := .obj

SRC = main.cpp \
      src/Client.cpp  \
      src/Server.cpp  \
      src/Channel.cpp \
	  src/Kick.cpp \
	  src/Mode.cpp \
	  src/Topic.cpp \
	  src/Join.cpp \
	  src/Privmsg.cpp \
	  src/Bot.cpp \
	  src/Files.cpp \
	  src/Invite.cpp \
	  src/Nick.cpp


OBJS = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

COMPILE = c++

CXXFLAGS = -Wall -Wextra -Werror -g3 -std=c++98 -fPIE -I$(INCLUDE_DIR)

EXE_NAME = -o $(NAME)

EXEC = $(NAME)

-include $(INCLUDE_DIR)/*.mk

all: $(NAME)
	@echo "Compilation terminée."

$(NAME): $(OBJS)
	@$(COMPILE) $(LDFLAGS) $^ $(LDLIBS) $(EXE_NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(COMPILE) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(EXEC)

fclean: clean

re: fclean all