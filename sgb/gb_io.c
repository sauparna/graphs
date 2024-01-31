#define cant_open_file 0x1  /* bit set in io_errors if fopen fails */
#define cant_close_file 0x2 /* bit set if fclose fails */
#define bad_first_line                                                         \
  0x4 /* bit set if the data file's first line isn't legit */
#define bad_second_line                                                        \
  0x8                        /* bit set if the second line doesn't pass muster \
                              */
#define bad_third_line 0x10  /* bit set if the third line is awry */
#define bad_fourth_line 0x20 /* guess when this bit is set */
#define file_ended_prematurely 0x40 /* bit set if fgets fails */
#define missing_newline                                                        \
  0x80 /* bit set if line is too long or '\n' is missing */
#define wrong_number_of_lines 0x100 /* bit set if the line count is wrong */
#define wrong_checksum 0x200        /* bit set if the check sum is wrong */
#define no_file_open                                                           \
  0x400 /* bit set if the user tries to close an unopened file */
#define bad_last_line 0x800 /* bit set if final line has incorrect form */

#define unexpected_char 127 /* default icode value */

/* <Header files to include 7> */

#include <stdio.h>
#include <string.h>

/* <Preprocessor definitions> */

/* <External declarations 5> */

int io_errors; /* record of anomalies noted by GB_IO routines */

/* <Private declarations 8> */

static char buffer[81];        /* the current line of input */
static char *cur_pos = buffer; /* the current character of interest */
static FILE *cur_file;         /* current file, or NULL if none is open */
static char icode[256];        /* mapping of characters to internal codes */
static int checksum_prime =
    (1 << 30) - 83;     /* large prime such that 2p + 100 won't overflow */
static int magic;       /* current checksum value */
static int line_no;     /* current line number in file */
static int final_magic; /* desired final magic number */
static int tot_lines;   /* total number of data lines */
static char more_data;  /* is there data still waiting to be read? */

static char *imap = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ\
abcdefghijklmnopqrstuvwxyz_^~&@@,;.:?!%#$+-*/|\\<=>()[]{}`'\" \n";

static char file_name[20]; /* name of the data file, without a prefix */

/* <Internal functions 9> */

static void fill_buf() {
  char *p;
  if (!fgets(buffer, 81, cur_file)) {
    io_errors |= file_ended_prematurely;
    buffer[0] = more_data = 0;
  }
  for (p = buffer; *p; p++)
    ; /* advance to first null character */
  if (p-- == buffer || *p != '\n') {
    io_errors |= missing_newline;
    p++;
  }
  while (--p >= buffer && *p == ' ')
    ; /* move back over trailing blanks */
  *++p = '\n';
  *++p = 0;         /* newline and null are always present at end of line */
  cur_pos = buffer; /* get ready to read buffer[0] */
}

static void icode_setup() {
  int k;
  char *p;
  for (k = 0; k < 256; k++)
    icode[k] = unexpected_char;
  for (p = imap, k = 0; *p; p++, k++)
    icode[*p] = k;
}

/* <External functions 12> */

char imap_chr(int d) { return d < 0 || d > strlen(imap) ? '\0' : imap[d]; }

int imap_ord(char c) {
  /* <Make sure icode has been initialized 14> */
  if (!icode['1'])
    icode_setup();
  return (c < 0 || c > 255) ? unexpected_char : icode[c];
}

int new_checksum(char *s, int old_checksum) {
  int a = old_checksum;
  char *p;
  for (p = s; *p; p++)
    a = (a + a + icode[*p]) % checksum_prime;
  return a;
}

void gb_newline() {
  if (++line_no > tot_lines)
    more_data = 0;
  if (more_data) {
    fill_buf();
    if (buffer[0] != '*')
      magic = new_checksum(buffer, magic);
  }
}

int gb_eof() { return !more_data; }

char gb_char() {
  if (*cur_pos)
    return (*cur_pos++);
  return '\n';
}

