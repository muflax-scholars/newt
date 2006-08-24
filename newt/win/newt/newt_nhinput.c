#include "hack.h"
#include "func_tab.h"

#include "newt_globals.h"
#include "newt_nhinput.h"
#include "newt_nhoutput.h"
#include "newt_nhwindows.h"
#include "newt_main.h"

#include <SDL.h>

/* ------------------------------------------------------------------------- */

/*
int nhgetch()
        -- Returns a single character input from the user.
        -- In the tty window-port, nhgetch() assumes that tgetch()
           will be the routine the OS provides to read a character.
           Returned character _must_ be non-zero and it must be
                   non meta-zero too (zero with the meta-bit set).
*/
int
newt_nhgetch (void)
{
    int retval;
    int x,y,mod;
#ifdef DEBUG
    printf("- newt_nhgetch();\n");
#endif

    while (!(retval=newt_nh_poskey(&x,&y,&mod)));
    return retval;
};

/* ------------------------------------------------------------------------- */

int noegnud_internal_convert_sdlkey_nhkey_fastwalk=0;
/* routine blatantly stolen from "noeGNUd",
 * I hope this doesn't anger the author :(
 */
static int
newt_convertkey_sdl2nh(sym, unicode, mod, manualfix)
    int sym;
    int unicode;
    int mod;
    int manualfix;
{
    int shift = mod & KMOD_SHIFT;
    int ctrl = mod & KMOD_CTRL;
    int alt = mod & KMOD_ALT;

    int ch;

    SDL_Event *pushedevent;

    ch=unicode;

    if (ch=='!') return 0; // damn shell thing :/

    if ((ctrl&&ch=='z')||(ch==('z'-('a'-1)))) return 0; // lets just ignore this nasty lil bugger...

    if (ctrl&&ch>='a'&&ch<'z') return ch-('a'-1);

    if (alt&&ch>='a'&&ch<='z') {
        /* *shudder* */
        pushedevent=malloc(sizeof(SDL_Event));
        ((SDL_KeyboardEvent *)pushedevent)->type=SDL_KEYDOWN;
        ((SDL_KeyboardEvent *)pushedevent)->which=0;
        ((SDL_KeyboardEvent *)pushedevent)->state=SDL_PRESSED;
        ((SDL_KeyboardEvent *)pushedevent)->keysym.sym=ch;
        ((SDL_KeyboardEvent *)pushedevent)->keysym.mod=0;
        ((SDL_KeyboardEvent *)pushedevent)->keysym.scancode=0;
        ((SDL_KeyboardEvent *)pushedevent)->keysym.unicode=ch;
        SDL_PushEvent(pushedevent);
        free(pushedevent);
        return '#';
    }

    if (ch>='a'&&ch<='z') {
        if (shift) ch+='A'-'a';
        return ch;
    }

    switch (sym) {
    case SDLK_BACKSPACE: return '\b';
    case SDLK_KP_ENTER:
    case SDLK_RETURN: return '\n';
    case SDLK_ESCAPE: return '\033';
    case SDLK_TAB: return '\t';
    case SDLK_KP8:
    case SDLK_UP: return iflags.num_pad ? '8' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'K' : 'k';
    case SDLK_KP2:
    case SDLK_DOWN: return iflags.num_pad ? '2' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'J' : 'j';
    case SDLK_KP4:
    case SDLK_LEFT: return iflags.num_pad ? '4' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'H' : 'h';
    case SDLK_KP6:
    case SDLK_RIGHT: return iflags.num_pad ? '6' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'L' : 'l';
    case SDLK_KP7: return iflags.num_pad ? '7' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'Y' : 'y';
    case SDLK_KP9: return iflags.num_pad ? '9' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'U' : 'u';
    case SDLK_KP1: return iflags.num_pad ? '1' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'B' : 'b';
    case SDLK_KP3: return iflags.num_pad ? '3' : (shift||noegnud_internal_convert_sdlkey_nhkey_fastwalk) ? 'N' : 'n';

    case SDLK_PAGEUP: return MENU_PREVIOUS_PAGE;
    case SDLK_PAGEDOWN: return MENU_NEXT_PAGE;
    case SDLK_HOME: return MENU_FIRST_PAGE;
    case SDLK_END: return MENU_LAST_PAGE;
    }

    /* this should only kick in if unicode didn't find these */

    if (manualfix&&shift) {
        switch(sym) {
            case '0': return ')';
            case '1': return '!';
            case '2': return '@';
            case '3': return '#';
            case '4': return '$';
            case '5': return '%';
            case '6': return '^';
            case '7': return '&';
            case '8': return '*';
            case '9': return '(';
            case '-': return '_';
            case '=': return '+';
            case '[': return '{';
            case ']': return '}';
            case '\\': return '|';
            case ';': return ':';
            case '\'': return '"';
            case ',': return '<';
            case '.': return '>';
            case '/': return '?';
        }
    }


    if ((ch>0)&&(ch<0x7e)) return ch;

    return 0;

}


