FILEMANAGER = fm
SUBDIR = sd
CALC = calc
PWD = pwd
LS = ls

all: $(FILEMANAGER) $(SUBDIR) $(CALC) $(PWD) $(LS)

$(FILEMANAGER): file_manager.c
	gcc file_manager.c -lncursesw -o $(FILEMANAGER)

$(SUBDIR): subscriber_directory.c
	gcc subscriber_directory.c -o $(SUBDIR)

$(CALC): calc.c
	gcc calc.c -o $(CALC)

$(PWD): pwd.c
	gcc pwd.c -o $(PWD)

$(LS): ls.c
	gcc ls.c -o $(LS)

clean:
	rm -f $(FILEMANAGER) $(SUBDIR) $(CALC) $(PWD) $(LS)