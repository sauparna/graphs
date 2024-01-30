/* <Header files to include 7> */
#include <stdio.h>
#include <string.h>

#define unexpected_char 127;
extern int io_errors;        /* record of anomalies noted by GB_IO routines */
extern char imap_chr(int d); /* the character that maps to a given character */
extern int imap_ord(char c); /* the ordinal number of a given character */
extern void gb_newline();    /* advance to the next line of the data file */
extern int new_checksum(char *s,
                        int old_checksum); /* compute change in magic number */
extern int gb_eof();                       /* has the data all been read? */
extern char gb_char();       /* get next character from current line, or '\n' */
extern void gb_backup();     /* move back ready to scan a character again */
extern int gb_digit(char d); /* gb_digit(d) reads a digit between 0 and d - 1 */
extern unsigned int gb_number(char d); /* gb_number(d) reads a radix-d number */

#define STR_BUF_LENGTH 160
extern char str_buf[]; /* safe place to receive output of gb_string */
extern char *gb_string(char *p,
                       char c); /* gb_string(p, c) reads a string delimited by c
                                   into bytes starting at p */

extern void gb_raw_open(char *f); /* open a file for GraphBase input */
extern int gb_open(char *f); /* open a GraphBase data file; return 0 if OK */
extern int gb_close();       /* close a GraphBase data file; return 0 if OK */
extern int gb_raw_close();   /* close file and return the checksum */
