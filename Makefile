NAME = pestilence
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g3

SRC_PATH = src/
OBJ_PATH = obj/

SRC = files_handle.c \
	main.c \
	signature.c \
	injector.c \
	elf_parser.c \
	syscall_tracer.c \
	anti_process.c

SRCS = $(addprefix $(SRC_PATH), $(SRC))
OBJS = $(patsubst $(SRC_PATH)%.c,$(OBJ_PATH)%.o,$(SRCS))

GREEN = \033[0;32m
BLUE = \033[0;34m
YELLOW = \033[0;33m
RED = \033[0;31m
RESET = \033[0m


all: $(NAME)

$(NAME): $(OBJS)
	@printf "$(YELLOW)Building $(NAME)...$(RESET) \n"
	@$(CC) $(CFLAGS) $(OBJS) $(INC) -o $(NAME) && \
		printf "$(GREEN)✔ Build succesful!$(RESET) \n" || \
		printf "$(RED)✘ Build failed!$(RESET) \n"

$(OBJ_PATH)%.o: $(SRC_PATH)%.c | $(OBJ_PATH)
	@printf "$(BLUE)Compiling $<...$(RESET) \n"
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OBJ_PATH):
	@printf "$(BLUE)Creating object directory...$(RESET) \n"
	@mkdir -p $(OBJ_PATH)

clean:
	@printf "$(BLUE)Cleaning object files...$(RESET) \n"
	@rm -f $(OBJS)
	@printf "$(GREEN)✔ Objects cleaned succesfully!$(RESET) \n"

fclean: clean
	@printf "$(BLUE)Removing binaries, dependencies and object files...$(RESET) \n"
	@rm -f $(NAME)
	@printf "$(GREEN)✔ Directory cleaned succesfully!$(RESET) \n"

re: fclean all

call: all clean
	@printf "$(YELLOW)Cleaning dependency builds...$(RESET) \n"
	@printf "$(GREEN)✔ Dependency builds cleaned succesfully!$(RESET) \n"

.PHONY: all clean fclean re call
