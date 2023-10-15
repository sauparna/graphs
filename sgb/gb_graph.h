#include <stdio.h>
#include <string.h>

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

#define ID_FIELD_SIZE 161
typedef struct graph_struct {
  Vertex *vertices;            /* beginning of the vertex array */
  long n;                      /* total number of vertices */
  long m;                      /* total number of arcs */
  char id[ID_FIELD_SIZE];      /* GraphBase identification */
  char util_types[15];         /* usage of utility fields */
  Area data;                   /* the main data blocks */
  Area aux_data;               /* subsidiary data blocks */
  util uu, vv, ww, xx, yy, zz; /* multipurpose fields */
} Graph;

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

extern long verbose; /* nonzero if "verbose" output is desired */
extern long
    panic_code; /* set nonzero if graph generator returns null pointer */
extern long gb_trouble_code; /* anomalies noted by gb_alloc */
extern long
    extra_n; /* the number of shadow vertices allocated by gb_new_graph */
extern char null_string[]; /* a null string constant */

extern void make_compound_id(); /* routine to set one id field from another */
extern void make_double_compound_id(); /* ditto, but from two others */

#define alloc_fault (-1)    /* a previous memory request failed */
#define no_room 1           /* the current memory request failed */
#define early_data_fault 10 /* error detected at beginning of .dat file */
#define late_data_fault 11  /* error detected at end of .dat file */
#define syntax_error 20     /* error detected while reading .dat file */
#define bad_specs 30        /* parameter out of range or otherwise disallowed */
#define very_bad_specs 40   /* parameter far out of range or otherwise stupid */
#define missing_operand 50  /* graph parameter is NULL */
#define invalid_operand 60  /* graph parameter doesn't obey assumptions */
#define impossible 90       /* "this can't happen" */

extern char *gb_alloc(); /* allocate another block for an area */
#define gb_typed_alloc(n, t, s) (t *)gb_alloc((long)((n) * sizeof(t)), s)
extern void gb_free(); /* deallocate all blocks for an area */

#define n_1 uu.I /* utility field uu may denote size of bipartite first part   \
                  */
#define mark_bipartite(g, n1) g->n_1 = n1, g->util_types[8] = 'I'
