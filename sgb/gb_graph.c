#include "gb_graph.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* <Preprocessor definitions> */

/* <Type declarations 8> */

/* <Private declarations 28> */
static Arc *next_arc;     /* the next Arc available for allocation */
static Arc *bad_arc;      /* but if next_arc = bad_arc, that Arc isn't there */
static char *next_string; /* the next byte available for storing a string */
static char *bad_string;  /* but if next_string = bad_string, don't byte */
static Arc dummy_arc[2];  /* an Arc record to point to in an emergency */
static Graph dummy_graph; /* a Graph record that's normally unused */
static Graph *cur_graph = &dummy_graph; /* the Graph most recently created */

/* <External declarations 5> */

long verbose = 0;    /* nonzero if "verbose" output is desired */
long panic_code = 0; /* set nonzero if graph generator returns null pointer */
long gb_trouble_code = 0; /* did gb_alloc return NULL? */
long extra_n = 4; /* the number of shadow vertices allocated by gb_new_graph */
char null_string[1]; /* a null string constant */
siz_t edge_trick = sizeof(Arc) - (sizeof(Arc) & (sizeof(Arc) - 1));

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

/* n: desired number of vertices */
Graph *gb_new_graph(long n) {
  cur_graph = (Graph *)calloc(1, sizeof(Graph));
  if (cur_graph) {
    cur_graph->vertices = gb_typed_alloc(n + extra_n, Vertex, cur_graph->data);
    if (cur_graph->vertices) {
      Vertex *p;
      cur_graph->n = n;
      for (p = cur_graph->vertices + n + extra_n - 1; p >= cur_graph->vertices;
           p--)
        p->name = null_string;
      sprintf(cur_graph->id, "gb_new_graph(%ld)", n);
      strcpy(cur_graph->util_types, "ZZZZZZZZZZZZZZ");
    } else {
      free((char *)cur_graph);
      cur_graph = NULL;
    }
  }
  next_arc = bad_arc = NULL;
  next_string = bad_string = NULL;
  gb_trouble_code = 0;
  return cur_graph;
}

/* g: graph whose id is to be set */
/* s1: string for the beginning of the new id */
/* gg: graph whose id is to be copied */
/* s2: string for the end of the new id */
void make_compound_id(Graph *g, char *s1, Graph *gg, char *s2) {
  int avail = ID_FIELD_SIZE - strlen(s1) - strlen(s2);
  char tmp[ID_FIELD_SIZE];
  strcpy(tmp, gg->id);
  if (strlen(tmp) < avail)
    sprintf(g->id, "%s%s%s", s1, tmp, s2);
  else
    sprintf(g->id, "%s%.*s...)%s", s1, avail - 5, tmp, s2);
}

/* g: graph whose id is to be set */
/* s1: string for the beginning of the new id */
/* gg: first graph whose id is to be copied */
/* s2: string for the middle of the new id */
/* ggg: second graph whose id is to be copied */
/* s3: string for the end of the new id */
void make_double_compound_id(Graph *g, char *s1, Graph *gg, char *s2,
                             Graph *ggg, char *s3) {
  int avail = ID_FIELD_SIZE - strlen(s1) - strlen(s2) - strlen(s3);
  if (strlen(gg->id) + strlen(ggg->id) < avail)
    sprintf(g->id, "%s%s%s%s%s", s1, gg->id, s2, ggg->id, s3);
  else sprintf(g->id, "%s%.*s...)%s%.*s...)%s", s1, avail / 2 - 5, gg->id,
               (avail - 9) / 2, ggg->id, s3);
}

#define arcs_per_block 102
Arc *gb_virgin_arc() {
  Arc *cur_arc = next_arc;
  if (cur_arc == bad_arc) {
    cur_arc = gb_typed_alloc(arcs_per_block, Arc, cur_graph->data);
    if (cur_arc == NULL) {
      cur_arc = dummy_arc;
    } else {
      next_arc = cur_arc + 1;
      bad_arc = cur_arc + arcs_per_block;
    }
  } else {
    next_arc++;
  }
  return cur_arc;
}

/* A newly created arc will go from u to v, and its length is len. */
void gb_new_arc(Vertex *u, Vertex *v, long len) {
    Arc *cur_arc = gb_virgin_arc();
    cur_arc->tip = v;
    cur_arc->next = u->arcs;
    cur_arc->len = len;
    u->arcs = cur_arc;
    cur_graph->m++;
}

/* New arcs will go from u to v and from v to u, and len is their length */
void gb_new_edge(Vertex *u, Vertex *v, long len) {
    Arc *cur_arc = gb_virgin_arc();
    if (cur_arc != dummy_arc) next_arc++;
    if (u < v) {
        cur_arc->tip = v;
        cur_arc->next = u->arcs;
        (cur_arc + 1)->tip = u;
        (cur_arc + 1)->next = v->arcs;
        u->arcs = cur_arc;
        v->arcs = cur_arc + 1;
    } else {
        (cur_arc + 1)->tip = v;
        (cur_arc + 1)->next = u->arcs;
        u->arcs = cur_arc + 1; /* do this now in case u = v */
        cur_arc->tip = u;
        cur_arc->next = v->arcs;
        v->arcs = cur_arc;
    }
    cur_arc->len = (cur_arc + 1)->len = len;
    cur_graph->m += 2;
}

#define string_block_size 1016 /* 1024 - 8 is usually efficient */

/* s is the string to be copied */
char *gb_save_string(char* s) {
    char *p = s;
    long len; /* length of the string and the following null charater */
    while (*p++); /* advance to the end of the string */
    len = p - s;
    p = next_string;
    if (p + len > bad_string) { /* not enough room in the current block */
        long size = string_block_size;
        if (len > size) size = len;
        p = gb_alloc(size, cur_graph->data);
        if (p == NULL) return null_string; /* return a pointer to "" if memory ran out */
        bad_string = p + size;
    }
    while (*s) *p++ = *s++; /* copy the non-null bytes of the string */
    *p++ = '\0'; /* and append a null character */
    next_string = p;
    return p - len;
}

void switch_to_graph(Graph *g) {
    cur_graph->ww.A = next_arc;
    cur_graph->xx.A = bad_arc;
    cur_graph->yy.S = next_string;
    cur_graph->zz.S = bad_string;
    cur_graph = (g ? g : &dummy_graph);
    next_arc = cur_graph->ww.A;
    bad_arc = cur_graph->xx.A;
    next_string = cur_graph->yy.S;
    bad_string = cur_graph->zz.S;
    cur_graph->ww.A = NULL;
    cur_graph->xx.A = NULL;
    cur_graph->yy.S = NULL;
    cur_graph->zz.S = NULL;
}

void gb_recycle(Graph *g) {
    if (g) {
        gb_free(g->data);
        gb_free(g->aux_data);
        free((char*)g); /* the user must not refer to g again */
    }
}

