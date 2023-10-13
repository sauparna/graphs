#include "gb_graph.h"

/* <Declaration of test variables 19> */

Area s;

int main() {

  /* <Create a small graph 36> */

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

  printf("OK, the gb_graph routines seem to work!\n");

  return 0;
}
