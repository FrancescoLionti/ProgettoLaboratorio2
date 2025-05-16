# ==============================================================
# 						Compilazione Java
# ==============================================================
.PHONY: all java c valgrind-c clean clean-java clean-c 

all: java c

FILE_JAVA = ../Java/Attore.java ../Java/CreaGrafo.java

java: $(FILE_JAVA)
	javac -d ../Eseguibili/ $(FILE_JAVA)

clean-java:
	rm -f ../Eseguibili/*.class

# ==============================================================
# 						Compilazione C
# ==============================================================

ESEGUIBILE = ../Eseguibili/cammini.out
FILE_C = ../C/cammini.c ../C/xerrori.c ../C/supportoBFS.c
FILE_O = ../C/cammini.o ../C/xerrori.o ../C/supportoBFS.o

c: $(ESEGUIBILE)

../C/%.o: ../C/%.c ../C/xerrori.h ../C/supportoBFS.h
	gcc -std=c11 -Wall -g -O3 -pthread -c $< -o $@

$(ESEGUIBILE): $(FILE_O)
	gcc -std=c11 -Wall -g -O3 -pthread -o $@ $(FILE_O) -lm -pthread

clean-c:
	rm -f $(FILE_O) $(ESEGUIBILE)

# ==============================================================
# 						Target generali
# ==============================================================

valgrind-c: c
	valgrind --leak-check=full --show-leak-kinds=all $(ESEGUIBILE) $(C_INPUT) $(NTHREADS)

clean: clean-java clean-c
