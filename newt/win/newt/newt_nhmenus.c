#include "hack.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include "newt_nhmenus.h"

#include "newt_nhinput.h"
#include "newt_nhoutput.h"
#include "newt_nhwindows.h"
#include "newt_globals.h"

extern SDL_Surface *newt_screen;

extern TTF_Font *newt_font;
extern Sint32 newt_fontsize;

extern SDL_Color newt_Menu_fg;
extern Uint32 newt_Menu_bg;
extern Uint32 newt_Menu_Border;

/* ------------------------------------------------------------------------- */

/*
start_menu(window)
		-- Start using window as a menu.  You must call start_menu()
		   before add_menu().  After calling start_menu() you may not
		   putstr() to the window.  Only windows of type NHW_MENU may
		   be used for menus.
*/
void
newt_start_menu (window)
    winid window;
{
	newt_window_rec *textwindow;

	#ifdef DEBUG
    printf("- newt_start_menu(%d);\n", window);
#endif

	textwindow=(newt_window_rec *)window;
	textwindow->type=NHW_MENU;
	newt_clear_nhwindow(window);

    /* "window" is the result as you returned it from create_nhwindow */

    /*TODO*/
};

/* ------------------------------------------------------------------------- */

/*
add_menu(windid window, int glyph, const anything identifier,
				char accelerator, char groupacc,
				int attr, char *str, boolean preselected)
		-- Add a text line str to the given menu window.  If identifier
		   is 0, then the line cannot be selected (e.g. a title).
		   Otherwise, identifier is the value returned if the line is
		   selected.  Accelerator is a keyboard key that can be used
		   to select the line.  If the accelerator of a selectable
		   item is 0, the window system is free to select its own
		   accelerator.  It is up to the window-port to make the
		   accelerator visible to the user (e.g. put "a - " in front
		   of str).  The value attr is the same as in putstr().
		   Glyph is an optional glyph to accompany the line.  If
		   window port cannot or does not want to display it, this
		   is OK.  If there is no glyph applicable, then this
		   value will be NO_GLYPH.
		-- All accelerators should be in the range [A-Za-z],
		   but there are a few exceptions such as the tty player
		   selection code which uses '*'.
	        -- It is expected that callers do not mix accelerator
		   choices.  Either all selectable items have an accelerator
		   or let the window system pick them.  Don't do both.
		-- Groupacc is a group accelerator.  It may be any character
		   outside of the standard accelerator (see above) or a
		   number.  If 0, the item is unaffected by any group
		   accelerator.  If this accelerator conflicts with
		   the menu command (or their user defined alises), it loses.
		   The menu commands and aliases take care not to interfere
		   with the default object class symbols.
		-- If you want this choice to be preselected when the
		   menu is displayed, set preselected to TRUE.
*/
void
newt_add_menu ( window, glyph, identifier,
                        ch, gch, attr, str, preselected)
    winid window;
    int glyph;
    const anything* identifier;
	CHAR_P ch;
    CHAR_P gch;
    int attr;
    const char *str;
    BOOLEAN_P preselected;
{
	char *newstr;
	newt_window_rec *textwindow;

#ifdef DEBUG
    printf("- newt_add_menu (%d,%d,%d,'%c','%c',%d,\"%s\",%d);\n",
        window, glyph, identifier, ch, gch, attr, str, preselected);
#endif

	newstr=malloc(TEXTLINE_ACCEL_USE_CHAR+1+1+1+1+strlen(str)+1);

  newstr[TEXTLINE_ACCEL_GROUP]=identifier->a_int ? gch ? gch : ' ' : '\033';
  newstr[TEXTLINE_ACCEL_CHAR]=identifier->a_int ? ch ? ch : ' ' : '\033',

	sprintf(&newstr[TEXTLINE_ACCEL_USE_CHAR],"%c %c %s",
		ch ? ch : ' ',
		identifier->a_int ? preselected ? '+' : '-' : ' ',
		str);
	newt_putstr(window, attr, newstr);
    free(newstr);

	textwindow=(newt_window_rec *)window;
	textwindow->identifier=
        (anything **)realloc( (void *)textwindow->identifier,
            sizeof(anything *)*textwindow->textline_amount);
	textwindow->identifier[textwindow->textline_amount-1]=
        malloc(sizeof(anything));
	*textwindow->identifier[textwindow->textline_amount-1]=*identifier;
	textwindow->glyph=
        (int *)realloc( (void *)textwindow->glyph,
            sizeof(int)*textwindow->textline_amount);
  textwindow->glyph[textwindow->textline_amount-1]=glyph;

    /* "window" is the result as you returned it from create_nhwindow */

    /*TODO*/
};

