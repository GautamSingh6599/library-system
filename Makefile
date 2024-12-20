CC = gcc
N_FLAGS = -lncurses -lssl -lcrypto
CFLAGS = -Werror -g -std=gnu99 -D_GNU_SOURCE
SRC = src/tui.c src/book.c src/accounts.c src/issuing.c src/bookrecords.c src/chatbot.c
# Header files
INCLUDES = -I include/
# Output executable
OUTPUT = library_system

# Default target
all: $(OUTPUT)

# Build executable
$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(INCLUDES) -o $(OUTPUT) $(N_FLAGS)

# Clean build files
clean:
	rm -f $(OUTPUT)
	rm -f *.o