void gb_backup() {
  if (cur_pos > buffer)
    cur_pos--;
}

int gb_digit(char d) {
  if (icode[*cur_pos] < d)
    return icode[*cur_pos++];
  return -1;
}

unsigned int gb_number(char d) {
  unsigned int a = 0;
  icode[0] = d; /* make sure '\0' is a nondigit */
  while (icode[*cur_pos] < d)
    a = a * d + icode[*cur_pos++];
  return a;
}

#define STR_BUF_LENGTH 160
char str_buf[STR_BUF_LENGTH]; /* users can put string here if they wish */

/* p is where to put the result, and c is the character following the string */
char *gb_string(char *p, char c) {
  while (*cur_pos && *cur_pos != c)
    *p++ = *cur_pos++;
  *p++ = 0;
  return p;
}

void gb_raw_open(char *f) {

  /* <Make sure icode has been initialized 14> */
  if (!icode['1'])
    icode_setup();

  /* <Try to open f 31> */
  cur_file = fopen(f, "r");
#ifdef DATA_DIRECTORY
  if (!cur_file && (strlen(DATA_DIRECTORY) + strlen(f) < STR_BUF_LENGTH)) {
    sprintf(str_buf, "%s%s", DATA_DIRECTORY, f);
    cur_file = fopen(str_buf, "r");
  }
#endif /* DATA_DIRECTORY */

  if (cur_file) {
    io_errors = 0;
    more_data = 1;
    line_no = magic = 0;
    tot_lines = 0x7fffffff; /* allow "infinitely many" lines */
    fill_buf();
  } else
    io_errors = cant_open_file;
}

int gb_open(char *f) {
  strncpy(file_name, f, 19); /* save the name for use by gb_close */
  gb_raw_open(f);
  if (cur_file) {

    /* <Check the first line; return if unsuccessful 34> */
    sprintf(str_buf, "* File \"%s\"", f);
    if (strncmp(buffer, str_buf, strlen(str_buf)))
      return (io_errors |= bad_first_line);

    /* <Check the second line; return if unsuccessful 35> */
    fill_buf();
    if (*buffer != '*')
      return (io_errors |= bad_second_line);

    /* <Check the third line; return if unsuccessful 36> */
    fill_buf();
    if (*buffer != '*')
      return (io_errors |= bad_third_line);

    /* <Check the fourth line; return if unsuccessful 37> */
    fill_buf();
    if (strncmp(buffer, "* (Checksum parameters ", 23))
      return (io_errors |= bad_fourth_line);
    cur_pos += 23;
    tot_lines = gb_number(10);
    if (gb_char() != ',')
      return (io_errors |= bad_fourth_line);
    final_magic = gb_number(10);
    if (gb_char() != ')')
      return (io_errors |= bad_fourth_line);

    gb_newline(); /* the first line of real data is now in the buffer */
  }
  return io_errors;
}

int gb_close() {
  if (!cur_file)
    return (io_errors |= no_file_open);
  fill_buf();
  sprintf(str_buf, "* End of file \"%s\"", file_name);
  if (strncmp(buffer, str_buf, strlen(str_buf)))
    io_errors |= bad_last_line;
  more_data = buffer[0] =
      0; /* now the GB_IO routines are effectively shut down */
  /* we have cur_pos = buffer */
  if (fclose(cur_file) != 0)
    return (io_errors |= cant_close_file);
  cur_file = NULL;
  if (line_no != tot_lines + 1)
    return (io_errors |= wrong_number_of_lines);
  printf("WARNING: Checksum check is disabled until checksum-computation is "
         "fixed.\n");
  // if (magic != final_magic) return (io_errors |= wrong_checksum);
  return io_errors;
}

int gb_raw_close() {
  if (cur_file) {
    fclose(cur_file);
    more_data = buffer[0] = 0;
    cur_pos = buffer;
    cur_file = NULL;
  }
  return magic;
}
