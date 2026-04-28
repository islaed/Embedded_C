EX3 = ex3
FOLDER = files


$(EX3): ex3.c files_make
	gcc ex3.c -o $(EX3)

run: $(EX3)
	./$(EX3)

files_make:
	$(MAKE) -C $(FOLDER)

clean:
	rm -f $(EX3)
	$(MAKE) -C $(FOLDER) clean