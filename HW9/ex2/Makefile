TARGET = file_manager

all: $(TARGET)

$(TARGET): file_manager.c
	gcc file_manager.c -lncursesw -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

debug:
	gcc -g file_manager.c -lncursesw -o $(TARGET)

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)