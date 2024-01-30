extern long
dijkstra(Vertex *uu, Vertex *vv, Graph *gg,
         long (*hh)(Vertex *)); /* procedure to calculate the shorted paths */
extern void
print_dijkstra_result(Vertex *vv); /* procedure to display the answer */

#define dist z.I
#define backlink y.V
#define hh_val x.I
#define llink v.V
#define rlink w.V

extern Vertex head[];

extern void (*init_queue)(
    long); /* create an empty priority queue for dijkstra */
extern void (*enqueue)(Vertex *,
                       long); /* insert a new element in the priority queue */
extern void (*requeue)(Vertex *,
                       long); /* decrease the key of an element in the queue */
extern Vertex *(*del_min)();  /* remove an element with the smallest key */

extern void init_dlist(long);
extern void enlist(Vertex *, long);
extern void reenlist(Vertex *, long);
extern Vertex *del_first();
extern void init_128(long);
extern Vertex *del_128();
extern void enq_128(Vertex *, long);
extern void req_128(Vertex *, long);
