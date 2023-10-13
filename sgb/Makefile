KERNELFILES = gb_flip.c gb_graph.c
ALL = $(KERNELFILES)

OBJS = $(KERNELFILES:.c=.o)
HEADERS = $(OBJS:.o=.h)

lib: libgb.a

libgb.a: $(OBJS)
	rm -f certified
	ar rcv libgb.a $(OBJS)
	- ranlib libgb.a

test_graph: gb_graph.o
	$(CC) $(CFLAGS) test_graph.c gb_graph.o -o test_graph

test_flip: gb_flip.o
	$(CC) $(CFLAGS) test_flip.c gb_flip.o -o test_flip

tests: test_graph test_flip
	./test_graph
	./test_flip
	make lib
	make test_sample
	- ./test_sample > sample.out
	diff test.gb test.correct
	diff sample.out sample.correct
	rm test.gb sample.out test_graph test_flip test_sample
	echo "Congratulations --- the tests have all been passed."
	touch certified

clean:
	rm -f *~ *.o libgb.a certified
