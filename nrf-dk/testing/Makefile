CC=gcc
CFLAGS=-Iinclude
LDFLAGS=-lbluetooth

SRC = src/main.c src/utils.c
OBJ = $(SRC:.c=.o)
EXECUTABLE=production

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)  # Make sure this line starts with a tab

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)  # And this line too

clean:
	rm -f src/*.o $(EXECUTABLE)  # And this line as well
