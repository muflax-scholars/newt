#include "hack.h"

#include "newt_nhwindows.h"
#include "newt_globals.h"
#include "newt_main.h"
#include "newt_nhinput.h"
#include "newt_nhoutput.h"
#include "newt_nhmenus.h"

#include <SDL.h>
#include <SDL_ttf.h>

/* ------------------------------------------------------------------------- */

/*
window = create_nhwindow(type)
		-- Create a window of type "type."

There are 5 basic window types, used to call create_nhwindow():

	NHW_MESSAGE	(top line)
	NHW_STATUS	(bottom lines)
	NHW_MAP		(main dungeon)
	NHW_MENU	(inventory or other "corner" windows)
	NHW_TEXT	(help/text, full screen paged window)

*/

winid
newt_create_nhwindow (type)
    int type;
{
    winid window=WIN_ERR;
	int counter;
	newt_window_rec *textwindow;

#ifdef DEBUG
    printf("- newt_create_nhwindow(");
    switch (type) {
        case NHW_MESSAGE:
            printf("NHW_MESSAGE");
            break;
        case NHW_STATUS:
            printf("NHW_STATUS");
            break;
        case NHW_MAP:
            printf("NHW_MAP");
            break;
        case NHW_MENU:
            printf("NHW_MENU");
            break;
        case NHW_TEXT:
            printf("NHW_TEXT");
            break;
        default:
            printf("NHW_*unknown*");
            break;
    }
    printf(");\n");
#endif

    switch (type) {
        case NHW_MESSAGE:
			if (iflags.msg_history<20) iflags.msg_history=20;
			if (iflags.msg_history>60) iflags.msg_history=60;
			newt_msghistory=malloc(sizeof(void *)*(iflags.msg_history*2));
			for (counter=0; counter<iflags.msg_history*2; counter++) {
				newt_msghistory[counter]=NULL;
			}
            window = (winid)newt_msghistory;
            break;
        case NHW_STATUS:
            window = (winid)&newt_status;
            break;
        case NHW_MAP:
            if (!newt_win_map_tiles) {
                newt_win_map_tiles=SDL_CreateRGBSurface(
                    SDL_SWSURFACE,
                    iflags.wc_tile_width*COLNO,
                    iflags.wc_tile_height*ROWNO,
                    VideoBPP,
                    newt_screen->format->Rmask,
                    newt_screen->format->Gmask,
                    newt_screen->format->Bmask,
                    newt_screen->format->Amask);
            }

            if (!newt_win_map_ascii) {
                TTF_SizeText( newt_font_map, "M", &newt_win_map_ascii_fontwidth, &newt_win_map_ascii_fontheight);
                newt_win_map_ascii=SDL_CreateRGBSurface(
                    SDL_SWSURFACE,
                    newt_win_map_ascii_fontwidth*COLNO,
                    newt_win_map_ascii_fontheight*ROWNO,
                    VideoBPP,
                    newt_screen->format->Rmask,
                    newt_screen->format->Gmask,
                    newt_screen->format->Bmask,
                    newt_screen->format->Amask);
            }

            newt_win_map=iflags.wc_tiled_map ? newt_win_map_tiles : newt_win_map_ascii;
            window = (winid)newt_win_map;
            break;
        case NHW_MENU:
        case NHW_TEXT:
			textwindow=malloc(sizeof(newt_window_rec));
			textwindow->type=NHW_TEXT;
			textwindow->textlines=NULL;
			textwindow->attrlines=NULL;
			textwindow->textline_amount=0;
			textwindow->textline_width=0;
			textwindow->title=NULL;
			textwindow->identifier=NULL;
			textwindow->glyph=NULL;

            window = (winid)textwindow;
            break;
    }

    return window;
}

/* ------------------------------------------------------------------------- */

