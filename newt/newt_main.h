#ifndef NEWT_MAIN_H
#define NEWT_MAIN_H

#include <SDL.h>

void newt_win_init(void);
void newt_init_nhwindows (int *argcp, char **argv);
void newt_player_selection (void);
void newt_askname (void);
void newt_get_nh_event (void);
void newt_exit_nhwindows (const char *str);
void newt_suspend_nhwindows (const char *str);
void newt_resume_nhwindows (void);
void newt_curs (winid window, int x, int y);
void newt_update_inventory (void);
void newt_mark_synch (void);
void newt_wait_synch (void);
#ifdef CLIPPING
void newt_cliparound (int x, int y);
#endif
#ifdef POSITIONBAR
void newt_update_positionbar (char *features);
#endif
void newt_nhbell (void);
int newt_doprev_message (void);
char newt_yn_function (
    const char *quesl, const char *choices, CHAR_P def);
void newt_number_pad (int state);
void newt_delay_output (void);
void newt_outrip(winid window, int how);
void newt_preference_update(const char *pref);

void newt_stretch_fast(SDL_Surface *source_surface, SDL_Rect *source_rect, SDL_Surface *destination_surface);

#endif