/* ------------------------------------------------------------------------- */

/*
int nh_poskey(int *x, int *y, int *mod)
        -- Returns a single character input from the user or a
           a positioning event (perhaps from a mouse).  If the
           return value is non-zero, a character was typed, else,
           a position in the MAP window is returned in x, y and mod.
           mod may be one of

            CLICK_1     - mouse click type 1
            CLICK_2     - mouse click type 2

           The different click types can map to whatever the
           hardware supports.  If no mouse is supported, this
           routine always returns a non-zero character.
*/
int
newt_nh_poskey (x, y, mod)
    int *x;
    int *y;
    int *mod;
{
    SDL_Event event;
    int retval;
    int newMapX=1;
    int newMapY=1;

#ifdef DEBUG
    printf("- newt_nh_poskey();\n");
#endif

    newt_current_msghistory=0;

  newt_wait_synch();
  while (1) {
    if (!SDL_PollEvent (NULL)) SDL_Delay(1);  /* give the system some time */
      while (SDL_PollEvent (&event)) {
      switch(event.type) {
        case SDL_KEYDOWN:
          if ((event.key.keysym.mod & KMOD_ALT) && (event.key.keysym.sym==SDLK_RETURN)) {
            newt_deltazoom=TRUE;
            SDL_WM_ToggleFullScreen(newt_screen);
            iflags.wc2_fullscreen=!iflags.wc2_fullscreen;
            newt_windowQueueAdd(WIN_MAP);
           } else
          switch (event.key.keysym.sym) {
            case SDLK_F3:
              newt_screen=SDL_SetVideoMode(
                newt_win_map->w+newt_margin_left,
                newt_win_map->h +
                ((newt_positionbarmode==NEWT_POSITIONBARMODE_NONE ? 3 : 4)*newt_fontsize),
                32,
                SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE | (iflags.wc2_fullscreen ? SDL_FULLSCREEN : 0));
              newt_deltazoom=TRUE;
              newt_clear_nhwindow(WIN_MESSAGE);
              newt_windowQueueAdd(WIN_STATUS);
              newt_windowQueueAdd(WIN_MAP);
              if (flags.perm_invent&&WIN_INVEN!=WIN_ERR) newt_windowQueueAdd(WIN_INVEN);
              break;
            case SDLK_F4:
              newt_ZoomMode++;
              if (newt_ZoomMode>NEWT_ZOOMMODE_END) newt_ZoomMode=NEWT_ZOOMMODE_START;
              newt_deltazoom=TRUE;
              newt_windowQueueAdd(WIN_MAP);
              break;
            case SDLK_F5:
              newt_win_map = (newt_win_map==newt_win_map_tiles) ? newt_win_map_ascii : newt_win_map_tiles;
              newt_deltazoom=TRUE;
              newt_windowQueueAdd(WIN_MAP);
              if (flags.perm_invent&&WIN_INVEN!=WIN_ERR) newt_windowQueueAdd(WIN_INVEN);
              break;
            case SDLK_F6:
              if ((++newt_positionbarmode)>NEWT_POSITIONBARMODE_END) {
                newt_positionbarmode=NEWT_POSITIONBARMODE_START;
              }
              newt_deltazoom=TRUE;
              newt_windowQueueAdd(WIN_MAP);
              if (flags.perm_invent&&WIN_INVEN!=WIN_ERR) newt_windowQueueAdd(WIN_INVEN);
              break;
            case SDLK_F9:
              newt_Zoom_x-=64;
              newt_Zoom_y-=64;
              if (newt_Zoom_x<256) newt_Zoom_x=256;
              if (newt_Zoom_y<256) newt_Zoom_y=256;
              newt_ZoomMode=NEWT_ZOOMMODE_CUSTOM;
              newt_deltazoom=TRUE;
              newt_windowQueueAdd(WIN_MAP);
              break;
            case SDLK_F10:
              newt_Zoom_x+=64;
              newt_Zoom_y+=64;
              newt_ZoomMode=NEWT_ZOOMMODE_CUSTOM;
              newt_deltazoom=TRUE;
              newt_windowQueueAdd(WIN_MAP);
              break;
            default:
              retval=newt_convertkey_sdl2nh(
                event.key.keysym.sym,
                event.key.keysym.unicode,
                event.key.keysym.mod,
                FALSE);
              if (retval) return retval;
              break;
          }
          break;
        case SDL_VIDEORESIZE:
          newt_screen=SDL_SetVideoMode(event.resize.w,event.resize.h,32,SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE | (iflags.wc2_fullscreen ? SDL_FULLSCREEN : 0));
          newt_deltazoom=TRUE;
          newt_clear_nhwindow(WIN_MESSAGE);
          newt_windowQueueAdd(WIN_STATUS);
          newt_windowQueueAdd(WIN_MAP);
          if (flags.perm_invent&&WIN_INVEN!=WIN_ERR) newt_windowQueueAdd(WIN_INVEN);
          break;
        case SDL_MOUSEMOTION:
            if ( newt_zoomed_map &&
                 event.motion.x >= newt_screen_mapRect.x &&
                 event.motion.y >= newt_screen_mapRect.y &&
                 event.motion.x < newt_screen_mapRect.x+newt_screen_mapRect.w &&
                 event.motion.y < newt_screen_mapRect.y+newt_screen_mapRect.h) {

                newMapX=(
                    ( ( ( event.motion.x-newt_screen_mapRect.x ) *
                        newt_map_visibleRect.w ) /
                      newt_screen_mapRect.w + newt_map_visibleRect.x ) * COLNO /
                    newt_win_map->w );

                newMapY=(
                    ( ( ( event.motion.y-newt_screen_mapRect.y ) *
                        newt_map_visibleRect.h ) /
                      newt_screen_mapRect.h + newt_map_visibleRect.y ) * ROWNO /
                    newt_win_map->h );

            }
            if (newMapX!=newt_map_curs_x||newMapY!=newt_map_curs_y) {
                newt_windowQueueAdd(WIN_MAP);
                newt_map_curs_x=newMapX;
                newt_map_curs_y=newMapY;
            }
          /* relative motion, leave the code in place for now, perhaps optionalise it later :D
          newt_map_curs_x+=(event.motion.xrel!=0) ? (event.motion.xrel/abs(event.motion.xrel)) : 0;
          newt_map_curs_y+=(event.motion.yrel!=0) ? (event.motion.yrel/abs(event.motion.yrel)) : 0;
          if (newt_map_curs_x<0) newt_map_curs_x=0;
          if (newt_map_curs_y<0) newt_map_curs_y=0;
          if (newt_map_curs_x>=COLNO) newt_map_curs_x=COLNO-1;
          if (newt_map_curs_y>=ROWNO) newt_map_curs_y=ROWNO-1;
          */
          break;
        case SDL_JOYAXISMOTION:
          printf("JOYSTICK MOTION: axis(%d) value(%d)\n", event.jaxis.axis, event.jaxis.value);
          if (event.jaxis.axis==0) {
            newt_map_curs_x+=(event.jaxis.value!=0) ? (event.jaxis.value/abs(event.jaxis.value)) : 0;
          }
          if (event.jaxis.axis==1) {
            newt_map_curs_y+=(event.jaxis.value!=0) ? (event.jaxis.value/abs(event.jaxis.value)) : 0;
          }  
            newt_windowQueueAdd(WIN_MAP);
        case SDL_MOUSEBUTTONDOWN:
          *x = newt_map_curs_x;
          *y = newt_map_curs_y;
          *mod = CLICK_1;
          return 0;  
          break;
        case SDL_JOYBUTTONDOWN:
          *x = newt_map_curs_x;
          *y = newt_map_curs_y;
          *mod = CLICK_1;
          return 0;  
          break;
      }
    }
    if (newt_windowQueueRender()) newt_wait_synch();
  }

  /*TODO*/
  return (int)getchar();
};