/*
clear_nhwindow(window)
		-- Clear the given window, when appropriate.
*/
void
newt_clear_nhwindow (window)
    winid window;
{
    SDL_Rect dstrect;
	newt_window_rec *textwindow;
	int counter;

#ifdef DEBUG
    printf("- newt_clear_nhwindow(%d);\n", window);
#endif

    if (window==WIN_ERR) return;

    if (window==WIN_MAP) {
      int row, column;

      memset(newt_ascii_map, 0, sizeof(newt_ascii_map));
      for (row=0;row<ROWNO;row++) for (column=0;column<COLNO;column++) newt_print_glyph (window, column, row, cmap_to_glyph(S_stone));

        } else
	if (window==WIN_MESSAGE) {
        dstrect.x=0;
        dstrect.y=0;
        dstrect.w=newt_screen->w;
        dstrect.h=newt_fontsize_message;
        SDL_FillRect(newt_screen, &dstrect, newt_Message_bg);

		dstrect.x=0;
		dstrect.y=newt_fontsize_message-1;
		dstrect.w=newt_screen->w;
		dstrect.h=1;
		SDL_FillRect(newt_screen,&dstrect,newt_Message_Border);

        newt_message_blank=TRUE;
    } else
	if (window==WIN_STATUS) {
        dstrect.x=0;
        dstrect.y=newt_screen->h-(newt_fontsize_status*2)+1;
        dstrect.w=newt_screen->w;
        dstrect.h=newt_fontsize_status*2-1;
        SDL_FillRect(newt_screen, &dstrect, newt_Status_bg);

		dstrect.x=0;
		dstrect.y=newt_screen->h-(newt_fontsize_status*2);
		dstrect.w=newt_screen->w;
		dstrect.h=1;
		SDL_FillRect(newt_screen,&dstrect,newt_Status_Border);
	} else {
		textwindow=(newt_window_rec *)window;
		for (counter=0; counter<textwindow->textline_amount; counter++) {
			free(textwindow->textlines[counter]);
			if (textwindow->identifier) free(textwindow->identifier[counter]);
		}
		if (textwindow->textlines) free(textwindow->textlines);
		if (textwindow->attrlines) free(textwindow->attrlines);
		if (textwindow->identifier) free(textwindow->identifier);
		if (textwindow->glyph) free(textwindow->glyph);

		textwindow->textlines=NULL;
		textwindow->attrlines=NULL;
		textwindow->textline_amount=0;
		textwindow->textline_width=0;
		textwindow->title=NULL;
		textwindow->identifier=NULL;
		textwindow->glyph=NULL;

	}
}

/* ------------------------------------------------------------------------- */

void newt_windowQueueAdd(window)
    winid window;
{
    newt_windowRenderQueue_((newt_windowRenderQueuePosition+newt_windowRenderQueueSize++)&(NEWT_WINDOWQUEUESIZE-1))=window;
}

int newt_windowQueueRender()
{
    int cleanupForward;
    int cleanupBackward;
    int cleanupWindow;
    int rendered;

    if (!newt_windowRenderQueueSize) return 0;
    
    for (   cleanupBackward=newt_windowRenderQueuePosition+(newt_windowRenderQueueSize-1);
            cleanupBackward>newt_windowRenderQueuePosition;
            cleanupBackward--) {
        cleanupWindow=newt_windowRenderQueue_(cleanupBackward);
        for (   cleanupForward=newt_windowRenderQueuePosition;
                cleanupForward<cleanupBackward;
                cleanupForward++) {
            if (newt_windowRenderQueue_(cleanupForward)==cleanupWindow) newt_windowRenderQueue_(cleanupForward) = WIN_ERR; 
        }
    }

    rendered=0;
    if (newt_windowRenderQueueSize) {
        while (newt_windowRenderQueueSize) {
            if (newt_windowRenderQueue[newt_windowRenderQueuePosition]!=WIN_ERR) {
                newt_display_nhwindow(newt_windowRenderQueue_(newt_windowRenderQueuePosition), FALSE);
                rendered++;
            }
            newt_windowRenderQueuePosition=(newt_windowRenderQueuePosition+1)&(NEWT_WINDOWQUEUESIZE-1);
            newt_windowRenderQueueSize--;
        }
    }

    return rendered;
}

