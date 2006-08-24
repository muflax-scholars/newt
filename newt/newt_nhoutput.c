#include "hack.h"
#include "decl.h"
#include "dlb.h"

#include "newt_nhoutput.h"
#include "newt_nhinput.h"
#include "newt_nhwindows.h"
#include "newt_globals.h"
#include "newt_nhmenus.h"

#include <SDL.h>
#include <SDL_ttf.h>
#ifdef USER_SOUNDS
# include <SDL_mixer.h>
#endif

/* ------------------------------------------------------------------------- */

/*
putstr(window, attr, str)
		-- Print str on the window with the given attribute.  Only
		   printable ASCII characters (040-0126) must be supported.
		   Multiple putstr()s are output on separate lines.  Attributes
		   can be one of
			ATR_NONE (or 0)
			ATR_ULINE
			ATR_BOLD
			ATR_BLINK
			ATR_INVERSE
		   If a window-port does not support all of these, it may map
		   unsupported attributes to a supported one (e.g. map them
		   all to ATR_INVERSE).  putstr() may compress spaces out of
		   str, break str, or truncate str, if necessary for the
		   display.  Where putstr() breaks a line, it has to clear
		   to end-of-line.
		-- putstr should be implemented such that if two putstr()s
		   are done consecutively the user will see the first and
		   then the second.  In the tty port, pline() achieves this
		   by calling more() or displaying both on the same line.
*/
void
newt_putstr (window, attr, str)
    winid window;
    int attr;
    const char *str;
{
	int counter;
	char *newstr;
	int width, height;
	newt_window_rec *textwindow;
  int textlinewidth;

#ifdef DEBUG
    printf("- newt_putstr(%d, %d, \"%s\");\n", window, attr, str);
#endif

    /* "window" is the result as you returned it from create_nhwindow */

    /* safety first ;) */
    if (window==WIN_ERR) {
        if (attr==ATR_BOLD) {
            newt_raw_print_bold(str);
        } else {
            newt_raw_print(str);
        }
        return;
    }

	if (window==WIN_MESSAGE) {

#if defined(USER_SOUNDS)
        play_sound_for_message(str);
#endif

        newt_prevmessage_firstcall=TRUE;

		if (newt_current_msghistory) {
			newstr=malloc(strlen(newt_msghistory[(iflags.msg_history-1)*2+0])+1+strlen(str)+1+strlen(NEWT_MORE)+1);
			sprintf(newstr,"%s %s %s",newt_msghistory[(iflags.msg_history-1)*2+0],str,NEWT_MORE);
			TTF_SizeText(newt_font, newstr, &width, &height);
			free(newstr);
			if (width<=newt_screen->w) {
				newstr=malloc(strlen(newt_msghistory[(iflags.msg_history-1)*2+0])+1+strlen(str)+1);
				sprintf(newstr,"%s %s",newt_msghistory[(iflags.msg_history-1)*2+0],str);
				free(newt_msghistory[(iflags.msg_history-1)*2+0]);
				newt_msghistory[(iflags.msg_history-1)*2+0]=newstr;
                newt_display_msghistory=iflags.msg_history-1;
        newt_message_blank=FALSE;
				newt_display_nhwindow(WIN_MESSAGE, FALSE);
				return;
			} else {
				newstr=malloc(strlen(newt_msghistory[(iflags.msg_history-1)*2+0])+1+strlen(NEWT_MORE)+1);
				sprintf(newstr,"%s %s",newt_msghistory[(iflags.msg_history-1)*2+0],NEWT_MORE);
				free(newt_msghistory[(iflags.msg_history-1)*2+0]);
				newt_msghistory[(iflags.msg_history-1)*2+0]=newstr;
                newt_display_msghistory=iflags.msg_history-1;
        newt_message_blank=FALSE;
				newt_display_nhwindow(WIN_MESSAGE, TRUE);
        while (newt_nhgetch()!=' ') {};

			}
		}

		if (newt_msghistory[0]) {
			free(newt_msghistory[0]);
			newt_msghistory[0]=NULL;
		}
		if (newt_msghistory[1]) {
			free(newt_msghistory[1]);
			newt_msghistory[1]=NULL;
		}

		for (counter=0; counter<iflags.msg_history-1; counter++) {
			newt_msghistory[counter*2+0]=newt_msghistory[(counter+1)*2+0];
			newt_msghistory[counter*2+1]=newt_msghistory[(counter+1)*2+1];
		}

		newt_msghistory[(iflags.msg_history-1)*2+0]=malloc(strlen(str)+1);
		newt_msghistory[(iflags.msg_history-1)*2+1]=malloc(strlen(str)+1);

		strcpy(newt_msghistory[(iflags.msg_history-1)*2+0],str);

		if (strlen(str)) newt_current_msghistory++;

        newt_display_msghistory=iflags.msg_history-1;
    newt_message_blank=FALSE;
		newt_display_nhwindow(WIN_MESSAGE, FALSE);
    } else
	if (window==WIN_STATUS) {
		strcpy(
			&newt_status[newt_status_curs_y][0][newt_status_curs_x],
			str);
		newt_status_curs_x=0;
		newt_status_curs_y++;
		if (newt_status_curs_y>1) newt_status_curs_y=0;
		newt_display_nhwindow(WIN_STATUS, FALSE);
    } else
	if (window==WIN_MAP) {
			/* hmmmmmm */
    } else
	{
		textwindow=(newt_window_rec *)window;
		textwindow->textline_amount++;
		textwindow->textlines=realloc(textwindow->textlines,sizeof(char *)*textwindow->textline_amount);
    if (textwindow->type==NHW_MENU) {
      textlinewidth=TEXTLINE_ACCEL_USE_CHAR+strlen(&str[TEXTLINE_ACCEL_USE_CHAR])+1;
      textwindow->textlines[textwindow->textline_amount-1]=malloc(textlinewidth);
      memcpy(textwindow->textlines[textwindow->textline_amount-1],str,textlinewidth);
      TTF_SizeText(newt_font, &str[TEXTLINE_ACCEL_USE_CHAR], &width, &height);
    } else {
      textwindow->textlines[textwindow->textline_amount-1]=malloc(strlen(str)+1);
      strcpy(textwindow->textlines[textwindow->textline_amount-1],str);
      TTF_SizeText(newt_font, str, &width, &height);
    }
		if (width>textwindow->textline_width) textwindow->textline_width=width;
	};

    /*TODO*/
};