/* ------------------------------------------------------------------------- */

void
newt_get_extcmd(input)
    char *input;
{
    int amountFound;
    int previousFind=0;
    int stepping;

    amountFound=0;
    for (stepping = 0; extcmdlist[stepping].ef_txt; stepping++) {
        if (extcmdlist[stepping].ef_txt[0] == '?') continue;
        if (strncmp(input, extcmdlist[stepping].ef_txt, strlen(input)-1)==0) {
            amountFound++;
            previousFind=stepping;
        }
    }

    if (amountFound==1) {
        strcpy(input, extcmdlist[previousFind].ef_txt);
        strcat(input, "_");
    }
}

/* ------------------------------------------------------------------------- */

/*
getlin(const char *ques, char *input)
        -- Prints ques as a prompt and reads a single line of text,
           up to a newline.  The string entered is returned without the
           newline.  ESC is used to cancel, in which case the string
           "\033\000" is returned.
        -- getlin() must call flush_screen(1) before doing anything.
        -- This uses the top line in the tty window-port, other
           ports might use a popup.
        -- getlin() can assume the input buffer is at least BUFSZ
           bytes in size and must truncate inputs to fit, including
           the nul character.
*/
void
newt_getlin (ques, input)
    const char *ques;
    char *input;
{
    char ch;
    int extcmd;

#ifdef DEBUG
    printf("- newt_getlin(\"%s\", \"%s\");\n",ques,input);
#endif

    extcmd = strcmp("#",ques)==0;

    newt_wait_synch();
    newt_putstr(WIN_MESSAGE, ATR_NONE, ques);

    *input=0;

    strcat(input,"_");

    newt_msghistory[(iflags.msg_history-1)*2+0]=realloc(newt_msghistory[(iflags.msg_history-1)*2+0],strlen(newt_msghistory[(iflags.msg_history-1)*2+0])+1+BUFSZ+1);

    ch=0;
    while (ch!='\n'&&ch!='\033') {
        sprintf(newt_msghistory[(iflags.msg_history-1)*2+0],"%s %s", ques, input);
        newt_display_nhwindow(WIN_MESSAGE, FALSE);
        ch=newt_nhgetch();
        switch (ch) {
            case '\b':
                if (strlen(input)>1) input[strlen(input)-2]='_';
                if (strlen(input)>1) input[strlen(input)-1]=0;
                break;
            case '\033':
                input[0]='\033';
                input[1]=0;
            case '\n':
                break;
            default:
                if (strlen(input)<BUFSZ-1) {
                    input[strlen(input)+1]=0;
                    input[strlen(input)-1]=ch;
                    input[strlen(input)]='_';
                    newt_get_extcmd(input);
                };
                break;
        }
    }

    input[strlen(input)-1]=0;
    newt_clear_nhwindow(WIN_MESSAGE);
};

