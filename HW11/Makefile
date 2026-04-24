TARGET = ex


$(TARGET): ex.c
	gcc ex.c -o $(TARGET) -lpthread

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET)