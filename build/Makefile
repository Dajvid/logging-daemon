CC = gcc
EXECUTABLE = logd
CFLAGS = -Wall -Wextra -pedantic -g
OBJS = logging_deamon.o file_list.o htable.o
SRC = ../src/

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: $(SRC)%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f $(EXECUTABLE) *.o
