#include "hack.h"

#include "newt_globals.h"

#include <SDL.h>
#include <SDL_ttf.h>

SDL_Surface *newt_screen;

SDL_Rect **VideoModes;
Uint32 VideoModeAmount=0;
Uint32 VideoMode=0;
Uint32 VideoBPP=32; // FIXME Changing this results in segfaults on startup

#ifdef USER_SOUNDS
Uint32 UseAudio=0;
#endif

SDL_Surface *newt_win_map=NULL;
SDL_Surface *newt_win_map_tiles=NULL;
SDL_Surface *newt_win_map_ascii=NULL;

Sint32 newt_win_map_ascii_fontwidth;
Sint32 newt_win_map_ascii_fontheight;

newt_ascii_mapchar newt_ascii_map[ROWNO][COLNO];

Uint32 newt_windowRenderQueue[NEWT_WINDOWQUEUESIZE];
Uint32 newt_windowRenderQueuePosition=0;
Uint32 newt_windowRenderQueueSize=0;

Uint32 newt_deltazoom=TRUE;
SDL_Surface *newt_zoomed_map=NULL;

SDL_Rect newt_map_visibleRect;
SDL_Rect newt_screen_mapRect;

#ifdef LINEOFSIGHT
SDL_Surface *newt_lineofsight;
#endif
SDL_Surface *newt_infrared;
SDL_Surface *newt_telepathy;

TTF_Font *newt_font_map;
Sint32 newt_fontsize_map=16;
TTF_Font *newt_font_menu;
Sint32 newt_fontsize_menu=14;
TTF_Font *newt_font_message;
Sint32 newt_fontsize_message=12;
TTF_Font *newt_font_status;
Sint32 newt_fontsize_status=14;
TTF_Font *newt_font_text;
Sint32 newt_fontsize_text=14;

SDL_Surface *newt_tiles;

Sint32 newt_centre_x=0;
Sint32 newt_centre_y=0;

Sint32 newt_map_curs_x=0;
Sint32 newt_map_curs_y=0;

char newt_positionbar[COLNO];

Uint32 newt_margin_left=0;
Uint32 newt_margin_right=0;

SDL_Color newt_Message_fg = {0xFF, 0xFF, 0xFF, 0};
Uint32 newt_Message_bg = 0x00202020;

SDL_Color newt_Status_fg = {0xFF, 0xFF, 0xFF, 0};
Uint32 newt_Status_bg = 0x00202020;

SDL_Color newt_Menu_fg = {0xFF, 0xFF, 0xFF, 0};
Uint32 newt_Menu_bg = 0x00000020;

Uint32 newt_Map_bg = 0x00602000;

SDL_Color newt_Info_fg = {0xFF, 0xFF, 0x00, 0};
SDL_Color newt_Info_bg = {0x00, 0x00, 0x00, 0};

SDL_Color newt_Position_fg = {0xFF, 0xFF, 0xFF, 0};
Uint32 newt_Position_bg = 0x00200800;
Uint32 newt_Position_highlite = 0x00804000;

Uint32 newt_Menu_Border = 0x008080A0;
Uint32 newt_Status_Border = 0x00A0A0A0;
Uint32 newt_Message_Border = 0x00A0A0A0;
Uint32 newt_Position_Border = 0x00A0A0A0;

Uint32 newt_Map_curs_colour = 0xFFFFFFFF;
Uint32 newt_Pet_colour = 0x00A00000;

SDL_Color newt_Nethack_Colours[16] = {
    {0x55, 0x55, 0x55, 0},
    {0xFF, 0x00, 0x00, 0},
    {0x00, 0x80, 0x00, 0},
    {0xA5, 0x2A, 0x2A, 0},
    {0x00, 0x00, 0xFF, 0},
    {0xFF, 0x00, 0xFF, 0},
    {0x00, 0xFF, 0xFF, 0},
    {0xC0, 0xC0, 0xC0, 0},
    {0x00, 0x00, 0x00, 0},
    {0xFF, 0xA5, 0x00, 0},
    {0x00, 0xFF, 0x00, 0},
    {0xFF, 0xFF, 0x00, 0},
    {0x00, 0xC0, 0xFF, 0},
    {0xFF, 0x80, 0xFF, 0},
    {0x80, 0xFF, 0xFF, 0},
    {0xFF, 0xFF, 0xFF, 0}
};

char newt_status[2][2][BUFSZ] = {
	{ " ", " "},
  { " ", " "}
};
Uint32 newt_status_curs_x=0;
Uint32 newt_status_curs_y=0;

char **newt_msghistory;
Sint32 newt_current_msghistory=0;
Sint32 newt_display_msghistory=0;

Sint32 newt_Zoom_x=NEWT_ZOOMMULT;
Sint32 newt_Zoom_y=NEWT_ZOOMMULT;

Uint32 newt_ZoomMode=NEWT_ZOOMMODE_NORMAL;

Uint32 newt_prevmessage_firstcall=TRUE;
Uint32 newt_message_blank=TRUE;

char newt_internalMessageText[BUFSZ]="";
Uint32 newt_internalMessageTime=0;
void newt_internalMessage(char *msg) {
    strncpy(newt_internalMessageText, msg, BUFSZ);
    newt_internalMessageTime=SDL_GetTicks();
}

Uint32 newt_disallowScreenResize = FALSE;

#ifdef POSITIONBAR
Uint32 newt_positionbarmode=NEWT_POSITIONBARMODE_FULL;
#else
Uint32 newt_positionbarmode=NEWT_POSITIONBARMODE_NONE;
#endif
