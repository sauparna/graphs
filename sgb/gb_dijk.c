#include "gb_dijk.h"
#include "gb_graph.h"

/* <Preprocessor definitions> */

#define dist                                                                   \
  z.I /* distance from uu, modified by hh, appears in vertex utility field z   \
       */
#define backlink y.V /* pointer to previous vertex appears in utility field y  \
                      */
#define hh_val x.I   /* computed value of hh(v) */
#define llink v.V    /* llink is stored in utility field v of a vertex */
#define rlink w.V    /* rlink is stored in utility field w of a vertex */

/* <Priority queue procedures 16> */

Vertex head[128]; /* list-head elements that are always present */

void init_dlist(long d) {
  head->llink = head->rlink = head;
  head->dist = d - 1; /* a value guaranteed to be smaller than any actual key */
}

void enlist(Vertex *v, long d) {
  Vertex *t = head->llink;
  v->dist = d;
  while (d < t->dist)
    t = t->llink;
  v->llink = t;
  (v->rlink = t->rlink)->llink = v;
  t->rlink = v;
}

void reenlist(Vertex *v, long d) {
  Vertex *t = v->llink;
  (t->rlink = v->rlink)->llink = v->llink; /* remove v */
  v->dist = d; /* we assume that the new dist is smaller than it was before */
  while (d < t->dist)
    t = t->llink;
  v->llink = t;
  (v->rlink = t->rlink)->llink = v;
  t->rlink = v;
}

Vertex *del_first() {
  Vertex *t;
  t = head->rlink;
  if (t == head)
    return NULL;
  (head->rlink = t->rlink)->llink = head;
  return t;
}

long master_key; /* smallest key that may be present in the priority queue */

void init_128(long d) {
  Vertex *u;
  master_key = d;
  for (u = head; u < head + 128; u++)
    u->llink = u->rlink = u;
}

Vertex *del_128() {
  long d;
  Vertex *u, *t;
  for (d = master_key; d < master_key + 128; d++) {
    u = head + (d & 0x7f); /* that'd d % 128 */
    t = u->rlink;
    if (t != u) { /* we found a nonempty list with minimum key */
      master_key = d;
      (u->rlink = t->rlink)->llink = u;
      return t; /* incidentally, t->dist = d */
    }
  }
  return NULL; /* all 128 lists are empty */
}

/* v is the new vertex for the queue and d its dist */
void enq_128(Vertex *v, long d) {
  Vertex *u = head + (d & 0x7f);
  v->dist = d;
  (v->llink = u->llink)->rlink = v;
  v->rlink = u;
  u->llink = v;
}

/* v is the vertex to be moved to another list, and d is its new dist */
void req_128(Vertex *v, long d) {
  Vertex *u = head + (d & 0x7f);
  (v->llink->rlink = v->llink)->llink = v->llink; /* remove v */
  v->dist = d; /* the new dist is smaller than it was before */
  (v->llink = u->llink)->rlink = v;
  v->rlink = u;
  u->llink = v;
  if (d < master_key)
    master_key = d; /* not needed for Dijkstra's algorithm */
}

/* <Global declarations 8> */

long dummy(Vertex *v) { return 0; }
void (*init_queue)(long) = init_dlist; /* create an empty dlist */
void (*enqueue)(Vertex *, long) = enlist;
; /* insert a new element in dlist */
void (*requeue)(Vertex *,
                long) = reenlist; /* decrease the key of an element in dlist */
Vertex *(*del_min)() = del_first; /* remove element with the smallest key */

/* <The dijkstra procedure 9> */

/* uu is the starting point, vv the ending point, gg is the graph they belong
 * to, hh is the heuristic function */
long dijkstra(Vertex *uu, Vertex *vv, Graph *gg, long (*hh)(Vertex *)) {
  Vertex *t; /* current vertex of interest */
  if (hh == NULL)
    hh = dummy; /* change to default heuristic */

  /* <Make uu the only vertex seen; also make it known 10> */
  for (t = gg->vertices + gg->n - 1; t >= gg->vertices; t--)
    t->backlink = NULL;
  uu->backlink = uu;
  uu->dist = 0;
  uu->hh_val = (*hh)(uu);
  (*init_queue)(0L); /* make the priority queue empty */

  t = uu;
  if (verbose) { /* <Print initial message 12> */
    printf("Distances from %s", uu->name);
    if (hh != dummy)
      printf(" [%ld]", uu->hh_val);
    printf(":\n");
  }
  while (t != vv) {
    /* <Put all unseen vertices adjacent to t into the queue, and update the
     * distances of other vertices adjacent to t 11> */
    {
      Arc *a; /* an arc leading from t */
      long d = t->dist - t->hh_val;
      for (a = t->arcs; a; a = a->next) {
        Vertex *v = a->tip; /* a vertex adjacent to t */
        if (v->backlink) {  /* v has already been seen */
          long dd = d + a->len + v->hh_val;
          if (dd < v->dist) {
            v->backlink = t;
            (*requeue)(v, dd); /* we found a better way to get there */
          }
        } else { /* v hasn't been seen before */
          v->hh_val = (*hh)(v);
          v->backlink = t;
          (*enqueue)(v, d + a->len + v->hh_val);
        }
      }
    }

    t = (*del_min)();
    if (t == NULL)
      return -1;   /* if the queue becomes empty, there's no way to get to vv */
    if (verbose) { /* <Print the distance to t 13> */
      printf(" %ld to %s", t->dist - t->hh_val + uu->hh_val, t->name);
      if (hh != dummy)
        printf(" [%ld]", t->hh_val);
      printf(" via %s\n", t->backlink->name);
    }
  }
  return vv->dist - vv->hh_val + uu->hh_val; /* true distance from uu to vv */
}

/* <The print_dijkstra_result procedure 14> */

void print_dijkstra_result(Vertex *vv) {
  Vertex *t, *p, *q; /* pointers for reversing links */
  t = NULL;
  p = vv;
  if (!p->backlink) {
    printf("Sorry, %s is unreachable.\n", p->name);
    return;
  }
  do { /* pop an item from p to t */
    q = p->backlink;
    p->backlink = t;
    t = p;
    p = q;
  } while (t != p); /* the loop stops with t = p = uu */
  do {
    printf("%10ld %s\n", t->dist - t->hh_val + p->hh_val, t->name);
    t = t->backlink;
  } while (t);
  t = p;
  do { /* pop an item from t to p */
    q = t->backlink;
    t->backlink = p;
    p = t;
    t = q;
  } while (p != vv);
}
