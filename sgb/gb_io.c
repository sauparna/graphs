#define cant_open_file 0x1 /* bit set in io_errors if fopen fails */
#define cant_close_file 0x2 /* bit set if fclose fails */
#define bad_first_line 0x4 /* bit set if the data file's first line isn't legit */
#define bad_second_line 0x8 /* bit set if the second line doesn't pass muster */
#define bad_third_line 0x10 /* bit set if the third line is awry */
#define bad_fourth_line 0x20 /* guess when this bit is set */
#define file_ended_prematurely 0x40 /* bit set if fgets fails */
#define missing_newline 0x80 /* bit set if line is too long or '\n' is missing */
#define wrong_number_of_lines 0x100 /* bit set if the line count is wrong */
#define wrong_checksum 0x200 /* bit set if the check sum is wrong */
#define no_file_open 0x400 /* bit set if the user tries to close an unopened file */
#define bad_last_line 0x800 /* bit set if final line has incorrect form */

#define unexpected_char 127 /* default icode value */

/* <Header files to include 7> */

#include <stdio.h>
#include <string.h>

/* <Preprocessor definitions> */

/* <External declarations 5> */

long io_errors; /* record of anomalies noted by GB_IO routines */

/* <Private declarations 8> */

static char buffer[81]; /* the current line of input */
static char *cur_pos = buffer; /* the current character of interest */
static FILE *cur_file; /* current file, or NULL if none is open */
static char icode[256]; /* mapping of characters to internal codes */
static long checksum_prime = (1L << 30) - 83; /* large prime such that 2p + 100 won't overflow */
static long magic; /* current checksum value */
static long line_no; /* current line number in file */
static long final_magic; /* desired final magic number */
static long tot_lines; /* total number of data lines */
static char more_data; /* is there data still waiting to be read? */

static char *imap = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ\
abcdefghijklmnopqrstuvwxyz_^~&@@,;.:?!%#$+-*/|\\<=>()[]{}`'\" \n";

/* <Internal functions 9> */

static void fill_buf() {
    char *p;
    if (!fgets(buffer, 81, cur_file)) {
        io_errors |= file_ended_prematurely;
        buffer[0] = more_data = 0;
    }
    for (p = buffer; *p; p++); /* advance to first null character */
    if (p-- == buffer || *p != '\n') {
        io_errors |= missing_newline;
        p++;
    }
    while (--p >= buffer && *p == ' '); /* move back over trailing blanks */
    *++p = '\n';
    *++p = 0; /* newline and null are always present at end of line */
    cur_pos = buffer; /* get ready to read buffer[0] */
}

static void icode_setup() {
    long k;
    char *p;
    for (k = 0; k < 256; k++) icode[k] = unexpected_char;
    for (p = imap, k = 0; *p; p++, k++) icode[*p] = k;
}

/* <External functions 12> */

char imap_chr(long d) {
    return d < 0 || d > strlen(imap) ? '\0' : imap[d];
}

long imap_ord(char c) {
    /* <Make sure icode has been initialized 4> */
    if (!icode['1'])
        icode_setup();
    return (c < 0 || c > 255) ? unexpected_char : icode[c];
}

long new_checksum(char *s, long old_checksum) {
    long a = old_checksum;
    char *p;
    for (p = s; *p; p++) a = (a + a + icode[*p]) % checksum_prime;
    return a;
}

void gb_newline() {
    if (++line_no > tot_lines) more_data = 0;
    if (more_data) {
        fill_buf();
        if (buffer[0] != '*') magic = new_checksum(buffer, magic);
    }
}

long gb_eof() {
    return !more_data;
}