/* ------------------------------------------------------------------------- */

/*
char yn_function(const char *ques, const char *choices, char default)
        -- Print a prompt made up of ques, choices and default.
           Read a single character response that is contained in
           choices or default.  If choices is NULL, all possible
           inputs are accepted and returned.  This overrides
           everything else.  The choices are expected to be in
           lower case.  Entering ESC always maps to 'q', or 'n',
           in that order, if present in choices, otherwise it maps
           to default.  Entering any other quit character (SPACE,
           RETURN, NEWLINE) maps to default.
        -- If the choices string contains ESC, then anything after
           it is an acceptable response, but the ESC and whatever
           follows is not included in the prompt.
        -- If the choices string contains a '#' then accept a count.
           Place this value in the global "yn_number" and return '#'.
        -- This uses the top line in the tty window-port, other
           ports might use a popup.
        -- If choices is NULL, all possible inputs are accepted and
           returned, preserving case (upper or lower.) This means that
           if the calling function needs an exact match, it must handle
           user input correctness itself.
*/
char
newt_yn_function (ques, choices, def)
    const char *ques;
    const char *choices;
    CHAR_P def;
{
    char str[BUFSZ];
    char choicedisp[BUFSZ];
    char *ch;
    char input;
    int any;

#ifdef DEBUG
    printf("- newt_yn_function(\"%s\",\"%s\",'%c');\n",
        ques, choices, def);
#endif
    /*TODO*/
    /* still need the '#' stuff */
    any=FALSE;
    if (choices) {
        strcpy(choicedisp, choices);
        ch=(char *)&choicedisp;
        while (*ch++) {
            if (*ch=='\033') {
                *ch=0;
                any=TRUE;
            }
        }
        if (def) {
            sprintf((char *)&str,"%s [%s] (%c)",ques,choicedisp,def);
        } else {
            sprintf((char *)&str,"%s [%s]",ques,choicedisp);
        }
    } else {
        any=TRUE;
        if (def) {
            sprintf((char *)&str,"%s (%c)",ques,def);
        } else {
            sprintf((char *)&str,"%s",ques);
        }
    }

  newt_raw_print((char *)&str);
  if (any) {
    input=newt_nhgetch();
  } else
  while (1) {
    input=newt_nhgetch();
    if (input=='\033') {
      if (strchr(choices, 'q')) input='q';
      if (strchr(choices, 'n')) input='n';
      input=def;
    } else      if (input==' '||input=='\n') {
      input=def;
    }
    if (strchr(choices, input)) break;
  }

  newt_clear_nhwindow(WIN_MESSAGE);
  return input;
};

/* ------------------------------------------------------------------------- */

/*
int get_ext_cmd(void)
        -- Get an extended command in a window-port specific way.
           An index into extcmdlist[] is returned on a successful
           selection, -1 otherwise.
*/
int
newt_get_ext_cmd (void)
{
    char input[BUFSZ];
    int  stepping;

#ifdef DEBUG
    printf("- newt_get_ext_cmd();\n");
#endif

    /* user wants extended menu ? If users does, then nethack makes
       is easy for us :D */
    if (iflags.extmenu) return extcmd_via_menu();

    newt_getlin("#", (char *)&input);
    if (strlen(input)<1) return -1;
    for (stepping = 0; extcmdlist[stepping].ef_txt; stepping++) {
        if (strncmp(input, extcmdlist[stepping].ef_txt, strlen(input))==0) {
            return stepping;
        }
    }

    return -1; /* failed */
};

/* ------------------------------------------------------------------------- */


