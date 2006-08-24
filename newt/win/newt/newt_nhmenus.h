#ifndef NEWT_MENUS_H
#define NEWT_MENUS_H

#define TEXTLINE_ACCEL_GROUP          0
#define TEXTLINE_ACCEL_CHAR           1
#define TEXTLINE_AMOUNT               2

#define TEXTLINE_ACCEL_USE_CHAR       10
#define TEXTLINE_HYPHEN               (TEXTLINE_ACCEL_USE_CHAR+2)
#define TEXTLINE_TEXT                 (TEXTLINE_ACCEL_USE_CHAR+4)

void newt_start_menu (winid window);
void newt_add_menu (
    winid window,
    int glyph,
    const anything* identifier,
	CHAR_P ch,
    CHAR_P gch,
    int attr,
    const char *str,
    BOOLEAN_P preselected);
void newt_end_menu (winid window, const char *prompt);
int  newt_select_menu (winid window, int how, menu_item ** menu_list);
char newt_message_menu (CHAR_P let, int how, const char *mesg);

#endif
