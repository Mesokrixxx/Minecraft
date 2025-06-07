NAME = Minecraft

W64DEVKIT_PATH = C:/w64devkit

ifeq ($(OS), Windows_NT)
	BIN = $(NAME).exe
	CFLAGS = -I$(W64DEVKIT_PATH)/include
	LFLAGS = -L$(W64DEVKIT_PATH)/lib -lSDL2 -lglew32 -lopengl32 -lcglm
else

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
