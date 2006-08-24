#ifndef NEWT_NHWINDOWS_H
#define NEWT_NHWINDOWS_H

#include <SDL.h>

winid newt_create_nhwindow (int type);
void newt_clear_nhwindow (winid window);
void newt_display_nhwindow (winid window, BOOLEAN_P blocking);
void newt_destroy_nhwindow (winid window);

typedef struct {
	Uint32 type;

	Uint32 textline_amount;
	Uint32 textline_width;
	char **textlines;
	char **attrlines;

	anything **identifier;
	int *glyph;

	char *title;
} newt_window_rec;

#endif
