FPROG = fprog
SPROG = sprog

all: $(FPROG) $(SPROG)

$(FPROG) : first_prog.c
	gcc first_prog.c -o $(FPROG)

$(SPROG) : second_prog.c
	gcc second_prog.c -o $(SPROG)

clean:
	rm -rf $(FPROG) $(SPROG)