FILEMANAGER = fm
SUBDIR = sd
CALC = calc
PWD = pwd
LS = ls
CAT = cat
GREP = grep
WC = wc

all: $(FILEMANAGER) $(SUBDIR) $(CALC) $(PWD) $(LS) $(CAT) $(GREP) $(WC)

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

$(CAT): cat.c
	gcc cat.c -o $(CAT)

$(GREP): grep.c
	gcc grep.c -o $(GREP)

$(WC): wc.c
	gcc wc.c -o $(WC)

clean:
	rm -f $(FILEMANAGER) $(SUBDIR) $(CALC) $(PWD) $(LS) $(CAT) $(GREP) $(WC)