CC = gcc
CFLAGS = -Wall -Werror -lncurses
SRC = src/tui.c
# Header files
INCLUDES = -I include/
# Output executable
OUTPUT = library_system

# Default target
all: $(OUTPUT)

# Build executable
$(OUTPUT): $(SRC)
	$(CC) $(SRC) $(INCLUDES) -o $(OUTPUT) $(CFLAGS)

# Clean build files
clean:
	rm -f $(OUTPUT)
	rm -f *.o

