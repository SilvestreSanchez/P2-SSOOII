DIROBJ = obj/
DIREXE = exec/
DIRHEA = include/
DIRSRC = src/

CFLAGS = -pthread -std=c++11 -I $(DIRHEA) 
CC = g++


# If the first argument is "solution"...
ifeq (solution,$(firstword $(MAKECMDGOALS)))
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

all : dirs SSOOIIGLE

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

SSOOIIGLE: $(DIROBJ)SSOOIIGLE.o
	$(CC) $(DIRSRC)SSOOIIGLE.cpp -o $(DIREXE)SSOOIIGLE $(CFLAGS)

$(DIROBJ)%.o: $(DIRSRC)%.cpp
	$(CC) $(CFLAGS) $^ -o $@

solution:
	./$(DIREXE)SSOOIIGLE $(RUN_ARGS)

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~
