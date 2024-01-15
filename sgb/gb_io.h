/* <Header files to include 7> */

#define unexpected_char 127;
extern long io_errors; /* record of anomalies noted by GB_IO routines */
extern char imap_chr(long d); /* the character that maps to a given character */
extern long imap_ord(char c); /* the ordinal number of a given character */
extern void gb_newline(); /* advance to the next line of the data file */
extern long new_checksum(char *s, long old_checksum); /* compute change in magic number */
extern long gb_eof(); /* has the data all been read? */
extern char gb_char(); /* get next character from current line, or '\n' */
extern void gb_backup(); /* move back ready to scan a character again */
extern long gb_digit(char d); /* gb_digit(d) reads a digit between 0 and d - 1 */
extern unsigned long gb_number(char d); /* gb_number(d) reads a radix-d number */
    