/* ------------------------------------------------------------------------- */
/*
display_nhwindow(window, boolean blocking)
		-- Display the window on the screen.  If there is data
		   pending for output in that window, it should be sent.
		   If blocking is TRUE, display_nhwindow() will not
		   return until the data has been displayed on the screen,
		   and acknowledged by the user where appropriate.
		-- All calls are blocking in the tty window-port.
		-- Calling display_nhwindow(WIN_MESSAGE,???) will do a
		   --more--, if necessary, in the tty window-port.
*/

void
newt_display_nhwindow (window, blocking)
    winid window;
    BOOLEAN_P blocking;
{
    SDL_Rect srcrect,dstrect;
    char *feature_loop;
    SDL_Surface *textsurface;
	int counter;
	newt_window_rec *textwindow;
	int pages, linesperpage, pagecounter;
	char *pagenum;
	int maparea_x,maparea_y;
	char *textline;
	char input;
    SDL_Rect zoom_window;
    SDL_Rect render_window;
    SDL_Rect cursor;
  int hasglyphs;
  SDL_Surface *glyph_surface_tmp;
  SDL_Surface *glyph_surface;
    SDL_Rect newt_screen_clipping;  

#ifdef DEBUG
    printf("- newt_display_nhwindow(%d, %d);\n", window, blocking);
#endif

    if (iflags.window_inited==FALSE) return;
    if (window==WIN_ERR) return;

    /* "window" is the result as you returned it from create_nhwindow */

    if (window==WIN_MAP) {

      if (!flags.perm_invent) newt_margin_left=0;
      maparea_y=(int)newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 2 : 3)*newt_fontsize_status+newt_fontsize_message);
      maparea_x=newt_screen->w-(newt_margin_left+newt_margin_right);

      if (maparea_x < 1 || maparea_y < 1) return;

        if (newt_ZoomMode!=NEWT_ZOOMMODE_FULLSCREEN) {
            /* clear background */
		dstrect.x=newt_margin_left;
		dstrect.y=newt_fontsize_message;
		dstrect.w=maparea_x;
		dstrect.h=maparea_y;
            SDL_FillRect(newt_screen, &dstrect, newt_Map_bg);
        }

        switch (newt_ZoomMode) {
            case NEWT_ZOOMMODE_NORMAL:
                newt_Zoom_x=NEWT_ZOOMMULT;
                newt_Zoom_y=NEWT_ZOOMMULT;
                dstrect.w=(newt_win_map->w*newt_Zoom_x)/NEWT_ZOOMMULT;
                dstrect.h=(newt_win_map->h*newt_Zoom_y)/NEWT_ZOOMMULT;
                break;
            case NEWT_ZOOMMODE_FULLSCREEN:
                newt_Zoom_x=(maparea_x*NEWT_ZOOMMULT)/(newt_win_map->w);
                newt_Zoom_y=
                    (maparea_y*NEWT_ZOOMMULT) /
                    (newt_win_map->h);
                dstrect.w=maparea_x;
                dstrect.h=maparea_y;
                break;
            case NEWT_ZOOMMODE_HORIZONTAL:
                newt_Zoom_x=(maparea_x*NEWT_ZOOMMULT)/(newt_win_map->w);
                newt_Zoom_y=newt_Zoom_x;
                dstrect.w=maparea_x;
                dstrect.h=(newt_win_map->h*newt_Zoom_y)/NEWT_ZOOMMULT;
                break;
            case NEWT_ZOOMMODE_VERTICAL:
                newt_Zoom_y=
                    (maparea_y*NEWT_ZOOMMULT) /
                    (newt_win_map->h);
                newt_Zoom_x=newt_Zoom_y;
                dstrect.w=(newt_win_map->w*newt_Zoom_x)/NEWT_ZOOMMULT;
                dstrect.h=maparea_y;
                break;
            case NEWT_ZOOMMODE_CUSTOM:
                dstrect.w=(newt_win_map->w*newt_Zoom_x)/NEWT_ZOOMMULT;
                dstrect.h=(newt_win_map->h*newt_Zoom_y)/NEWT_ZOOMMULT;
                break;
        }

        /* ensure zoomed (destination) size */
        if (dstrect.w>maparea_x) dstrect.w=maparea_x;
        if (dstrect.h>maparea_y) dstrect.h=maparea_y;

        /* create zoom destination surface */
        if (newt_zoomed_map) SDL_FreeSurface(newt_zoomed_map); 
        newt_zoomed_map=SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            dstrect.w,
            dstrect.h,
            VideoBPP,
            newt_win_map->format->Rmask,
            newt_win_map->format->Gmask,
            newt_win_map->format->Bmask,
            newt_win_map->format->Amask);

        /* calculate zoom source dimensions */
        srcrect.w=((Uint32)newt_zoomed_map->w*NEWT_ZOOMMULT)/newt_Zoom_x;
        srcrect.h=((Uint32)newt_zoomed_map->h*NEWT_ZOOMMULT)/newt_Zoom_y;
        if (srcrect.w>newt_win_map->w) srcrect.w=newt_win_map->w;
        if (srcrect.h>newt_win_map->h) srcrect.h=newt_win_map->h;

        /* calculate zoom source location */
        srcrect.x=((newt_centre_x*2+1)*newt_win_map->w)/(COLNO*2);
        srcrect.x-=(srcrect.w/2);
        srcrect.y=((newt_centre_y*2+1)*newt_win_map->h)/(ROWNO*2);
        srcrect.y-=(srcrect.h/2);

        /* zoom source bounds checking */
        if (srcrect.x+srcrect.w>=newt_win_map->w) srcrect.x=(newt_win_map->w-srcrect.w);
        if (srcrect.x<0) srcrect.x=0;
        if (srcrect.y+srcrect.h>=newt_win_map->h) srcrect.y=(newt_win_map->h-srcrect.h);
        if (srcrect.y<0) srcrect.y=0;

        zoom_window=srcrect;

        /* zoom */
        newt_map_visibleRect=srcrect;
        newt_stretch_fast(newt_win_map, (SDL_Rect *)&srcrect, newt_zoomed_map);

        /* zoom source & destination */
        srcrect.x=srcrect.y=0;
        srcrect.w=dstrect.w=newt_zoomed_map->w;
        srcrect.h=dstrect.h=newt_zoomed_map->h;

        /* centre zoom destination if smaller than screen */
        dstrect.x=(maparea_x-newt_zoomed_map->w)/2;
        if (dstrect.x<0) dstrect.x=0;
        if (dstrect.x+dstrect.w>maparea_x) dstrect.x=maparea_x-dstrect.w;

        dstrect.y=(maparea_y-newt_zoomed_map->h)/2;
        if (dstrect.y<0) dstrect.y=0;
        if (dstrect.y+dstrect.h>maparea_y) dstrect.y=maparea_y-dstrect.h;

        dstrect.x+=newt_margin_left;
        dstrect.y+=newt_fontsize_message;

        /* blit zoomed surface */
        newt_screen_mapRect=render_window=dstrect;
        SDL_BlitSurface(newt_zoomed_map, &srcrect, newt_screen, &dstrect);

        /* Cursor */

        cursor.w=(newt_win_map->w/COLNO)*newt_Zoom_x/NEWT_ZOOMMULT;
        cursor.h=(newt_win_map->h/ROWNO)*newt_Zoom_y/NEWT_ZOOMMULT;

		cursor.x=((newt_win_map->w*newt_Zoom_x/NEWT_ZOOMMULT)*(int)(newt_map_curs_x))/COLNO - zoom_window.x*newt_Zoom_x/NEWT_ZOOMMULT;
        cursor.x+=render_window.x;

		cursor.y=((newt_win_map->h*newt_Zoom_y/NEWT_ZOOMMULT)*(int)(newt_map_curs_y))/ROWNO - zoom_window.y*newt_Zoom_y/NEWT_ZOOMMULT;
        cursor.y+=render_window.y;

        SDL_GetClipRect(newt_screen,(SDL_Rect *)&newt_screen_clipping);
        srcrect.y=newt_fontsize_message;
        srcrect.x=newt_margin_left;
        srcrect.h=maparea_y;
        srcrect.w=newt_screen->w-(newt_margin_left+newt_margin_right);
        SDL_SetClipRect(newt_screen,(SDL_Rect *)&srcrect);
        
		dstrect.x=cursor.x;
		dstrect.y=cursor.y;
		dstrect.w=1;
		dstrect.h=cursor.h;
		SDL_FillRect(newt_screen,&dstrect,newt_Map_curs_colour);
		dstrect.x=cursor.x;
		dstrect.y=cursor.y;
		dstrect.w=cursor.w;
		dstrect.h=1;
		SDL_FillRect(newt_screen,&dstrect,newt_Map_curs_colour);
		dstrect.x=cursor.x+cursor.w;
		dstrect.y=cursor.y;
		dstrect.w=1;
		dstrect.h=cursor.h;
		SDL_FillRect(newt_screen,&dstrect,newt_Map_curs_colour);
		dstrect.x=cursor.x;
		dstrect.y=cursor.y+cursor.h;
		dstrect.w=cursor.w;
		dstrect.h=1;
		SDL_FillRect(newt_screen,&dstrect,newt_Map_curs_colour);

        SDL_SetClipRect(newt_screen,(SDL_Rect *)&newt_screen_clipping);

        /* Positionbar */
		if (newt_positionbarmode!=NEWT_POSITIONBARMODE_NONE) {
            /* background */
			dstrect.x=0;
			dstrect.y=newt_screen->h-(newt_fontsize_status*3);
			dstrect.w=newt_screen->w;
			dstrect.h=newt_fontsize_status;
			SDL_FillRect(newt_screen, &dstrect, newt_Position_bg);

            /* highlight */
			if (newt_positionbarmode==NEWT_POSITIONBARMODE_FULL) {
                dstrect.x=(zoom_window.x*newt_screen->w)/newt_win_map->w;
                dstrect.w=(zoom_window.w*newt_screen->w)/newt_win_map->w;
            } else
            if (newt_positionbarmode==NEWT_POSITIONBARMODE_ZOOM) {
                dstrect.x=render_window.x;
                dstrect.w=render_window.w;
            }
				dstrect.y=newt_screen->h-(newt_fontsize_status*3);
				dstrect.h=newt_fontsize_status;
				SDL_FillRect(newt_screen, &dstrect, newt_Position_highlite);

			dstrect.x=0;
			dstrect.y=newt_screen->h-(newt_fontsize_status*3);
			dstrect.w=newt_screen->w;
			dstrect.h=1;
			SDL_FillRect(newt_screen,&dstrect,newt_Position_Border);

			feature_loop=newt_positionbar;
			while (*feature_loop) {
				textsurface = TTF_RenderGlyph_Blended(
					newt_font_status, *feature_loop++, newt_Position_fg);

				if (newt_positionbarmode==NEWT_POSITIONBARMODE_FULL) {
					dstrect.x=(newt_screen->w*(int)(*feature_loop++))/COLNO;
                    dstrect.x+=((newt_screen->w/COLNO)-textsurface->w)/2;
				} else {
					dstrect.x=((newt_win_map->w*newt_Zoom_x/NEWT_ZOOMMULT)*(int)(*feature_loop++))/COLNO - zoom_window.x*newt_Zoom_x/NEWT_ZOOMMULT;
                    dstrect.x+=((iflags.wc_tile_width*newt_Zoom_x/NEWT_ZOOMMULT)-textsurface->w)/2;
                    dstrect.x+=render_window.x;
				}

				dstrect.y=newt_screen->h-(newt_fontsize_status*3)+((newt_fontsize_status-textsurface->h)/2);
				srcrect.x=srcrect.y=0;
				srcrect.w=textsurface->w;
				srcrect.h=textsurface->h;
				dstrect.w=textsurface->w;
				dstrect.h=textsurface->h;
				SDL_BlitSurface(textsurface, &srcrect, newt_screen, &dstrect);

                SDL_FreeSurface(textsurface);
			}

		}

    } else
	if (window==WIN_MESSAGE) {
    if (!newt_message_blank) {
		newt_clear_nhwindow(window);
    newt_message_blank=FALSE;

		if (newt_msghistory[(newt_display_msghistory)*2+0]&&strlen(newt_msghistory[(newt_display_msghistory)*2+0])) {
			textsurface = TTF_RenderText_Blended(
				newt_font_message, newt_msghistory[(newt_display_msghistory)*2+0], newt_Message_fg);
			srcrect.x=srcrect.y=0;
			srcrect.w=textsurface->w;
			srcrect.h=textsurface->h;
			dstrect.x=0;
			dstrect.y=0;
			dstrect.w=newt_screen->w;
			dstrect.h=newt_fontsize_message;

			SDL_BlitSurface(textsurface, &srcrect, newt_screen, &dstrect);
            SDL_FreeSurface(textsurface);
		}
  }
	} else
	if (window==WIN_STATUS) {

		newt_clear_nhwindow(window);

		for (counter=0; counter<2; counter++) {
			textsurface = TTF_RenderText_Blended(
				newt_font_status, newt_status[counter][0], newt_Status_fg);
			srcrect.x=srcrect.y=0;
			srcrect.w=textsurface->w;
			srcrect.h=textsurface->h;
			dstrect.x=0;
			dstrect.y=newt_screen->h-((2-counter)*newt_fontsize_status);
			dstrect.w=newt_screen->w;
			dstrect.h=newt_fontsize_status;

			SDL_BlitSurface(textsurface, &srcrect, newt_screen, &dstrect);
            SDL_FreeSurface(textsurface);
		}
	} else {
    int start_page;
    int end_page;

        Sint32 newt_fontsize = newt_fontsize_text; /* FIXME should be _menu for menus */
        TTF_Font *newt_font = newt_font_text; /* FIXME should be _menu for menus */

		blocking=FALSE;

		textwindow=(newt_window_rec *)window;

		linesperpage=(newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 2 : 3)*newt_fontsize_status+newt_fontsize_message))/newt_fontsize;
		linesperpage--;
		if (linesperpage<1) linesperpage=1;

		pages=(textwindow->textline_amount+(linesperpage-1))/linesperpage;

    if (flags.perm_invent && window==WIN_INVEN) {
      start_page=0;
      end_page=start_page+1;
    } else {
      start_page=0;
      end_page=pages;
    }

		for (pagecounter=start_page; pagecounter<end_page; pagecounter++) {

      hasglyphs=FALSE;
      for (counter=0; counter<linesperpage; counter++) {
        if (pagecounter*linesperpage+counter>=textwindow->textline_amount) break;
        hasglyphs=hasglyphs||(textwindow->glyph && textwindow->glyph[pagecounter*linesperpage+counter]!=NO_GLYPH);
      }

      if (hasglyphs && (newt_win_map!=newt_win_map_ascii)) {
        hasglyphs=newt_fontsize+4;
      } else {
        hasglyphs=0;
      }

      if (flags.perm_invent && window==WIN_INVEN) {
        newt_margin_left=textwindow->textline_width+6+4+hasglyphs;
      } else {
        newt_margin_right=textwindow->textline_width+6+4+hasglyphs;
      }
      newt_display_nhwindow(WIN_MAP,FALSE);

			/* background */
			dstrect.x=(flags.perm_invent && window==WIN_INVEN) ? 0 : newt_screen->w-(textwindow->textline_width+6+4+hasglyphs);
			dstrect.y=newt_fontsize_message;
			dstrect.w=textwindow->textline_width+6+4+hasglyphs;
			dstrect.h=newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 2 : 3)*newt_fontsize_status+newt_fontsize_message);
			SDL_FillRect(newt_screen, &dstrect, newt_Menu_bg);

			/* seperator */
			dstrect.x= (flags.perm_invent && window==WIN_INVEN) ? (textwindow->textline_width+5+4+hasglyphs) : newt_screen->w-(textwindow->textline_width+5+4+hasglyphs);
			dstrect.y=newt_fontsize_message;
			dstrect.w=1;
			dstrect.h=newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 2 : 3)*newt_fontsize_status+newt_fontsize_message);
			SDL_FillRect(newt_screen,&dstrect,newt_Menu_Border);

          glyph_surface_tmp=SDL_CreateRGBSurface(
            SDL_HWSURFACE | SDL_SRCALPHA,
            iflags.wc_tile_width,
            iflags.wc_tile_height,
            newt_screen->format->BitsPerPixel,
            newt_screen->format->Rmask,
            newt_screen->format->Gmask,
            newt_screen->format->Bmask,
            newt_screen->format->Amask);

          dstrect.w=dstrect.h=newt_fontsize;
          glyph_surface=SDL_CreateRGBSurface(
            SDL_HWSURFACE | SDL_SRCALPHA,
            dstrect.w,
            dstrect.h,
            newt_screen->format->BitsPerPixel,
            newt_screen->format->Rmask,
            newt_screen->format->Gmask,
            newt_screen->format->Bmask,
            newt_screen->format->Amask);

			for (counter=0; counter<linesperpage; counter++) {
				if (pagecounter*linesperpage+counter>=textwindow->textline_amount) break;

				if (textwindow->type==NHW_MENU) {
					textline=&textwindow->textlines[pagecounter*linesperpage+counter][TEXTLINE_ACCEL_USE_CHAR];
				} else {
					textline=textwindow->textlines[pagecounter*linesperpage+counter];
				}
				if (!strlen(textline)) continue;

          if (hasglyphs && textwindow->glyph && textwindow->glyph[pagecounter*linesperpage+counter]!=NO_GLYPH)
          {
              int background_glyph = cmap_to_glyph(S_room);

              dstrect.w=srcrect.w=iflags.wc_tile_width;
              dstrect.h=srcrect.h=iflags.wc_tile_height;
              srcrect.x=(glyph2tile[background_glyph]%(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_width;
              srcrect.y=(glyph2tile[background_glyph]/(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_height;
              dstrect.x=dstrect.y=0;
              SDL_BlitSurface(newt_tiles, (SDL_Rect *)&srcrect, glyph_surface_tmp, (SDL_Rect *)&dstrect);

              dstrect.w=srcrect.w=iflags.wc_tile_width;
              dstrect.h=srcrect.h=iflags.wc_tile_height;
              srcrect.x=(glyph2tile[textwindow->glyph[pagecounter*linesperpage+counter]]%(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_width;
              srcrect.y=(glyph2tile[textwindow->glyph[pagecounter*linesperpage+counter]]/(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_height;
              dstrect.x=dstrect.y=0;
              SDL_BlitSurface(newt_tiles, (SDL_Rect *)&srcrect, glyph_surface_tmp, (SDL_Rect *)&dstrect);

              srcrect.w=iflags.wc_tile_width;
              srcrect.h=iflags.wc_tile_height;
              srcrect.x=0;
              srcrect.y=0;
              newt_stretch_fast(glyph_surface_tmp, (SDL_Rect *)&srcrect, glyph_surface);

              srcrect.x=srcrect.y=0;
              srcrect.w=dstrect.w=glyph_surface->w;
              srcrect.h=dstrect.h=glyph_surface->h;
              dstrect.x=(flags.perm_invent && window==WIN_INVEN) ? 1 : newt_screen->w-(textwindow->textline_width+4+hasglyphs);
              dstrect.y=(counter+1+(textwindow->title ? 1 : 0))*newt_fontsize;
              SDL_BlitSurface(glyph_surface, &srcrect, newt_screen, &dstrect);
          }

				textsurface = TTF_RenderText_Blended(
					newt_font, textline, newt_Menu_fg);
				srcrect.x=srcrect.y=0;
				srcrect.w=textsurface->w;
				srcrect.h=textsurface->h;
				dstrect.x=(flags.perm_invent && window==WIN_INVEN) ? 1 + hasglyphs : newt_screen->w-(textwindow->textline_width+4);
				dstrect.y=(counter)*newt_fontsize+newt_fontsize_message;
				dstrect.w=textwindow->textline_width;
				dstrect.h=newt_fontsize;

				SDL_BlitSurface(textsurface, &srcrect, newt_screen, &dstrect);
				SDL_FreeSurface(textsurface);

			}

        SDL_FreeSurface(glyph_surface);
        SDL_FreeSurface(glyph_surface_tmp);

      if (!(flags.perm_invent && window==WIN_INVEN)) {
			pagenum=malloc(BUFSZ);
			sprintf(pagenum, "(Page %d of %d)", pagecounter+1, pages);
				textsurface = TTF_RenderText_Blended(
					newt_font, pagenum, newt_Menu_fg);
				srcrect.x=srcrect.y=0;
				srcrect.w=textsurface->w;
				srcrect.h=textsurface->h;
				dstrect.x=newt_screen->w-(textsurface->w+4);
				dstrect.y=newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 2 : 3)*newt_fontsize_status+newt_fontsize_message);
				dstrect.w=textsurface->w;
				dstrect.h=newt_fontsize;

				SDL_BlitSurface(textsurface, &srcrect, newt_screen, &dstrect);
				SDL_FreeSurface(textsurface);
			free(pagenum);

			input=newt_nhgetch();
  newt_margin_right=0;
  newt_display_nhwindow(WIN_MAP,FALSE);
			switch (input) {
				case '\033':
					pagecounter=pages;
					break;
                case MENU_FIRST_PAGE: /* top page of menu */
					pagecounter=-1;
                    break;
                case MENU_LAST_PAGE: /* bottom page of menu */
					pagecounter=pages-2;
                    break;
                case MENU_PREVIOUS_PAGE: /* page up */
					pagecounter-=2;
					if (pagecounter<-1) pagecounter=-1;
                    break;
                case MENU_NEXT_PAGE: /* page down */
					if (pagecounter>pages-2) pagecounter=pages-2;
				default:
                    break;
			}
      }
		}
	}

    /*TODO*/

    if (blocking&&(window!=WIN_MESSAGE)) {
        while (newt_nhgetch()!=' ') {};
    }
}

/* ------------------------------------------------------------------------- */

/*
destroy_nhwindow(window)
		-- Destroy will dismiss the window if the window has not
		   already been dismissed.
*/
void
newt_destroy_nhwindow (window)
    winid window;
{
    int counter;
	newt_window_rec *textwindow;

#ifdef DEBUG
    printf("- newt_destroy_nhwindow(%d);\n", window);
#endif
	if (window==WIN_MAP) {
/*
        sprintf(filename, "%s_%02d_%03d.bmp", plname, u.uz.dnum, u.uz.dlevel);
        SDL_SaveBMP( newt_win_map, filename);
*/
        SDL_FreeSurface(newt_win_map);
        newt_win_map=(SDL_Surface *)(WIN_MAP=WIN_ERR);

	} else
	if (window==WIN_MESSAGE) {
    WIN_MESSAGE=WIN_ERR;
	} else
	if (window==WIN_STATUS) {
    WIN_STATUS=WIN_ERR;
	} else
	{
		textwindow=(newt_window_rec *)window;
        for (counter=0; counter<textwindow->textline_amount; counter++)
            free(textwindow->textlines[counter]);
        free(textwindow->textlines);
        if (textwindow->identifier) {
            for (counter=0; counter<textwindow->textline_amount; counter++)
                free(textwindow->identifier[counter]);
            free(textwindow->identifier);
        }
        if (textwindow->glyph) free(textwindow->glyph);
        if (textwindow->title) free(textwindow->title);
		free(textwindow);
    if (window==WIN_INVEN) WIN_INVEN=WIN_ERR;
	}

  if (WIN_MAP!=WIN_ERR) newt_display_nhwindow(WIN_MAP,FALSE);
  if (flags.perm_invent&&WIN_INVEN!=WIN_ERR) newt_display_nhwindow(WIN_INVEN,FALSE);

}

/* ------------------------------------------------------------------------- */


