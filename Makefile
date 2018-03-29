CFLAGS = -g
LDFLAGS = -lm

kdtree_test:
	$(CC) kdtree2.c kdtree.c -lm -o kdtree_test
