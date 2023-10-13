#define gb_next_rand() (*gb_fptr >= 0 ? *gb_fptr-- : gb_flip_cycle())
extern long *gb_fptr;        /* the next A value to be used */
extern long gb_flip_cycle(); /* compute 55 more pseudo-random numbers */
#define mod_diff(x, y) (((x) - (y)) & 0x7fffffff) /* difference modulo 2^31 */
extern void gb_init_rand();
extern long gb_unif_rand();
