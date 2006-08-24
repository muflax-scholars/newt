#ifndef NEWT_NHOUTPUT_H
#define NEWT_NHOUTPUT_H

void newt_putstr (winid window, int attr, const char *str);
void newt_display_file (const char *fname, BOOLEAN_P complain);
void newt_print_glyph (winid window, int x, int y, int glyph);
void newt_raw_print (const char *str);
void newt_raw_print_bold (const char *str);

#endif
