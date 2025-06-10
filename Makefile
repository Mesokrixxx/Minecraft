NAME = Minecraft

W64DEVKIT_PATH = C:/w64devkit

CC = gcc

ifeq ($(OS), Windows_NT)
	BIN = $(NAME).exe
	CFLAGS = -I$(W64DEVKIT_PATH)/include -D_WINDOWS_
	LFLAGS = -L$(W64DEVKIT_PATH)/lib -lSDL2 -lglew32 -lopengl32 -lcglm
else
	BIN = $(NAME)
	UNAME_S = $(shell uname -s)
	ifeq (UNAME_S, Linux)
		CFLAGS = -D_LINUX_
		LFLAGS = -lSDL2 -lGLEW -lGL -lcglm -lm
	endif
# TODO: macos makefile part
	ifeq (UNAME_S, Darwin)
		CFLAGS = -D_APPLE_
		LFLAGS = 
	endif
endif

SRC = $(wildcard src/*.c src/**/*.c)
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(BIN) $(LFLAGS)

clean:
	rm -f $(OBJ) $(NAME)

re: clean all

run: all
	./$(BIN)

ren: clean run

.PHONY: all clean re run ren
