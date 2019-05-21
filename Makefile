CC = gcc
EXECUTABLE = logd
CFLAGS = -Wall -Wextra -pedantic -g
OBJS = logging_deamon.o file_list.o htable.o

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) $(SRC)*.o
