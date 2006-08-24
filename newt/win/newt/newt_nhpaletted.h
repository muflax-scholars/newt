#ifndef NEWT_NHPALETTED_H
#define NEWT_NHPALETTED_H

#ifdef CHANGE_COLOR
void newt_change_colour(int colour, long rgb, int reverse);
char *newt_get_colour_string(void);
#ifdef MAC
void *newt_change_background(int white);
short newt_set_font_name(int wintype, char *font_name);
#endif
#endif

#endif
