#include "gb_graph.h"

/* <Declaration of test variables 19> */

Area s;
Graph *g;
Vertex *u, *v;

int main() {

    /* <Create a small graph 36> */
    g = gb_new_graph(2);
    if (g == NULL) {
        fprintf(stderr, "Oops, I couldn't even create a trivial graph!\n");
        return -4;
    }
    u = g->vertices;
    v = u + 1;
    u->name = gb_save_string("vertex 0");
    v->name = gb_save_string("vertex 1");

    /* <Test some intentional errors 18> */

    if (gb_alloc(0, s) != NULL || gb_trouble_code != 2) {
        fprintf(stderr, "Allocation error 2 wasn't reported properly!\n");
        return -2;
    }
    for (; g->vv.I < 100; g->vv.I++)
        if (gb_alloc(100000, s))
            g->uu.I++;
    if (g->uu.I < 100 && gb_trouble_code != 3) {
        fprintf(stderr, "Allocation error 1 wasn't reported properly!\n");
        return -1;
    }
    if (g->uu.I == 0) {
        fprintf(stderr, "I couldn't allocate any memory!\n");
        return -3;
    }
    gb_free(s);
    printf("Hey, I allocated %ld00000 bytes successfully. Terrific...\n",
           g->uu.I);
    gb_trouble_code = 0;

    /* <Check that the small graph is still there 38> */
    if (strncmp(u->name, v->name, 7)) {
        fprintf(stderr, "Something is fouled up in the string storage machinery!\n");
        return -5;
    }
    gb_new_edge(v, u, -1);
    gb_new_edge(u, u, 1);
    gb_new_arc(v, u, -1);
    if ((edge_trick & (siz_t)(u->arcs)) || (edge_trick & (siz_t)(u->arcs->next->next)) || !(edge_trick & (size_t)(v->arcs->next)))
        printf("Warning: The \"edge trick\" failed!\n");
    if(v->name[7] + g->n != v->arcs->next->tip->name[7] + g->m - 2) { /* '1' + 2 != '0' + 5 - 2 */
        fprintf(stderr, "Sorry the graph data structures aren't working yet.\n");
        return -6;
    }

    printf("OK, the gb_graph routines seem to work!\n");

    return 0;
}
