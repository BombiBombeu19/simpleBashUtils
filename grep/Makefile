CC=gcc
CFLAGS=-c -Wall -Werror -Wextra
SRC1=s21_grep.c

all: s21_grep

s21_cat: s21_grep.o 
	$(CC) -o s21_grep s21_grep.o 

main.o: $(SRC1) 
	$(CC) $(CFLAGS) $(SRC1)

clean:
	rm -f *.o s21_grep

rebuild: clean all