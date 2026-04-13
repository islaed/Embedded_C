TARGET = ex1
OUTPUT = output.txt

all: $(TARGET)

$(TARGET): ex1.c
	gcc ex1.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OUTPUT) $(TARGET)