/* ------------------------------------------------------------------------- */

/*
display_file(str, boolean complain)
		-- Display the file named str.  Complain about missing files
		   if complain is TRUE.
*/
void
newt_display_file (fname, complain)
    const char *fname;
    BOOLEAN_P complain;
{
	dlb *f;
	winid window;
	char buffer[BUFSZ];
	char *ch;

	#ifdef DEBUG
    printf("- newt_display_file(\"%s\", %d);\n", fname, complain);
#endif

	if ((f=dlb_fopen(fname,"r"))) {
		window=newt_create_nhwindow(NHW_TEXT);
		while (dlb_fgets(buffer,BUFSZ,f)) {
			if ((ch=index(buffer,'\n'))) *ch=0;
			if ((ch=index(buffer,'\r'))) *ch=0;

			newt_putstr(window,0,buffer);
		}

		newt_display_nhwindow(window,TRUE);
		newt_destroy_nhwindow(window);

		dlb_fclose(f);
	} else {
		printf("WARNING: could not open and display (dlb)file: '%s'\n",fname);
	}
};


/* ------------------------------------------------------------------------- */

/*
print_glyph(window, x, y, glyph)
		-- Print the glyph at (x,y) on the given window.  Glyphs are
		   integers at the interface, mapped to whatever the window-
		   port wants (symbol, font, colour, attributes, ...there's
		   a 1-1 map between glyphs and distinct things on the map).
*/
void
newt_print_glyph (window, x, y, glyph)
    winid window;
    int x;
    int y;
    int glyph;
{
    SDL_Rect srcrect, dstrect;
    int background_glyph;
    SDL_Surface *textsurface;
    char shortstring[2];
    int invert_text;

#ifdef DEBUG
    printf("- newt_print_glyph(%d, %d, %d, %d);\n",
        window, x, y, glyph);
#endif

    /*TODO*/

    /* retrieving text/ascii display information */
    mapglyph (
        glyph,
        (int *)&newt_ascii_map[y][x].character,
        (int *)&newt_ascii_map[y][x].colour,
        (unsigned int *)&newt_ascii_map[y][x].special,
        x, y);

    if (!iflags.use_color) newt_ascii_map[y][x].colour=CLR_WHITE;

    shortstring[1]=0;
    shortstring[0]=newt_ascii_map[y][x].character;

    invert_text=((newt_ascii_map[y][x].special&MG_PET) && iflags.hilite_pet);

    srcrect.x=srcrect.y=0;
    srcrect.w=dstrect.w=newt_win_map_ascii_fontwidth;
    srcrect.h=dstrect.h=newt_win_map_ascii_fontheight;
    dstrect.x=newt_win_map_ascii_fontwidth*x;
    dstrect.y=newt_win_map_ascii_fontheight*y;
    if (invert_text)
    {
        SDL_FillRect(
            newt_win_map_ascii,
            &dstrect,
            SDL_MapRGB(
                newt_win_map_ascii->format,
                newt_Nethack_Colours[newt_ascii_map[y][x].colour].r,
                newt_Nethack_Colours[newt_ascii_map[y][x].colour].g,
                newt_Nethack_Colours[newt_ascii_map[y][x].colour].b));
        textsurface = TTF_RenderText_Blended(
            newt_font, shortstring, newt_Nethack_Colours[NO_COLOR]);
    } else
    {
        SDL_FillRect(
            newt_win_map_ascii,
            &dstrect,
            SDL_MapRGB(
                newt_win_map_ascii->format,
                newt_Nethack_Colours[NO_COLOR].r,
                newt_Nethack_Colours[NO_COLOR].g,
                newt_Nethack_Colours[NO_COLOR].b));
        textsurface = TTF_RenderText_Blended(
            newt_font, shortstring, newt_Nethack_Colours[newt_ascii_map[y][x].colour]);
    }
    SDL_BlitSurface(textsurface, &srcrect, newt_win_map_ascii, &dstrect);
    SDL_FreeSurface(textsurface);

    background_glyph = cmap_to_glyph(S_room);

     srcrect.w=dstrect.w=iflags.wc_tile_width;
     srcrect.h=dstrect.h=iflags.wc_tile_height;

     dstrect.x=x*iflags.wc_tile_width;
     dstrect.y=y*iflags.wc_tile_height;

     srcrect.x=(glyph2tile[background_glyph]%(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_width;
     srcrect.y=(glyph2tile[background_glyph]/(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_height;

     SDL_BlitSurface(newt_tiles, &srcrect, newt_win_map_tiles, &dstrect);

    background_glyph = back_to_glyph(x,y);

    srcrect.w=dstrect.w=iflags.wc_tile_width;
     srcrect.h=dstrect.h=iflags.wc_tile_height;

     dstrect.x=x*iflags.wc_tile_width;
     dstrect.y=y*iflags.wc_tile_height;

     srcrect.x=(glyph2tile[background_glyph]%(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_width;
     srcrect.y=(glyph2tile[background_glyph]/(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_height;

     SDL_BlitSurface(newt_tiles, &srcrect, newt_win_map_tiles, &dstrect);

     srcrect.w=dstrect.w=iflags.wc_tile_width;
     srcrect.h=dstrect.h=iflags.wc_tile_height;

     dstrect.x=x*iflags.wc_tile_width;
     dstrect.y=y*iflags.wc_tile_height;

     srcrect.x=(glyph2tile[glyph]%(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_width;
     srcrect.y=(glyph2tile[glyph]/(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_height;

     SDL_BlitSurface(newt_tiles, &srcrect, newt_win_map_tiles, &dstrect);

#ifdef LINEOFSIGHT
    if (iflags.wc_lineofsight) {
	if (!couldsee(x,y) || !cansee(x,y) || glyph==cmap_to_glyph(S_corr)) {
	    srcrect.x=srcrect.y=0;
	    srcrect.w=dstrect.w;
	    srcrect.h=dstrect.h;
	    SDL_BlitSurface(newt_lineofsight, &srcrect, newt_win_map_tiles, &dstrect);
	}
    }
#endif

    if ( glyph_is_monster( glyph ) && !cansee(x,y) ) {
        register struct monst *mon;
        mon = m_at(x,y);
        if ( mon && see_with_infrared( mon) ) {
            srcrect.x=srcrect.y=0;
            srcrect.w=dstrect.w;
            srcrect.h=dstrect.h;
            SDL_BlitSurface(newt_infrared, &srcrect, newt_win_map_tiles, &dstrect);
        }
    }

    if ((newt_ascii_map[y][x].special&MG_PET) && iflags.hilite_pet) {
		dstrect.x=x*iflags.wc_tile_width;
		dstrect.y=y*iflags.wc_tile_height;
		dstrect.w=1;
		dstrect.h=iflags.wc_tile_height-1;
		SDL_FillRect(newt_win_map_tiles,&dstrect,newt_Pet_colour);
		dstrect.x=x*iflags.wc_tile_width;
		dstrect.y=y*iflags.wc_tile_height;
		dstrect.w=iflags.wc_tile_width-1;
		dstrect.h=1;
		SDL_FillRect(newt_win_map_tiles,&dstrect,newt_Pet_colour);
		dstrect.x=x*iflags.wc_tile_width+iflags.wc_tile_width-1;
		dstrect.y=y*iflags.wc_tile_height;
		dstrect.w=1;
		dstrect.h=iflags.wc_tile_height-1;
		SDL_FillRect(newt_win_map_tiles,&dstrect,newt_Pet_colour);
		dstrect.x=x*iflags.wc_tile_width;
		dstrect.y=y*iflags.wc_tile_height+iflags.wc_tile_height-1;
		dstrect.w=iflags.wc_tile_width;
		dstrect.h=1;
		SDL_FillRect(newt_win_map_tiles,&dstrect,newt_Pet_colour);

    }

};

/* ------------------------------------------------------------------------- */

/*
raw_print(str)	-- Print directly to a screen, or otherwise guarantee that
		   the user sees str.  raw_print() appends a newline to str.
		   It need not recognize ASCII control characters.  This is
		   used during startup (before windowing system initialization
		   -- maybe this means only error startup messages are raw),
		   for error messages, and maybe other "msg" uses.  E.g.
		   updating status for micros (i.e, "saving").
*/
void
newt_raw_print (str)
    const char *str;
{
#ifdef DEBUG
    printf("- newt_raw_print(\"%s\");\n", str);
#endif

/* generally speaking this should be output to the window that was created as
 * NHW_MESSAGE with newt_create_nhwindow(). (imo)
 * It is also used for printing messages pre/post newt windowing
 * system initialisation/closure.
 */

    if (!str||!strlen(str)) return;

    if (iflags.window_inited && WIN_MESSAGE!=WIN_ERR) {
        newt_putstr(WIN_MESSAGE,ATR_NONE,str);
    } else {
        printf("%s\n",str);
    }

    /*TODO*/
}

/* ------------------------------------------------------------------------- */

/*
raw_print_bold(str)
		-- Like raw_print(), but prints in bold/standout (if possible).
*/
void
newt_raw_print_bold (str)
    const char *str;
{
#ifdef DEBUG
    printf("- newt_raw_print_bold(\"%s\");\n", str);
#endif

    /*TODO*/

/* lazy man's (or no bold support) solution :
 *
    newt_raw_print (str);
 *
 */

    if (WIN_MESSAGE!=WIN_ERR) {
        newt_putstr(WIN_MESSAGE,ATR_BOLD,str);
    } else {
        printf("%s\n",str);
    }

}

/* ------------------------------------------------------------------------- */

#ifdef USER_SOUNDS
#define CHUNKAMOUNT 16

Mix_Music *newt_music=NULL;
Uint32 newt_chunknum = 0;
Mix_Chunk *newt_chunk[CHUNKAMOUNT]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

void play_usersound(const char *filename, int volume) {                                                                                    
    Mix_Music *musicNew;
    Mix_Chunk *chunkNew;
    
    if (!UseAudio) return;

    if ((chunkNew=Mix_LoadWAV(filename))) {
        Mix_VolumeChunk(chunkNew,volume);
        Mix_PlayChannel(-1,chunkNew,0);
        if (newt_chunk[newt_chunknum]) Mix_FreeChunk(newt_chunk[newt_chunknum]);
        newt_chunk[newt_chunknum]=chunkNew;
        newt_chunknum=(newt_chunknum+1)&(CHUNKAMOUNT-1);
    } else {
    if ((musicNew=Mix_LoadMUS(filename))) {
        Mix_VolumeMusic(volume);
        Mix_PlayMusic(musicNew,0);
        if (newt_music) Mix_FreeMusic(newt_music);
        newt_music=musicNew;
    } else
        printf("[Warning] could not play file(type) '%s', sorry.\n",filename);
    }
}                                                                                                                                          

#endif

/* ------------------------------------------------------------------------- */
