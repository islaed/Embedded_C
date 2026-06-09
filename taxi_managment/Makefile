TAXI_MANAGMENT = taxi_managment
OBJECT = driver_list.o

all: $(TAXI_MANAGMENT)

$(TAXI_MANAGMENT): taxi_managment.c $(OBJECT)
	gcc taxi_managment.c $(OBJECT) -o $(TAXI_MANAGMENT)

$(OBJECT): driver_list.c
	gcc -c driver_list.c -o $(OBJECT)

clean:
	rm -rf $(TAXI_MANAGMENT) $(OBJECT)