/* ------------------------------------------------------------------------- */

/*
end_menu(window, prompt)
		-- Stop adding entries to the menu and flushes the window
		   to the screen (brings to front?).  Prompt is a prompt
		   to give the user.  If prompt is NULL, no prompt will
		   be printed.
		** This probably shouldn't flush the window any more (if
		** it ever did).  That should be select_menu's job.  -dean
*/
void
newt_end_menu (window, prompt)
    winid window;
    const char *prompt;
{
	newt_window_rec *textwindow;
  int width, height;

#ifdef DEBUG
  printf("- [done] newt_end_menu (%d, \"%s\");\n", window, prompt);
#endif

	textwindow=(newt_window_rec *)window;
	if (textwindow->title) free(textwindow->title);
	textwindow->title=NULL;
	if (prompt&&strlen(prompt)) {
		textwindow->title=malloc(strlen(prompt)+1);
		strcpy(textwindow->title, prompt);
    TTF_SizeText(newt_font, prompt, &width, &height);
    if (width>textwindow->textline_width) textwindow->textline_width=width;
	}

};

/* ------------------------------------------------------------------------- */

/*
int select_menu(windid window, int how, menu_item **selected)
		-- Return the number of items selected; 0 if none were chosen,
		   -1 when explicitly cancelled.  If items were selected, then
		   selected is filled in with an allocated array of menu_item
		   structures, one for each selected line.  The caller must
		   free this array when done with it.  The "count" field
		   of selected is a user supplied count.  If the user did
		   not supply a count, then the count field is filled with
		   -1 (meaning all).  A count of zero is equivalent to not
		   being selected and should not be in the list.  If no items
		   were selected, then selected is NULL'ed out.  How is the
		   mode of the menu.  Three valid values are PICK_NONE,
		   PICK_ONE, and PICK_ANY, meaning: nothing is selectable,
		   only one thing is selectable, and any number valid items
		   may selected.  If how is PICK_NONE, this function should
		   never return anything but 0 or -1.
		-- You may call select_menu() on a window multiple times --
		   the menu is saved until start_menu() or destroy_nhwindow()
		   is called on the window.
		-- Note that NHW_MENU windows need not have select_menu()
		   called for them. There is no way of knowing whether
		   select_menu() will be called for the window at
		   create_nhwindow() time.
*/
int
newt_select_menu (window, how, menu_list)
    winid window;
    int how;
    menu_item ** menu_list;
{
    char input;
    SDL_Rect srcrect,dstrect;
    SDL_Surface *textsurface;
	newt_window_rec *textwindow=NULL;
	int pages, linesperpage, pagecounter;
	char *pagenum;
	int counter, fwd_counter;
	char *textline;
	int amount_selected;
	char ch;
	int found;
    menu_item *menu_list_use;
  int howmany=0;
  int hasglyphs;
  SDL_Surface *glyph_surface_tmp;
  SDL_Surface *glyph_surface;

#ifdef DEBUG
    printf("- newt_select_menu (%d, %d, (todo)#menu_list#);\n",
        window, how);
#endif

  if (WIN_MESSAGE!=WIN_ERR) newt_clear_nhwindow(WIN_MESSAGE);

	menu_list_use=*menu_list;
	menu_list_use=NULL;
    /* "window" is the result as you returned it from create_nhwindow */
    /*TODO*/
    switch (how) {
        case PICK_NONE:  /* simply wait for "space"/"enter"/"foo" and return */
			newt_display_nhwindow( window, FALSE);
			return -1;
        case PICK_ONE:   /* return single selected item */
			textwindow=(newt_window_rec *)window;

			linesperpage=(newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 3 : 4)*newt_fontsize))/newt_fontsize;
            if (textwindow->title) linesperpage--;
			linesperpage--;
			if (linesperpage<1) linesperpage=1;

			pages=(textwindow->textline_amount+(linesperpage-1))/linesperpage;

			for (pagecounter=0; pagecounter<pages; pagecounter++) {

				/* assign missing accelerators */
				ch='a';
        hasglyphs=FALSE;
				for (counter=0; counter<linesperpage; counter++) {
					if (pagecounter*linesperpage+counter>=textwindow->textline_amount) break;
          hasglyphs=hasglyphs||(textwindow->glyph && textwindow->glyph[pagecounter*linesperpage+counter]!=NO_GLYPH);
					if (textwindow->textlines[pagecounter*linesperpage+counter][TEXTLINE_ACCEL_CHAR]==' ') {
						for (found=FALSE; found; ch=='z' ? ch='A': ch++) {
							found=FALSE;
							for (fwd_counter=counter+1; fwd_counter<linesperpage; fwd_counter++) {
								if (pagecounter*linesperpage+fwd_counter>=textwindow->textline_amount) break;
								if (ch==textwindow->textlines[pagecounter*linesperpage+fwd_counter][TEXTLINE_ACCEL_CHAR]) {
									found=TRUE;
									break;
								}
							}
						}
						textwindow->textlines[pagecounter*linesperpage+counter][TEXTLINE_ACCEL_USE_CHAR]=ch++;
					};
				}

        if (hasglyphs && (newt_win_map!=newt_win_map_ascii)) {
          hasglyphs=newt_fontsize+4;
        } else {
          hasglyphs=0;
        }

        newt_margin_right=textwindow->textline_width+6+4+hasglyphs;
        newt_display_nhwindow(WIN_MAP,FALSE);

				/* background */
				dstrect.x=newt_screen->w-(textwindow->textline_width+6+4+hasglyphs);
				dstrect.y=newt_fontsize;
				dstrect.w=textwindow->textline_width+6+4+hasglyphs;
				dstrect.h=newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 3 : 4)*newt_fontsize);
				SDL_FillRect(newt_screen, &dstrect, newt_Menu_bg);

				/* seperator */
				dstrect.x=newt_screen->w-(textwindow->textline_width+5+4+hasglyphs);
				dstrect.y=newt_fontsize;
				dstrect.w=1;
				dstrect.h=newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 3 : 4)*newt_fontsize);
				SDL_FillRect(newt_screen,&dstrect,newt_Menu_Border);

        if (textwindow->title) {
					textsurface = TTF_RenderText_Blended(
						newt_font, textwindow->title, newt_Menu_fg);
					srcrect.x=srcrect.y=0;
					srcrect.w=textsurface->w;
					srcrect.h=textsurface->h;
					dstrect.x=newt_screen->w-(textwindow->textline_width+4);
					dstrect.y=newt_fontsize;
					dstrect.w=textwindow->textline_width;
					dstrect.h=newt_fontsize;

					SDL_BlitSurface(textsurface, &srcrect, newt_screen, &dstrect);
					SDL_FreeSurface(textsurface);

                    dstrect.x=newt_screen->w-(textwindow->textline_width+5+4+hasglyphs);
                    dstrect.y=(newt_fontsize*2);
                    dstrect.w=textwindow->textline_width+4+5;
                    dstrect.h=1;
                    SDL_FillRect(newt_screen,&dstrect,newt_Menu_Border);
        }

          glyph_surface_tmp=SDL_CreateRGBSurface(
            SDL_HWSURFACE | SDL_SRCALPHA,
            iflags.wc_tile_width,
            iflags.wc_tile_height,
            32,
            newt_tiles->format->Rmask,
            newt_tiles->format->Gmask,
            newt_tiles->format->Bmask,
            newt_tiles->format->Amask);

          dstrect.w=dstrect.h=newt_fontsize;
          glyph_surface=SDL_CreateRGBSurface(
            SDL_HWSURFACE | SDL_SRCALPHA,
            dstrect.w,
            dstrect.h,
            32,
            newt_tiles->format->Rmask,
            newt_tiles->format->Gmask,
            newt_tiles->format->Bmask,
            newt_tiles->format->Amask);

				for (counter=0; counter<linesperpage; counter++) {
					if (pagecounter*linesperpage+counter>=textwindow->textline_amount) break;

          if (textwindow->identifier[pagecounter*linesperpage+counter]->a_int) {
            textline=&textwindow->textlines[pagecounter*linesperpage+counter][TEXTLINE_ACCEL_USE_CHAR];
          } else {
            textline=&textwindow->textlines[pagecounter*linesperpage+counter][TEXTLINE_TEXT];
          }
					if (!strlen(textline)) continue;

          if (hasglyphs && textwindow->glyph && textwindow->glyph[pagecounter*linesperpage+counter]!=NO_GLYPH) {
            dstrect.w=srcrect.w=iflags.wc_tile_width;
            dstrect.h=srcrect.h=iflags.wc_tile_height;
            srcrect.x=(glyph2tile[textwindow->glyph[pagecounter*linesperpage+counter]]%(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_width;
            srcrect.y=(glyph2tile[textwindow->glyph[pagecounter*linesperpage+counter]]/(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_height;
            dstrect.x=dstrect.y=0;
            SDL_FillRect(glyph_surface_tmp,(SDL_Rect *)&dstrect,newt_Menu_bg);
            SDL_BlitSurface(newt_tiles, (SDL_Rect *)&srcrect, glyph_surface_tmp, (SDL_Rect *)&dstrect);
            srcrect.w=iflags.wc_tile_width;
            srcrect.h=iflags.wc_tile_height;
            srcrect.x=0;
            srcrect.y=0;
            newt_stretch_fast(glyph_surface_tmp, (SDL_Rect *)&srcrect, glyph_surface);
            srcrect.x=srcrect.y=0;
            srcrect.w=dstrect.w=glyph_surface->w;
            srcrect.h=dstrect.h=glyph_surface->h;
            dstrect.x=newt_screen->w-(textwindow->textline_width+4+hasglyphs);
            dstrect.y=(counter+1+(textwindow->title ? 1 : 0))*newt_fontsize;
            SDL_BlitSurface(glyph_surface, &srcrect, newt_screen, &dstrect);
          }

					textsurface = TTF_RenderText_Blended(
						newt_font, textline, newt_Menu_fg);
					srcrect.x=srcrect.y=0;
					srcrect.w=textsurface->w;
					srcrect.h=textsurface->h;
					dstrect.x=newt_screen->w-(textwindow->textline_width+4);
					dstrect.y=(counter+1+(textwindow->title ? 1 : 0))*newt_fontsize;
					dstrect.w=textwindow->textline_width;
					dstrect.h=newt_fontsize;

					SDL_BlitSurface(textsurface, &srcrect, newt_screen, &dstrect);
					SDL_FreeSurface(textsurface);

				}

        SDL_FreeSurface(glyph_surface);
        SDL_FreeSurface(glyph_surface_tmp);

				pagenum=malloc(BUFSZ);
				sprintf(pagenum, "(Page %d of %d)", pagecounter+1, pages);
					textsurface = TTF_RenderText_Blended(
						newt_font, pagenum, newt_Menu_fg);
					srcrect.x=srcrect.y=0;
					srcrect.w=textsurface->w;
					srcrect.h=textsurface->h;
					dstrect.x=newt_screen->w-(textsurface->w+4);
					dstrect.y=newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 3 : 4)*newt_fontsize);
					dstrect.w=textsurface->w;
					dstrect.h=newt_fontsize;

					SDL_BlitSurface(textsurface, &srcrect, newt_screen, &dstrect);
					SDL_FreeSurface(textsurface);
				free(pagenum);

				input=newt_nhgetch();
  newt_margin_right=0;
  newt_display_nhwindow(WIN_MAP,FALSE);
  if (flags.perm_invent&&WIN_INVEN!=WIN_ERR) newt_display_nhwindow(WIN_INVEN,FALSE);

				switch (input) {
					case '\033':
						return -1;
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
					case ' ':
                        break;
					case MENU_NEXT_PAGE: /* page down */
						if (pagecounter>pages-2) pagecounter=pages-2;
						break;
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            howmany*=10;
            howmany+=(input-'0');
						pagecounter--;
            break;
					default:
						for (counter=0; counter<linesperpage; counter++) {
							if (pagecounter*linesperpage+counter>=textwindow->textline_amount) break;

							textline=textwindow->textlines[pagecounter*linesperpage+counter];
							if (!strlen(textline)) continue;

							if (input==textline[TEXTLINE_ACCEL_GROUP]||input==textline[TEXTLINE_ACCEL_USE_CHAR]) {
								textline[TEXTLINE_HYPHEN]=(textline[TEXTLINE_HYPHEN]=='-')?(howmany?'#':'+'):'-';
                textline[TEXTLINE_AMOUNT]=howmany?howmany:-1;
								pagecounter=pages;
								break;
							}
						}
						pagecounter--;
						break;
				}
			}

			break;
        case PICK_ANY:   /* allow for multiple/mass selections and return results */
			textwindow=(newt_window_rec *)window;

			linesperpage=(newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 3 : 4)*newt_fontsize))/newt_fontsize;
            if (textwindow->title) linesperpage--;
			linesperpage--;
			if (linesperpage<1) linesperpage=1;

			pages=(textwindow->textline_amount+(linesperpage-1))/linesperpage;

			for (pagecounter=0; pagecounter<pages; pagecounter++) {

				/* assign missing accelerators */
				ch='a';
        hasglyphs=FALSE;
				for (counter=0; counter<linesperpage; counter++) {
					if (pagecounter*linesperpage+counter>=textwindow->textline_amount) break;
          hasglyphs=hasglyphs||(textwindow->glyph && textwindow->glyph[pagecounter*linesperpage+counter]!=NO_GLYPH);
					if (textwindow->textlines[pagecounter*linesperpage+counter][TEXTLINE_ACCEL_CHAR]==' ') {
						for (found=FALSE; found;) {
							found=FALSE;
							for (fwd_counter=counter+1; fwd_counter<linesperpage; fwd_counter++) {
								if (pagecounter*linesperpage+fwd_counter>=textwindow->textline_amount) break;
								if (ch==textwindow->textlines[pagecounter*linesperpage+fwd_counter][TEXTLINE_ACCEL_CHAR]) {
									found=TRUE;
									break;
								}
							}
						}
						textwindow->textlines[pagecounter*linesperpage+counter][TEXTLINE_ACCEL_USE_CHAR]=ch++;
						if (ch==('z'+1)) ch='A';
					};
				}

        if (hasglyphs && (newt_win_map!=newt_win_map_ascii)) {
          hasglyphs=newt_fontsize+4;
        } else {
          hasglyphs=0;
        }

        newt_margin_right=textwindow->textline_width+6+4+hasglyphs;
        printf("%d ---> %d\n",WIN_MAP,newt_margin_right);
        newt_display_nhwindow(WIN_MAP,FALSE);

				/* background */
				dstrect.x=newt_screen->w-(textwindow->textline_width+6+4+hasglyphs);
				dstrect.y=newt_fontsize;
				dstrect.w=textwindow->textline_width+6+4+hasglyphs;
				dstrect.h=newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 3 : 4)*newt_fontsize);
				SDL_FillRect(newt_screen, &dstrect, newt_Menu_bg);

				/* seperator */
				dstrect.x=newt_screen->w-(textwindow->textline_width+5+4+hasglyphs);
				dstrect.y=newt_fontsize;
				dstrect.w=1;
				dstrect.h=newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 3 : 4)*newt_fontsize);
				SDL_FillRect(newt_screen,&dstrect,newt_Menu_Border);

                if (textwindow->title) {
					textsurface = TTF_RenderText_Blended(
						newt_font, textwindow->title, newt_Menu_fg);
					srcrect.x=srcrect.y=0;
					srcrect.w=textsurface->w;
					srcrect.h=textsurface->h;
					dstrect.x=newt_screen->w-(textwindow->textline_width+4);
					dstrect.y=newt_fontsize;
					dstrect.w=textwindow->textline_width;
					dstrect.h=newt_fontsize;

					SDL_BlitSurface(textsurface, &srcrect, newt_screen, &dstrect);
					SDL_FreeSurface(textsurface);

                    dstrect.x=newt_screen->w-(textwindow->textline_width+5+4+hasglyphs);
                    dstrect.y=(newt_fontsize*2);
                    dstrect.w=textwindow->textline_width+4+5+hasglyphs;
                    dstrect.h=1;
                    SDL_FillRect(newt_screen,&dstrect,newt_Menu_Border);
                }

          glyph_surface_tmp=SDL_CreateRGBSurface(
            SDL_HWSURFACE | SDL_SRCALPHA,
            iflags.wc_tile_width,
            iflags.wc_tile_height,
            32,
            newt_tiles->format->Rmask,
            newt_tiles->format->Gmask,
            newt_tiles->format->Bmask,
            newt_tiles->format->Amask);

          dstrect.w=dstrect.h=newt_fontsize;
          glyph_surface=SDL_CreateRGBSurface(
            SDL_HWSURFACE | SDL_SRCALPHA,
            dstrect.w,
            dstrect.h,
            32,
            newt_tiles->format->Rmask,
            newt_tiles->format->Gmask,
            newt_tiles->format->Bmask,
            newt_tiles->format->Amask);

				for (counter=0; counter<linesperpage; counter++) {
					if (pagecounter*linesperpage+counter>=textwindow->textline_amount) break;

          if (textwindow->identifier[pagecounter*linesperpage+counter]->a_int) {
            textline=&textwindow->textlines[pagecounter*linesperpage+counter][TEXTLINE_ACCEL_USE_CHAR];
          } else {
            textline=&textwindow->textlines[pagecounter*linesperpage+counter][TEXTLINE_TEXT];
          }
					if (!strlen(textline)) continue;

          if (hasglyphs && textwindow->glyph && textwindow->glyph[pagecounter*linesperpage+counter]!=NO_GLYPH) {
            dstrect.w=srcrect.w=iflags.wc_tile_width;
            dstrect.h=srcrect.h=iflags.wc_tile_height;
            srcrect.x=(glyph2tile[textwindow->glyph[pagecounter*linesperpage+counter]]%(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_width;
            srcrect.y=(glyph2tile[textwindow->glyph[pagecounter*linesperpage+counter]]/(newt_tiles->w/iflags.wc_tile_width))*iflags.wc_tile_height;
            dstrect.x=dstrect.y=0;
            SDL_FillRect(glyph_surface_tmp,(SDL_Rect *)&dstrect,newt_Menu_bg);
            SDL_BlitSurface(newt_tiles, (SDL_Rect *)&srcrect, glyph_surface_tmp, (SDL_Rect *)&dstrect);
            srcrect.w=iflags.wc_tile_width;
            srcrect.h=iflags.wc_tile_height;
            srcrect.x=0;
            srcrect.y=0;
            newt_stretch_fast(glyph_surface_tmp, (SDL_Rect *)&srcrect, glyph_surface);
            srcrect.x=srcrect.y=0;
            srcrect.w=dstrect.w=glyph_surface->w;
            srcrect.h=dstrect.h=glyph_surface->h;
            dstrect.x=newt_screen->w-(textwindow->textline_width+4+hasglyphs);
            dstrect.y=(counter+1+(textwindow->title ? 1 : 0))*newt_fontsize;
            SDL_BlitSurface(glyph_surface, &srcrect, newt_screen, &dstrect);
          }

					textsurface = TTF_RenderText_Blended(
						newt_font, textline, newt_Menu_fg);
					srcrect.x=srcrect.y=0;
					srcrect.w=textsurface->w;
					srcrect.h=textsurface->h;
					dstrect.x=newt_screen->w-(textwindow->textline_width+4);
					dstrect.y=(counter+1+(textwindow->title ? 1 : 0))*newt_fontsize;
					dstrect.w=textwindow->textline_width;
					dstrect.h=newt_fontsize;

					SDL_BlitSurface(textsurface, &srcrect, newt_screen, &dstrect);
					SDL_FreeSurface(textsurface);

				}

        SDL_FreeSurface(glyph_surface);
        SDL_FreeSurface(glyph_surface_tmp);

				pagenum=malloc(BUFSZ);
				sprintf(pagenum, "(Page %d of %d)", pagecounter+1, pages);
					textsurface = TTF_RenderText_Blended(
						newt_font, pagenum, newt_Menu_fg);
					srcrect.x=srcrect.y=0;
					srcrect.w=textsurface->w;
					srcrect.h=textsurface->h;
					dstrect.x=newt_screen->w-(textsurface->w+4);
					dstrect.y=newt_screen->h-((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 3 : 4)*newt_fontsize);
					dstrect.w=textsurface->w;
					dstrect.h=newt_fontsize;

					SDL_BlitSurface(textsurface, &srcrect, newt_screen, &dstrect);
					SDL_FreeSurface(textsurface);
				free(pagenum);

				input=newt_nhgetch();
  newt_margin_right=0;
  newt_display_nhwindow(WIN_MAP,FALSE);
  if (flags.perm_invent&&WIN_INVEN!=WIN_ERR) newt_display_nhwindow(WIN_INVEN,FALSE);

                switch(input) {
					case '\033':
						return -1;
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
					case ' ':
                        break;
					case MENU_NEXT_PAGE: /* page down */
						if (pagecounter>pages-2) pagecounter=pages-2;
						break;
          case MENU_SELECT_PAGE: /* select the page in view */
						for (counter=0; counter<linesperpage; counter++) {
							if (pagecounter*linesperpage+counter>=textwindow->textline_amount) break;

							textline=&textwindow->textlines[pagecounter*linesperpage+counter][TEXTLINE_ACCEL_USE_CHAR];
							if (!strlen(textline)) continue;
							if (textline[TEXTLINE_HYPHEN]=='-') {
                textline[TEXTLINE_HYPHEN]='+';
                textline[TEXTLINE_AMOUNT]=howmany?howmany:-1;
              }
						}
						pagecounter--;
            break;
          case MENU_UNSELECT_PAGE: /* deselect the page in view */
						for (counter=0; counter<linesperpage; counter++) {
							if (pagecounter*linesperpage+counter>=textwindow->textline_amount) break;

							textline=&textwindow->textlines[pagecounter*linesperpage+counter][TEXTLINE_ACCEL_USE_CHAR];
							if (!strlen(textline)) continue;
							if (textline[TEXTLINE_HYPHEN]=='+') textline[TEXTLINE_HYPHEN]='-';
						}
						pagecounter--;
            break;
          case MENU_INVERT_PAGE: /* invert selections in page */
						for (counter=0; counter<linesperpage; counter++) {
							if (pagecounter*linesperpage+counter>=textwindow->textline_amount) break;
							textline=textwindow->textlines[pagecounter*linesperpage+counter];
							if (!strlen(textline)) continue;
							if (textline[TEXTLINE_HYPHEN]=='-'||textline[TEXTLINE_HYPHEN]=='+') {
                textline[TEXTLINE_HYPHEN]=textline[TEXTLINE_HYPHEN]=='+'?'-':'+';
                textline[TEXTLINE_AMOUNT]=howmany?howmany:-1;
              }
						}
						pagecounter--;
            break;
          case MENU_SELECT_ALL: /* select everything */
						for (counter=0; counter<textwindow->textline_amount; counter++) {
							textline=textwindow->textlines[counter];
							if (!strlen(textline)) continue;
							if (textline[TEXTLINE_HYPHEN]=='-') textline[TEXTLINE_HYPHEN]='+';
              textline[TEXTLINE_AMOUNT]=howmany?howmany:-1;
						}
						pagecounter--;
            break;
          case MENU_UNSELECT_ALL: /* deselect everything */
						for (counter=0; counter<textwindow->textline_amount; counter++) {
							textline=textwindow->textlines[counter];
							if (!strlen(textline)) continue;
							if (textline[TEXTLINE_HYPHEN]=='+') textline[TEXTLINE_HYPHEN]='-';
						}
						pagecounter--;
            break;
          case MENU_INVERT_ALL: /* invert all selections */
						for (counter=0; counter<textwindow->textline_amount; counter++) {
							textline=textwindow->textlines[counter];
							if (!strlen(textline)) continue;
							if (textline[TEXTLINE_HYPHEN]=='-'||textline[TEXTLINE_HYPHEN]=='+') {
                textline[TEXTLINE_HYPHEN]=textline[TEXTLINE_HYPHEN]=='+'?'-':'+';
                textline[TEXTLINE_AMOUNT]=howmany?howmany:-1;
              }
						}
						pagecounter--;
            break;
          case MENU_SEARCH: /* search and toggle those found */
						pagecounter--;
            break;
          case '\n':
						pagecounter=pages;
						break;
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            howmany*=10;
            howmany+=(input-'0');
						pagecounter--;
            break;
					default:
						for (counter=0; counter<linesperpage; counter++) {
							if (pagecounter*linesperpage+counter>=textwindow->textline_amount) break;

							textline=textwindow->textlines[pagecounter*linesperpage+counter];
							if (!strlen(textline)) continue;

							if (input==textline[TEXTLINE_ACCEL_GROUP]||input==textline[TEXTLINE_ACCEL_USE_CHAR]) {
								textline[TEXTLINE_HYPHEN]=(textline[TEXTLINE_HYPHEN]=='-')?(howmany?'#':'+'):'-';
                textline[TEXTLINE_AMOUNT]=howmany?howmany:-1;
								if (input==textline[TEXTLINE_ACCEL_USE_CHAR]) break;
							}
						}
            howmany=0;
						pagecounter--;
						break;
				}
			}

			break;
    }

	amount_selected=0;
	for (counter=0; counter<textwindow->textline_amount; counter++) {
		textline=textwindow->textlines[counter];
		if (!strlen(textline)) continue;

		if (textline[TEXTLINE_HYPHEN]=='+'||textline[TEXTLINE_HYPHEN]=='#') {
			amount_selected++;
			menu_list_use=(menu_item *)realloc(menu_list_use, sizeof(menu_item)*amount_selected);
			menu_list_use[amount_selected-1].item=*textwindow->identifier[counter];
			menu_list_use[amount_selected-1].count=textline[TEXTLINE_AMOUNT];
			textline[TEXTLINE_HYPHEN]='-';
		}
	}
	*menu_list=menu_list_use;
    return amount_selected;
};

/* ------------------------------------------------------------------------- */

/*
char message_menu(char let, int how, const char *mesg)
		-- tty-specific hack to allow single line context-sensitive
		   help to behave compatibly with multi-line help menus.
		-- This should only be called when a prompt is active; it
		   sends `mesg' to the message window.  For tty, it forces
		   a --More-- prompt and enables `let' as a viable keystroke
		   for dismissing that prompt, so that the original prompt
		   can be answered from the message line "help menu".
		-- Return value is either `let', '\0' (no selection was made),
		   or '\033' (explicit cancellation was requested).
		-- Interfaces which issue prompts and messages to separate
		   windows typically won't need this functionality, so can
		   substitute genl_message_menu (windows.c) instead.
*/
char
newt_message_menu (let, how, mesg)
    CHAR_P let;
    int how;
    const char *mesg;
{
#ifdef DEBUG
    printf("- newt_message_menu ('%c', %d, \"%s\");\n",
        let, how, mesg);
#endif

    return genl_message_menu(let, how, mesg);
};

/* ------------------------------------------------------------------------- */

