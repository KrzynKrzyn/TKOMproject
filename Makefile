CC = g++
CPPFLAGS = -O2 -Wall -Wextra -pedantic -std=c++11 -I./include

SRC_DIR = ./src
HPP_DIR = ./include
OBJ_DIR = ./obj
OBJ = main.o source.o token.o parser.o lexer.o error.o error_manager.o ast.o semantic.o

vpath %cpp $(SRC_DIR)
vpath %hpp $(HPP_DIR)
vpath %o $(OBJ_DIR)

app: $(OBJ)
	mkdir -p bin
	$(CC) -o ./bin/TKOM \
	$(OBJ_DIR)/main.o \
	$(OBJ_DIR)/source.o \
	$(OBJ_DIR)/token.o \
	$(OBJ_DIR)/parser.o \
	$(OBJ_DIR)/lexer.o \
	$(OBJ_DIR)/error.o \
	$(OBJ_DIR)/error_manager.o \
	$(OBJ_DIR)/ast.o \
	$(OBJ_DIR)/semantic.o

.cpp.o:
	$(CC) -o $(OBJ_DIR)/$@ $(CPPFLAGS) -c $(<)

main.o: parser.hpp

source.o: source.hpp error_manager.hpp

token.o: token.hpp

parser.o: parser.hpp token.hpp lexer.hpp error_manager.hpp ast.hpp

lexer.o: lexer.hpp token.hpp source.hpp error_manager.hpp

error.o: error.hpp

error_manager.o: error_manager.hpp error.hpp

ast.o: ast.hpp token.hpp

semantic.o: semantic.hpp

clean:
	rm -f $(OBJ_DIR)/*.o
