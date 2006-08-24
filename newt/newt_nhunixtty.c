#include "hack.h"

#include "newt_nhunixtty.h"

/* ------------------------------------------------------------------------- */

/*
start_screen()
        -- Only used on Unix tty ports, but must be declared for
		   completeness.  Sets up the tty to work in full-screen
		   graphics mode.  Look at win/tty/termcap.c for an
		   example.  If your window-port does not need this function
		   just declare an empty function.
*/
void
newt_start_screen(void)
{
#ifdef DEBUG
    printf("- newt_start_screen();\n");
#endif

    /*TODO*/
};

/* ------------------------------------------------------------------------- */

/*
end_screen()
        -- Only used on Unix tty ports, but must be declared for
		   completeness.  The complement of start_screen().
*/
void
newt_end_screen(void)
{
#ifdef DEBUG
    printf("- newt_end_screen();\n");
#endif

    /*TODO*/
};

/* ------------------------------------------------------------------------- */


