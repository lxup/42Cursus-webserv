# ** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rgiraud <rgiraud@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/13 15:29:09 by lquehec           #+#    #+#              #
#    Updated: 2024/08/02 15:33:31 by rgiraud          ###   ########.fr        #
#                                                                              #
# ** #

# ** #
#                                     VARS                                     #
# ** #

END				=	\001\033[0m\002

# COLORS
BLACK			=	\001\033[0;30m\002
RED				=	\001\033[0;31m\002
GREEN			=	\001\033[0;32m\002
ORANGE			=	\001\033[0;33m\002
BLUE			=	\001\033[0;34m\002
PURPLE			=	\001\033[0;35m\002
CYAN			=	\001\033[0;36m\002
LIGHT_GRAY		=	\001\033[0;37m\002
DARK_GRAY		=	\001\033[1;30m\002
LIGHT_RED		=	\001\033[1;31m\002
LIGHT_GREEN		=	\001\033[1;32m\002
YELLOW			=	\001\033[1;33m\002
LIGHT_BLUE		=	\001\033[1;34m\002
LIGHT_PURPLE	=	\001\033[1;35m\002
LIGHT_CYAN		=	\001\033[1;36m\002
WHITE			=	\001\033[1;37m\002

# ** #
#                                   PROGRAM                                    #
# ** #

NAME			=	webserv

# ** #
#                                   COMPILER                                   #
# ** #

CXX				=	c++
RM				=	rm -rf
CXXFLAGS		+=	-Wall -Wextra -Werror -std=c++98 -g3

# ** #
#                                    PATHS                                     #
# ** #

INC_PATH		=	includes
SRC_PATH		=	srcs
OBJ_PATH		=	.obj

# ** #
#                                    FLAGS                                     #
# ** #

CXXFLAGS		+=	-I$(INC_PATH)

CXXFLAGS		+=	-MMD -MP

# CXXFLAGS		+=	-pedantic

# ** #
#                                   SOURCES                                    #
# ** #

# MAIN
MAIN			=	main

# LOGGER
LOGGER_PATH		=	$(SRC_PATH)/Logger
LOGGER			=	Logger

# CONFIG
CONFIG_PATH		=	$(SRC_PATH)/Config
CONFIG			=	ArgsManager \
					ConfigParser \
					BlocServer \
					BlocLocation \
					ListenConfig \
					ErrorPage \

# SERVER
SERVER_PATH		=	$(SRC_PATH)/Server
SERVER			=	Server \
					Socket \
					Client \

# REQUEST
REQUEST_PATH	=	$(SRC_PATH)/Request
REQUEST			=	Request \
					RequestCgi \
					RequestBody \

# RESPONSE
RESPONSE_PATH	=	$(SRC_PATH)/Response
RESPONSE		=	Response

# CGI
CGI_PATH		=	$(SRC_PATH)/Cgi
CGI				=	CgiExecutor \
					CgiHandler \
					

# UTILS
UTILS_PATH		=	$(SRC_PATH)/Utils
UTILS			=	Utils

SRCS			+=	$(addprefix $(SRC_PATH)/, $(addsuffix .cpp, $(MAIN))) \
					$(addprefix $(LOGGER_PATH)/, $(addsuffix .cpp, $(LOGGER))) \
					$(addprefix $(CONFIG_PATH)/, $(addsuffix .cpp, $(CONFIG))) \
					$(addprefix $(SERVER_PATH)/, $(addsuffix .cpp, $(SERVER))) \
					$(addprefix $(REQUEST_PATH)/, $(addsuffix .cpp, $(REQUEST))) \
					$(addprefix $(RESPONSE_PATH)/, $(addsuffix .cpp, $(RESPONSE))) \
					$(addprefix $(CGI_PATH)/, $(addsuffix .cpp, $(CGI))) \
					$(addprefix $(UTILS_PATH)/, $(addsuffix .cpp, $(UTILS)))

CLASSES			=	$(LOGGER_PATH) $(CONFIG_PATH) $(SERVER_PATH) $(REQUEST_PATH) $(RESPONSE_PATH) $(CGI_PATH) $(UTILS_PATH)

CXXFLAGS		+=	$(addprefix -I, $(CLASSES))

OBJS			=	$(SRCS:%.cpp=$(OBJ_PATH)/%.o)

DEPS			=	$(OBJS:.o=.d)


# ** #
#                                    RULES                                     #
# ** #

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "\n${GREEN}> $(NAME) was successfuly compiled 🎉${END}"

$(OBJ_PATH)/%.o: %.cpp
			@mkdir -p $(dir $@)
			@printf "${BLUE}>Generating $(NAME) objects... %-33.33s\r${END}" $@
			@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJ_PATH)
	@printf "${YELLOW}> Cleaning $(NAME)'s objects has been done ❌${END}\n"

fclean:	clean
	@$(RM) $(NAME)
	@printf "${YELLOW}> Cleaning of $(NAME) has been done ❌${END}\n"

re: clean all

-include $(DEPS)

.PHONY: all clean re fclean