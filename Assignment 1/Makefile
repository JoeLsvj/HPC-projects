CC=mpicc
INCDIR=include
OBJDIR=obj
SRCDIR=src
SNAPDIR=snapshots

CFLAGS=-I$(INCDIR) -Wall -c -fopenmp
OBJFLAGS=-Wall -fopenmp -O3 -march=native

OBJECTS=$(OBJDIR)/main.o $(OBJDIR)/initialize_playground.o $(OBJDIR)/read_write_pgm_image.o $(OBJDIR)/play_game.o\
        $(OBJDIR)/evolve_cell.o $(OBJDIR)/ordered_evolution.o $(OBJDIR)/static_evolution.o

main.x: $(OBJECTS)
	$(CC) $(OBJFLAGS) -o $@ $^
$(OBJDIR)/main.o: main.c
	$(CC) $(CFLAGS) -o $@ $^	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm -f $(OBJDIR)/*.o *.x
	rm -f *.pgm
	rm -f $(SNAPDIR)/*

