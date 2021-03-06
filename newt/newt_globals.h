#ifndef NEWT_GLOBALS_H
#define NEWT_GLOBALS_H

#include <SDL.h>
#include <SDL_ttf.h>

#define NEWT_VERSION_VERSION         1
#define NEWT_VERSION_RELEASE         3
#define NEWT_VERSION_PATCHLEVEL      0

#define NEWT_DEFAULT_TILESET         "nhtiles.bmp"
#define NEWT_DEFAULT_TILESET_WIDTH   16
#define NEWT_DEFAULT_TILESET_HEIGHT  16

#define NEWT_POSITIONBARMODE_START	0
#define NEWT_POSITIONBARMODE_NONE	0
#define NEWT_POSITIONBARMODE_FULL	1
#define NEWT_POSITIONBARMODE_ZOOM	2
#define NEWT_POSITIONBARMODE_END		2

#define NEWT_ZOOMMODE_START          0
#define NEWT_ZOOMMODE_NORMAL         0
#define NEWT_ZOOMMODE_FULLSCREEN     1
#define NEWT_ZOOMMODE_HORIZONTAL     2
#define NEWT_ZOOMMODE_VERTICAL       3
#define NEWT_ZOOMMODE_END            3
/* yes, CUSTOM *must* be after END ! */
#define NEWT_ZOOMMODE_CUSTOM         4

#define NEWT_ZOOMMULT                16384

#define NEWT_MORE "-- more --"

extern SDL_Surface *newt_screen;
extern SDL_Rect **VideoModes;
extern Uint32 VideoModeAmount;
extern Uint32 VideoMode;
extern Uint32 VideoBPP;

#ifdef USER_SOUNDS
extern Uint32 UseAudio;
#endif

extern SDL_Surface *newt_win_map;
extern SDL_Surface *newt_win_map_tiles;
extern SDL_Surface *newt_win_map_ascii;

extern Sint32 newt_win_map_ascii_fontwidth;
extern Sint32 newt_win_map_ascii_fontheight;


typedef struct {
    int character;
    int colour;
    unsigned int special;
} newt_ascii_mapchar;

extern newt_ascii_mapchar newt_ascii_map[ROWNO][COLNO];

#define NEWT_WINDOWQUEUESIZE        1024
extern winid newt_windowRenderQueue[NEWT_WINDOWQUEUESIZE];
extern Uint32 newt_windowRenderQueuePosition;
extern Uint32 newt_windowRenderQueueSize;

extern Uint32 newt_deltazoom;
extern SDL_Surface *newt_zoomed_map;

extern SDL_Rect newt_map_visibleRect;
extern SDL_Rect newt_screen_mapRect;

#ifdef LINEOFSIGHT
extern SDL_Surface *newt_lineofsight;
#endif
extern SDL_Surface *newt_infrared;
extern SDL_Surface *newt_telepathy;

extern TTF_Font *newt_font_map;
extern Sint32 newt_fontsize_map;
extern TTF_Font *newt_font_menu;
extern Sint32 newt_fontsize_menu;
extern TTF_Font *newt_font_message;
extern Sint32 newt_fontsize_message;
extern TTF_Font *newt_font_status;
extern Sint32 newt_fontsize_status;
extern TTF_Font *newt_font_text;
extern Sint32 newt_fontsize_text;

extern SDL_Surface *newt_tiles;

extern Sint32 newt_centre_x;
extern Sint32 newt_centre_y;

extern Sint32 newt_previouslevel;

extern Sint32 newt_map_curs_x;
extern Sint32 newt_map_curs_y;

extern char newt_positionbar[COLNO];

extern Uint32 newt_margin_left;
extern Uint32 newt_margin_right;

extern SDL_Color newt_Message_fg;
extern Uint32 newt_Message_bg;

extern SDL_Color newt_Status_fg;
extern Uint32 newt_Status_bg;

extern SDL_Color newt_Menu_fg;
extern Uint32 newt_Menu_bg;

extern Uint32 newt_Map_bg;

extern SDL_Color newt_Info_fg;
extern SDL_Color newt_Info_bg;

extern SDL_Color newt_Position_fg;
extern Uint32 newt_Position_bg;
extern Uint32 newt_Position_highlite;

extern Uint32 newt_Menu_Border;
extern Uint32 newt_Status_Border;
extern Uint32 newt_Message_Border;
extern Uint32 newt_Position_Border;

extern Uint32 newt_Map_curs_colour;
extern Uint32 newt_Pet_colour;

extern SDL_Color newt_Nethack_Colours[16];

extern char newt_status[2][2][BUFSZ];
extern Uint32 newt_status_curs_x;
extern Uint32 newt_status_curs_y;

extern char **newt_msghistory;
extern Sint32 newt_current_msghistory;
extern Sint32 newt_display_msghistory;

extern Uint32 newt_positionbarmode;

extern short glyph2tile[];

extern Sint32 newt_Zoom_x;
extern Sint32 newt_Zoom_y;

extern Uint32 newt_ZoomMode;

extern Uint32 newt_prevmessage_firstcall;
extern Uint32 newt_message_blank;

extern char newt_internalMessageText[BUFSZ];
extern Uint32 newt_internalMessageTime;
void newt_internalMessage(char *msg);

extern Uint32 newt_disallowScreenResize;

#endif
