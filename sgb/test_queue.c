#include "gb_dijk.h"
#include "gb_graph.h"

#define FAIL 0
#define PASS 1

void print_priority_queue();
int test_head_and_tail_links();

void print_priority_queue() {
  if (head->rlink == head) {
    fprintf(stderr, "Can't print, the priority queue is empty!\n");
    return;
  }
  int c = 0;
  for (Vertex *t = head->rlink; t != head; t = t->rlink) {
    fprintf(stderr, "%4ld ", t->dist);
    c++;
    if (c % 20 == 0)
      fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n[queue size = %d]\n", c);
}

int test_head_and_tail_links() {
  if (head->llink->rlink == head) {
    fprintf(stderr, "  PASS: The head's llink points to the tail and the "
                    "tail's rlink points to the head.\n");
    return PASS;
  } else {
    fprintf(stderr, "  FAIL: The head and tail don't point to each other.!\n");
    return FAIL;
  }
}

int main() {
  int test_count = 0;

  (*init_queue)(0L);
  Vertex v[1024];
  for (int i = 1; i < 1024; i++) {
    (*enqueue)(v + i, i);
  }
  fprintf(stderr, "Inserted 1023 vertices into the priority queue, where each "
                  "element's dist is in ascending order, in [1, 1023].\n");

  fprintf(stderr,
          "%d. Test the queue structure after 1023 enqueue operations.\n",
          ++test_count);

  if (head->rlink->dist == 1) {
    fprintf(stderr, "  PASS: The first element has dist = 1.\n");
  } else {
    fprintf(stderr, "  FAIL: The first element's dist is not 1.!\n");
    return -1;
  }
  if (head->llink->dist == 1023) {
    fprintf(stderr, "  PASS: The tail element has dist = 1023.\n");
  } else {
    fprintf(stderr, "  FAIL: The tail element's dist is not 1023!\n");
    return -2;
  }
  if (test_head_and_tail_links() == FAIL)
    return -3;
  fprintf(stderr, "Insert an element with dist = 0.\n");
  (*enqueue)(&v[0], 0);
  if (head->rlink->dist == 0) {
    fprintf(stderr, "  PASS: enqueue queued the smallest element correctly; "
                    "the first element has dist = 0.\n");
  } else {
    fprintf(stderr, "  FAIL: The smallest element enqueue queued could not be "
                    "found at the correct location!\n");
    return -4;
  }
  if (test_head_and_tail_links() == FAIL)
    return -5;

  fprintf(stderr, "The queue looks structurally sound, which asserts that "
                  "enqueu works correctly.\n");

  fprintf(stderr, "%d. Test the requeue operation.\n", ++test_count);
  {
    Vertex *old_tail = head->llink;
    (*requeue)(head->llink, 2);
    Vertex *requeued_element = head->rlink->rlink->rlink->rlink;
    if (requeued_element == old_tail) {
      fprintf(
          stderr,
          "  PASS: The tail element was requeued to the correct position.\n");
    } else {
      fprintf(stderr, "  FAIL: The tail element is not to be found in the "
                      "expected position after a requeue!\n");
      return -6;
    }
    if (requeued_element->dist == 2) {
      fprintf(stderr, "  PASS: The requeued element's dist is correct.\n");
    } else {
      fprintf(stderr,
              "  FAIL: The requeued element's dist was not correctly set.\n");
      return -7;
    }
    if (test_head_and_tail_links() == FAIL)
      return -8;
  }
  fprintf(stderr, "requeue works correctly.\n");

  fprintf(stderr, "%d. Test the del_min operation.\n", ++test_count);
  {
    Vertex *old_first_element = head->rlink;
    Vertex *old_second_element = head->rlink->rlink;
    Vertex *t = del_min();
    if (t == old_first_element) {
      fprintf(stderr, "  PASS: The first element was correctly removed.\n");
    } else {
      fprintf(stderr, "  FAIL: The wrong element was removed!\n");
      return -9;
    }
    if (head->rlink == old_second_element) {
      fprintf(stderr, "  PASS: del_min left the queue in the correct state.\n");
    } else {
      fprintf(stderr,
              "  FAIL: del_min left the queue in an inconcistent state!\n");
      return -10;
    }
    if (test_head_and_tail_links() == FAIL)
      return -11;
  }
  fprintf(stderr, "del_min works correctly.\n");

  fprintf(stderr,
          "OK, the priority queue implementation in GB_DIJK seem to work! "
          "(Passes all %d tests.)\n",
          test_count);

  return 0;
}
