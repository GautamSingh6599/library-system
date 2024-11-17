CC = gcc
CFLAGS = -Werror -lncurses
SRC = src/tui.c src/book.c
# Header files
INCLUDES = -I include/
# Output executable
OUTPUT = library_system

# Default target
all: $(OUTPUT)

# Build executable
$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(INCLUDES) -o $(OUTPUT)

# Clean build files
clean:
	rm -f $(OUTPUT)
	rm -f *.o

