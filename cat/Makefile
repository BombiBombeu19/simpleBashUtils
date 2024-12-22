CC=gcc
CFLAGS=-c -Wall -Werror -Wextra
SRC1=s21_cat.c

all: s21_cat

s21_cat: s21_cat.o 
	$(CC) -o s21_cat s21_cat.o 

main.o: $(SRC1) 
	$(CC) $(CFLAGS) $(SRC1)

clean:
	rm -f *.o s21_cat

rebuild: clean all