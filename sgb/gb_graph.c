#include <stdio.h>
#include <string.h>

/* <Preprocessor definitions> */

/* <Type declarations 8> */

typedef union {
  struct vertex_struct *V; /* pointer to Vertex */
  struct arc_struct *A;    /* pointer to Arc */
  struct graph_struct *G;  /* pointer to Graph */
  char *S;                 /* pointer to string */
  long I;                  /* integer */
} util;

typedef struct vertex_struct {
  struct arc_struct *arcs; /* linked list of arcs coming out of this vertex */
  char *name;              /* string identifying this vertex symbolically */
  util u, v, w, x, y, z;   /* multipurpose fields */
} Vertex;

typedef struct arc_struct {
  struct vertex_struct *tip; /* the arc points to this vertex */
  struct arc_struct *next;   /* another arc pointing from the same vertex */
  long len;                  /* length of this arc */
  util a, b;                 /* multipurpose fields */
} Arc;

#define gb_typed_alloc(n, t, s) (t *)gb_alloc((long)((n) * sizeof(t)), s)
#define init_area(s) *s = NULL;

struct area_pointers {
  char *first; /* address of the beginning of this block  */
  struct area_pointers
      *next; /* address of area pointers in the previously allocated block  */
};

/* The Area type is defined to be an array of length 1. This makes it
 * possible for users to say just 's' instead of '&s' when using an
 * area variable as a parameter. */

typedef struct area_pointers *Area[1];

/* <Private declarations 28> */

/* <External declarations 5> */

long verbose = 0;    /* nonzero if "verbose" output is desired */
long panic_code = 0; /* set nonzero if graph generator returns null pointer */
long gb_trouble_code = 0; /* did gb_alloc return NULL? */

/* <External functions 13> */

/* n: is the number of consecutive bytes desired  */
/* s: storage area that will contain the new block  */
char *gb_alloc(long n, Area s) {
  long m = sizeof(char *); /* m is the size of a pointer variable */
  Area t;                  /* a temporary pointer */
  char *loc;               /* the block address */
  if (n <= 0 || n > 0xffff00 - 2 * m) {
    gb_trouble_code |= 2; /* illegal request */
    return NULL;
  }
  n = ((n + m - 1) / m) * m; /* round up to multiple of m */
  loc = (char *)calloc((unsigned)((n + 2 * m + 255) / 256), 256);
  if (loc) {
    *t = (struct area_pointers *)(loc + n);
    (*t)->first = loc;
    (*t)->next = *s;
    *s = *t;
  } else
    gb_trouble_code |= 1;
  return loc;
}

void gb_free(Area s) {
  Area t;
  while (*s) {
    *t = (*s)->next;
    free((*s)->first);
    *s = *t;
  }
}
