#   Change SGBDIR to the directory where all GraphBase files will go:
SGBDIR = /home/rup/work/graphs/sgb

#   Change DATADIR to the directory where GraphBase data files will go:
DATADIR = $(SGBDIR)/data

#   Change INCLUDEDIR to the directory where GraphBase header files will go:
INCLUDEDIR = $(SGBDIR)/include

#   SHORTCUT: Uncomment these lines, for single-directory installation:
DATADIR = .
INCLUDEDIR = .
#LIBDIR = .
#BINDIR = .
#CWEBINPUTS = .

#   If you prefer optimization to debugging, change -g to something like -O:
CFLAGS = -g -I$(INCLUDEDIR)

########## You shouldn't have to change anything after this point ##########

CC = clang
KERNELFILES = gb_flip.c gb_graph.c gb_io.c gb_dijk.c
ALL = $(KERNELFILES)

OBJS = $(KERNELFILES:.c=.o)
HEADERS = $(OBJS:.o=.h)

lib: libgb.a

libgb.a: $(OBJS)
	rm -f certified
	ar rcv libgb.a $(OBJS)
	- ranlib libgb.a

gb_io.o: gb_io.c
	$(CC) $(CFLAGS) -DDATA_DIRECTORY=\"$(DATADIR)/\" -c gb_io.c

test_queue: gb_dijk.o gb_graph.o
	$(CC) $(CFLAGS) test_queue.c gb_dijk.o gb_graph.o -o test_queue

test_io: gb_io.o
	$(CC) $(CFLAGS) test_io.c gb_io.o -o test_io

test_graph: gb_graph.o
	$(CC) $(CFLAGS) test_graph.c gb_graph.o -o test_graph

test_flip: gb_flip.o
	$(CC) $(CFLAGS) test_flip.c gb_flip.o -o test_flip

tests: test_io test_graph test_flip
	./test_io
	./test_graph
	./test_flip
	make lib
	make test_sample
	- ./test_sample > sample.out
	diff test.gb test.correct
	diff sample.out sample.correct
	rm test.gb sample.out test_io test_graph test_flip test_sample
	echo "Congratulations --- the tests have all been passed."
	touch certified

clean:
	rm -f *~ *.o libgb.a certified
