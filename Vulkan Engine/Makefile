##
## Makefile for Dwarf in /home/aubry_g/perso/Dwarf/Vulkan Engine
## 
## Made by Victor Aubry
## Login   <aubry_g@epitech.net>
## 
## Started on  Fri Oct  7 19:43:50 2016 Victor Aubry
## Last update Fri Oct  7 20:03:54 2016 Victor Aubry
##

NAME=		Dwarf

SRC=		${wildcard *.cpp}
OBJ_DIR=	obj
OBJ=		${addprefix $(OBJ_DIR)/,$(SRC:.cpp=.o)}
CC=		g++
CXXFLAGS +=	-std=c++11 -I include/ -I ~/Downloads/VulkanSDK/1.0.26.0/x86_64/include
MK_DIR=		mkdir
RM=		rm -rf
RM_DIR=		rmdir

all:		$(NAME)

$(NAME):	$(OBJ_DIR) $(OBJ)
		$(CC) -o $(NAME) $(OBJ)

$(OBJ_DIR)/%.o:	%.cpp
		$(CC) -o $@ -c $(CXXFLAGS) $<

$(OBJ_DIR):
		$(MK_DIR) $(OBJ_DIR)

clean:
		$(RM) $(OBJ)

fclean:		clean
		$(RM) $(NAME)

ffclean:	fclean
		$(RM_DIR) $(OBJ_DIR)

re:		fclean all
