
#include "com_err.h"
#include "error_table.h"
#include "internal.h"

static const char char_set[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";

static char buf[6];

const char * error_table_name(num)
    errcode_t num;
{
    int ch;
    int i;
    char *p;

    /* num = aa aaa abb bbb bcc ccc cdd ddd d?? ??? ??? */
    p = buf;
    num >>= ERRCODE_RANGE;
    /* num = ?? ??? ??? aaa aaa bbb bbb ccc ccc ddd ddd */
    num &= 077777777L;
    /* num = 00 000 000 aaa aaa bbb bbb ccc ccc ddd ddd */
    for (i = 4; i >= 0; i--) {
	ch = (int)((num >> BITS_PER_CHAR * i) & ((1 << BITS_PER_CHAR) - 1));
	if (ch != 0)
	    *p++ = char_set[ch-1];
    }
    *p = '\0';
    return(buf);
}
