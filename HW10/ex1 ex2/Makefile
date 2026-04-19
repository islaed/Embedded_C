EX1 = ex1
EX2 = ex2


$(EX1): ex1.c
	gcc ex1.c -o $(EX1)

run1: $(EX1)
	./$(EX1)

$(EX2): ex2.c
	gcc ex2.c -o $(EX2)

run2: $(EX2)
	./$(EX2)

clean:
	rm -f $(EX1) $(EX2)