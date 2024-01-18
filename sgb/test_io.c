#include "gb_io.h"
#define exit_test(m) { fprintf(stderr, "%s!\n(Error code = %d)\n", m, io_errors); return -1; }

int main() {
    /* <Test the gb_open routine; exit if there's trouble 28> */
    if (gb_open("test.dat") != 0) exit_test("Can't open test.dat");
    
    
    /* <Test the sample data lines; exit if there's trouble 27> */
    if (gb_number(10) != 123456789) io_errors |= 1 << 20; /* decimal number not working */
    if (gb_digit(16) != 10) io_errors |= 1 << 21; /* we missed the A following the decimal number */
    gb_backup(); gb_backup(); /* get set to read '9A' again */
    if (gb_number(16) != 0x9ABCDEF) io_errors |= 1 << 22; /* hexadecimal number not working */
    gb_newline(); /* now we should be scanning a blank line */
    if (gb_char() != '\n') io_errors |= 1 << 23; /* newline not inserted at end */
    if (gb_char() != '\n') io_errors |= 1 << 24; /* newline not implied after end */
    if (gb_number(60) != 0) io_errors |= 1 << 25; /* number should stop at null character */
    {
        char temp[100];
        if (gb_string(temp, '\n') != temp + 1) io_errors |= 1 << 26; /* string should be null after end of line */
        gb_newline();
        if (gb_string(temp, ':') != temp + 5 || strcmp(temp, "Oops")) io_errors |= 1 << 27; /* string not read properly */
    }
    if (io_errors) exit_test("Sorry, it failed. Look at the error code for clues");
    if (gb_digit(10) != -1) exit_test("Digit error not detected");
    if (gb_char() != ':') io_errors |= 1 << 28; /* lost synch after gb_string and gb_digit */
    if (gb_eof()) io_errors |= 1 << 29; /* premature end of file indication */
    gb_newline();
    if (!gb_eof()) io_errors |= 1 << 30; /* postmature end of file indication */
    
    /* <Test the gb_close routine; exit if there's trouble 38> */
    if (gb_close() != 0) exit_test("Bad checksum, or difficulty closing the file");
    
    printf("OK, the gb_io routines seem to work!\n");
    return 0;
}
