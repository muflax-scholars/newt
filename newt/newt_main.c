#include "hack.h"

#include "newt_main.h"

#include "newt_nhwindows.h"
#include "newt_nhmenus.h"
#include "newt_nhinput.h"
#include "newt_nhoutput.h"
#include "newt_nhunixtty.h"
#include "newt_nhpaletted.h"
#include "newt_globals.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#ifdef USER_SOUNDS
# include <SDL_mixer.h>
#endif

/* Interface definition, for windows.c */
/*
    if any of these are not required by your interface system,
    simply replace the function name in this list with :
        donull
*/
struct window_procs newt_procs = {
    "newt",
    WC_COLOR |
    WC_HILITE_PET |
    WC_TILED_MAP |
    WC_PRELOAD_TILES |
    WC_TILE_WIDTH |
    WC_TILE_HEIGHT |
    WC_TILE_FILE |
    WC_MAP_MODE |
    WC_TILED_MAP |
    WC_PLAYER_SELECTION,
#ifdef POSITIONBAR_RCOPTION
    WC2_POSITIONBAR |
#endif
    WC2_FULLSCREEN,
/* example of a "null" function, one that isn't used :
 *
    donull,
*/
    newt_init_nhwindows,        /* newt_main.c */
    newt_player_selection,      /* newt_main.c */
    newt_askname,               /* newt_main.c */
    newt_get_nh_event,          /* newt_main.c */
    newt_exit_nhwindows,        /* newt_main.c */
    newt_suspend_nhwindows,     /* newt_main.c */
    newt_resume_nhwindows,      /* newt_main.c */
    newt_create_nhwindow,       /* newt_nhwindows.c */
    newt_clear_nhwindow,        /* newt_nhwindows.c */
    newt_display_nhwindow,      /* newt_nhwindows.c */
    newt_destroy_nhwindow,      /* newt_nhwindows.c */
    newt_curs,                  /* newt_main.c */
    newt_putstr,                /* newt_nhoutput.c */
    newt_display_file,          /* newt_nhoutput.c */
    newt_start_menu,            /* newt_nhmenus.c */
    newt_add_menu,              /* newt_nhmenus.c */
    newt_end_menu,              /* newt_nhmenus.c */
    newt_select_menu,           /* newt_nhmenus.c */
    newt_message_menu,          /* newt_nhmenus.c */
    newt_update_inventory,      /* newt_main.c */
    newt_mark_synch,            /* newt_main.c */
    newt_wait_synch,            /* newt_main.c */
#ifdef CLIPPING
    newt_cliparound,            /* newt_main.c */
#endif
#ifdef POSITIONBAR
    newt_update_positionbar,    /* newt_main.c */
#endif
    newt_print_glyph,           /* newt_nhoutput.c */
    newt_raw_print,             /* newt_nhoutput.c */
    newt_raw_print_bold,        /* newt_nhoutput.c */
    newt_nhgetch,               /* newt_nhinput.c */
    newt_nh_poskey,             /* newt_nhinput.c */
    newt_nhbell,                /* newt_main.c */
    newt_doprev_message,        /* newt_main.c */
    newt_yn_function,           /* newt_nhinput.c */
    newt_getlin,                /* newt_nhinput.c */
    newt_get_ext_cmd,           /* newt_nhinput.c */
    newt_number_pad,            /* newt_main.c */
    newt_delay_output,          /* newt_main.c */
#ifdef CHANGE_COLOR		/* the Mac uses a palette device */
    newt_change_colour,         /* newt_nhpaletted.c */
#ifdef MAC
    newt_change_background,     /* newt_nhpaletted.c */
    newt_set_font_name,         /* newt_nhpaletted.c */
#endif
    newt_get_colour_string,     /* newt_nhpaletted.c */
#endif
    newt_start_screen,          /* newt_nhunixtty.c */
    newt_end_screen,            /* newt_nhunixtty.c */
    newt_outrip,                /* newt_main.c */
    newt_preference_update      /* newt_main.c */
};


/* ------------------------------------------------------------------------- */

void
newt_win_init(void)
{
#ifdef DEBUG
    printf("- [done] newt_win_init();\n");
#endif
    /* ignore this functions completely !
     * this function exists purely as as "locator" for src/windows.c's
     * interface into your code.
     */
};

/* ------------------------------------------------------------------------- */

/*
init_nhwindows(int* argcp, char** argv)
		-- Initialize the windows used by NetHack.  This can also
		   create the standard windows listed at the top, but does
		   not display them.
		-- Any commandline arguments relevant to the windowport
		   should be interpreted, and *argcp and *argv should
		   be changed to remove those arguments.
		-- When the message window is created, the variable
		   iflags.window_inited needs to be set to TRUE.  Otherwise
		   all plines() will be done via raw_print().
		** Why not have init_nhwindows() create all of the "standard"
		** windows?  Or at least all but WIN_INFO?	-dean
*/
void
newt_init_nhwindows (argcp, argv)
    int *argcp;
    char **argv;
{
	char nhversion[BUFSZ];
	char version[BUFSZ];

    int joyCount;

#ifdef DEBUG
    char **debug_argv;
#endif
    SDL_Rect dstrect;

#ifdef DEBUG
    printf("- newt_init_nhwindows(%d, \"",*argcp);
    debug_argv=argv;
    if (*debug_argv) {
        printf("%s",*debug_argv);
        debug_argv++;
    }
    while (*debug_argv) {
        printf(" %s",*debug_argv);
        debug_argv++;
    }
    printf("\");\n");
#endif

    if (!(iflags.wc_tile_file && *iflags.wc_tile_file)) {
        if (iflags.wc_tile_file) free (iflags.wc_tile_file);
        iflags.wc_tile_file=malloc(sizeof(NEWT_DEFAULT_TILESET)+1);
        strcpy(iflags.wc_tile_file,NEWT_DEFAULT_TILESET);
        iflags.wc_tile_width=NEWT_DEFAULT_TILESET_WIDTH;
        iflags.wc_tile_height=NEWT_DEFAULT_TILESET_HEIGHT;
        printf("defaulting to tileset \"%s\"(%dx%d), to change this, edit \"tile_file\" within your \"%s\"\n",
        iflags.wc_tile_file,
        iflags.wc_tile_width,
        iflags.wc_tile_height,
        configfile);
    } else {
        if (!iflags.wc_tile_width || !iflags.wc_tile_height) {
            printf("you *must* set the \"tile_width\" and the \"tile_height\" values for \"%s\" tileset within your \"%s\".\n",
            iflags.wc_tile_file,
            configfile);
            exit(-1);
        }
    }

    if (SDL_Init(SDL_INIT_NOPARACHUTE)) {
        printf("SDL_Init: init failed. \"%s\"\n", SDL_GetError());
        exit(-1);
    }
    atexit(SDL_Quit);

    if (SDL_InitSubSystem(SDL_INIT_TIMER)) {
        printf("SDL_InitSubSystem: init subsystem(TIMER) failed. \"%s\"\n", SDL_GetError());
        exit(-1);
    }

    if (SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        printf("SDL_InitSubSystem: init subsystem(VIDEO) failed. \"%s\"\n", SDL_GetError());
        exit(-1);
    }

#ifdef USER_SOUNDS
    
    if (SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        printf("SDL_InitSubSystem: init subsystem(AUDIO) failed. \"%s\"\n", SDL_GetError());
        // exit(-1);
    }

    if ((UseAudio=SDL_WasInit(SDL_INIT_AUDIO))) {
        if (Mix_OpenAudio(44100,AUDIO_S16,1,512)<0) {
            printf("Sound Initialisation Failed, going silent\n");
            UseAudio=0;
        }
    }

#endif    
    
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
        printf("SDL_InitSubSystem: init subsystem(JOYSTICK) failed. \"%s\"\n", SDL_GetError());
        // exit(-1);
    }

    if (SDL_WasInit(SDL_INIT_JOYSTICK)) {
        for (joyCount=0;joyCount<SDL_NumJoysticks();joyCount++) {
            printf("Initialising Joystick '%d:%s' : ",joyCount,SDL_JoystickName(joyCount));
            if (SDL_JoystickOpen(joyCount)) {
                printf("Ok");
            } else {
                printf("FAILED");
            }
            printf("\n");
        };
    }

    newt_screen=SDL_SetVideoMode(800,600,VideoBPP, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE | SDL_ASYNCBLIT );
    VideoModes=SDL_ListModes(newt_screen->format, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE | SDL_FULLSCREEN | SDL_ASYNCBLIT);

    if (VideoModes) {
        VideoModeAmount=0;
        printf("Available %dbpp video modes:\n", VideoBPP);
        while (*(VideoModes+VideoModeAmount)) {
            printf("    %3d: %dx%d\n",VideoModeAmount,(*(VideoModes+VideoModeAmount))->w,(*(VideoModes+VideoModeAmount))->h);
            if ((iflags.wc_tile_height*ROWNO+newt_fontsize*4)<(*(VideoModes+VideoModeAmount))->h&&(*(VideoModes+VideoModeAmount))->h<(*(VideoModes+VideoMode))->h) VideoMode=VideoModeAmount;
            VideoModeAmount++;
        }

        if (VideoMode>=VideoModeAmount) {
            printf("Attempted to set impossibly enumerated video mode.\n");
            exit(-1);
        }

        printf("Selected  %3d: %dx%d\n", VideoMode, (*(VideoModes+VideoMode))->w,(*(VideoModes+VideoMode))->h);

        newt_screen=SDL_SetVideoMode((*(VideoModes+VideoMode))->w,(*(VideoModes+VideoMode))->h,VideoBPP, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE | SDL_ASYNCBLIT | (iflags.wc2_fullscreen ? SDL_FULLSCREEN : 0));
    }

    if (!newt_screen) {
        printf("SDL_Init: setting video mode failed. \"%s\"\n",SDL_GetError());
        exit(-1);
    }

    if (!(newt_tiles=IMG_Load(iflags.wc_tile_file))) {
        printf("Failed to load tileset from : \"%s\"\n",iflags.wc_tile_file);
        exit(-1);
    }
    printf("Loaded tileset : \"%s\"\n",iflags.wc_tile_file );

    TTF_Init();
    newt_font = TTF_OpenFont("ASCII.ttf",newt_fontsize);
    if (!newt_font) {
        printf("Failed to load default font from : \"%s\"\n", "ASCII.ttf");
        exit(-1);
    }
    
    newt_fontsize=TTF_FontHeight(newt_font);

	getversionstring((char *)&nhversion);
	sprintf((char *)&version,
        "%s-%d.%d.%d - %s","newt",
        NEWT_VERSION_VERSION,
        NEWT_VERSION_RELEASE,
        NEWT_VERSION_PATCHLEVEL,
        (char *)&nhversion);
	SDL_WM_SetCaption(version,NULL);
    SDL_WM_GrabInput(SDL_GRAB_FULLSCREEN);

    SDL_EnableUNICODE (1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

#ifdef LINEOFSIGHT
    newt_lineofsight = SDL_CreateRGBSurface(
	SDL_SWSURFACE | SDL_SRCALPHA,
	iflags.wc_tile_width, iflags.wc_tile_height, VideoBPP,
	newt_screen->format->Rmask,
	newt_screen->format->Gmask,
	newt_screen->format->Bmask,
	newt_screen->format->Amask);
    dstrect.x=dstrect.y=0;
    dstrect.w=iflags.wc_tile_width;
    dstrect.h=iflags.wc_tile_height;
    SDL_FillRect( newt_lineofsight, (SDL_Rect *)&dstrect, 0);
    if (newt_lineofsight) {
	SDL_SetAlpha( newt_lineofsight, SDL_SRCALPHA, SDL_ALPHA_TRANSPARENT + (SDL_ALPHA_OPAQUE - SDL_ALPHA_TRANSPARENT)/3);
    }
#endif

    newt_infrared = SDL_CreateRGBSurface(
        SDL_SWSURFACE | SDL_SRCALPHA,
        iflags.wc_tile_width, iflags.wc_tile_height, VideoBPP,
        newt_screen->format->Rmask,
        newt_screen->format->Gmask,
        newt_screen->format->Bmask,
        newt_screen->format->Amask);
    dstrect.x=dstrect.y=0;
    dstrect.w=iflags.wc_tile_width;
    dstrect.h=iflags.wc_tile_height;
    if (newt_infrared) {
        SDL_FillRect( newt_infrared, (SDL_Rect *)&dstrect, SDL_MapRGB(newt_infrared->format,0xFF,0,0));
        SDL_SetAlpha( newt_infrared, SDL_SRCALPHA, SDL_ALPHA_TRANSPARENT + (SDL_ALPHA_OPAQUE - SDL_ALPHA_TRANSPARENT)/5);
    }

  iflags.window_inited=TRUE;
}

/* ------------------------------------------------------------------------- */

/*
askname()	-- Ask the user for a player name.
*/
void
newt_askname (void)
{
#ifdef DEBUG
    printf("- newt_askname();\n");
#endif
    /*TODO*/
    /* a simple working example : */
    newt_getlin("What is your name?",plname);
};

/* ------------------------------------------------------------------------- */

/*
get_nh_event()	-- Does window event processing (e.g. exposure events).
		   A noop for the tty and X window-ports.
*/
void
newt_get_nh_event (void)
{
#ifdef DEBUG
    printf("- newt_get_nh_event();\n");
#endif
    /*TODO*/
};

/* ------------------------------------------------------------------------- */

/*
exit_nhwindows(str)
		-- Exits the window system.  This should dismiss all windows,
		   except the "window" used for raw_print().  str is printed
		   if possible.
*/
void
newt_exit_nhwindows (str)
    const char *str;
{
#ifdef DEBUG
  printf("- [done] newt_exit_nhwindows(\"%s\");\n",str);
#endif

  iflags.window_inited=FALSE;
  SDL_Quit();

  newt_raw_print(str);
}

/* ------------------------------------------------------------------------- */

/*
suspend_nhwindows(str)
		-- Prepare the window to be suspended.
*/
void
newt_suspend_nhwindows (str)
    const char *str;
{
#ifdef DEBUG
    printf("- newt_suspend_nhwindows(\"%s\");\n",str);
#endif
    /* this is normally called on ^Z within *nix systems */
    /*TODO*/
};

/* ------------------------------------------------------------------------- */

/*
resume_nhwindows()
		-- Restore the windows after being suspended.
*/
void
newt_resume_nhwindows (void)
{
#ifdef DEBUG
    printf("- newt_resume_nhwindows();\n");
#endif
    /*TODO*/
};

/* ------------------------------------------------------------------------- */

/*
curs(window, x, y)
		-- Next output to window will start at (x,y), also moves
		   displayable cursor to (x,y).  For backward compatibility,
		   1 <= x < cols, 0 <= y < rows, where cols and rows are
		   the size of window.
		-- For variable sized windows, like the status window, the
		   behavior when curs() is called outside the window's limits
		   is unspecified. The mac port wraps to 0, with the status
		   window being 2 lines high and 80 columns wide.
		-- Still used by curs_on_u(), status updates, screen locating
		   (identify, teleport).
		-- NHW_MESSAGE, NHW_MENU and NHW_TEXT windows do not
		   currently support curs in the tty window-port.
*/
void
newt_curs (window, x, y)
    winid window;
    int x;
    int y;
{
#ifdef DEBUG
    printf("- newt_curs(%d, %d, %d);\n", window, x, y);
#endif
    /* "window" is the result as you returned it from create_nhwindow */

	if (window==WIN_STATUS) {
		newt_status_curs_x=x;
		newt_status_curs_y=y;
	} else
	if (window==WIN_MAP) {
		newt_map_curs_x=x;
		newt_map_curs_y=y;
	};
};


/* ------------------------------------------------------------------------- */

/*
update_inventory()
		-- Indicate to the window port that the inventory has been
		   changed.
		-- Merely calls display_inventory() for window-ports that
		   leave the window up, otherwise empty.
*/
void
newt_update_inventory (void)
{
#ifdef DEBUG
    printf("- [done] newt_update_inventory();\n");
#endif
    if (flags.perm_invent) display_inventory(NULL,FALSE);
};


/* ------------------------------------------------------------------------- */

/*
mark_synch()
        -- Don't go beyond this point in I/O on any channel until
		   all channels are caught up to here.  Can be an empty call
		   for the moment
*/
void
newt_mark_synch (void)
{
#ifdef DEBUG
    printf("- [done] newt_mark_synch();\n");
#endif

    /*TODO*/
    /* actullay, not "TODO", you can leave this one empty for now */
};

/* ------------------------------------------------------------------------- */

/*
wait_synch()
        -- Wait until all pending output is complete (*flush*() for
		   streams goes here).
		-- May also deal with exposure events etc. so that the
		   display is OK when return from wait_synch().
*/
void
newt_wait_synch (void)
{
#ifdef DEBUG
    printf("- [done] newt_wait_synch();\n");
#endif
    if (iflags.window_inited) {
        SDL_Flip(newt_screen);
    }
};

/* ------------------------------------------------------------------------- */

/*
cliparound(x, y)
        -- Make sure that the user is more-or-less centered on the
		   screen if the playing area is larger than the screen.
		-- This function is only defined if CLIPPING is defined.
*/
#ifdef CLIPPING
void
newt_cliparound (x, y)
    int x;
    int y;
{
#ifdef DEBUG
    printf("- [done] newt_cliparound(%d, %d);\n", x, y);
#endif
    newt_centre_x=x;
    newt_centre_y=y;
};
#endif

/* ------------------------------------------------------------------------- */

/*
update_positionbar(char *features)
		-- Optional, POSITIONBAR must be defined. Provide some
		   additional information for use in a horizontal
		   position bar (most useful on clipped displays).
		   Features is a series of char pairs.  The first char
		   in the pair is a symbol and the second char is the
		   column where it is currently located.
		   A '<' is used to mark an upstairs, a '>'
		   for a downstairs, and an '@' for the current player
		   location. A zero char marks the end of the list.
*/
#ifdef POSITIONBAR
void
newt_update_positionbar (features)
    char *features;
{
#ifdef DEBUG
    char *debug_features;
    printf("- [done] newt_update_positionbar(");
    debug_features=features;
    printf("(%2d,",*debug_features);
    debug_features++;
    printf("%c)",*debug_features);
    debug_features++;
    while (*debug_features)
    {
        printf(",(%2d,",*debug_features);
        debug_features++;
        printf("%c)",*debug_features);
        debug_features++;
    }
    printf(");\n");
#endif

    strcpy(newt_positionbar,features);

};
#endif

/* ------------------------------------------------------------------------- */

/*
nhbell()
        -- Beep at user.  [This will exist at least until sounds are
		   redone, since sounds aren't attributable to windows anyway.]
*/
void
newt_nhbell (void)
{
#ifdef DEBUG
    printf("- newt_nhbell();\n");
#endif

    /*TODO*/
};

/* ------------------------------------------------------------------------- */

/*
doprev_message()
		-- Display previous messages.  Used by the ^P command.
		-- On the tty-port this scrolls WIN_MESSAGE back one line.
*/
int
newt_doprev_message (void)
{
#ifdef DEBUG
    printf("- newt_doprev_message();\n");
#endif
    if (!newt_prevmessage_firstcall)
    {
        newt_display_msghistory--;
        if (newt_display_msghistory<newt_current_msghistory)
            newt_display_msghistory=iflags.msg_history-1;
        while (! (newt_msghistory[(newt_display_msghistory)*2+0] &&
                strlen(newt_msghistory[(newt_display_msghistory)*2+0])))
        {
            newt_display_msghistory--;
            if (newt_display_msghistory<newt_current_msghistory)
            {
                newt_display_msghistory=iflags.msg_history-1;
                break;
            }
        }
    }
    newt_clear_nhwindow(WIN_MESSAGE);
    newt_message_blank=FALSE;
    newt_display_nhwindow(WIN_MESSAGE, FALSE);
    newt_prevmessage_firstcall=FALSE;

    /*TODO*/
    return 0;
};

/* ------------------------------------------------------------------------- */

/*
number_pad(state)
		-- Initialize the number pad to the given state.
*/
void
newt_number_pad (state)
    int state;
{
#ifdef DEBUG
    printf("- newt_number_pad(%d);\n", state);
#endif
    /*TODO*/

    /* most interfaces simply ignore this function, oh well ... */
    switch (state)
    {
        case -1:        /* keypad mode, escape sequenses [numlock off] */
            break;
        case 1:         /* numeric mode for keypad (digits) [numlock on] */
            break;
        case 0:         /* dont do anything, leave terminal "as-is" */
            break;
    }
};

/* ------------------------------------------------------------------------- */

/*
delay_output()
        -- Causes a visible delay of 50ms in the output.
		   Conceptually, this is similar to wait_synch() followed
		   by a nap(50ms), but allows asynchronous operation.
*/
void
newt_delay_output (void)
{
#ifdef DEBUG
    printf("- [done] newt_delay_output();\n");
#endif
    newt_wait_synch();
    SDL_Delay(50);
};

/* ------------------------------------------------------------------------- */

/*
outrip(winid, int)
		-- The tombstone code.  If you want the traditional code use
		   genl_outrip for the value and check the #if in rip.c.
*/
void
newt_outrip(window, how)
    winid window;
    int how;
{
#ifdef DEBUG
    printf("- [done] newt_outrip(%d,%d);\n", window, how);
#endif
   /*TODO*/

    /* simple text tombstone from src/rip.c.
     * if you get compile errors at this point, remember to add
     * "NEWT" to the #ifdef checks within rip.c
     * .. or remove this call, and add your own pretty tombstone :D
     */
	genl_outrip(window,how);
};

/* ------------------------------------------------------------------------- */

/*
preference_update(preference)
		-- The player has just changed one of the wincap preference
		   settings, and the NetHack core is notifying your window
		   port of that change.  If your window-port is capable of
		   dynamically adjusting to the change then it should do so.
		   Your window-port will only be notified of a particular
		   change if it indicated that it wants to be by setting the
		   corresponding bit in the wincap mask.
*/
void
newt_preference_update(pref)
    const char *pref;
{
#ifdef DEBUG
    printf("- newt_preference_update(\"%s\");\n", pref);
#endif
    /*TODO*/

#ifdef POSITIONBAR_RCOPTION
    if (!strcmp(pref,"position_bar")) {
      if (iflags.wc_position_bar)
        newt_positionbarmode=NEWT_POSITIONBARMODE_FULL;
      else
        newt_positionbarmode=NEWT_POSITIONBARMODE_NONE;
    }
#endif

    genl_preference_update(pref);
}

/* ------------------------------------------------------------------------- */

void newt_stretch_fast(source_surface, source_rect, destination_surface)
    SDL_Surface *source_surface;
    SDL_Rect *source_rect;
    SDL_Surface *destination_surface;
{
    int sx, sy, dx, dy;
    Uint32 *pixel_s;
    Uint32 *pixel_d;
    SDL_Rect dest_rect;

    if ( (source_rect->w==destination_surface->w) &&
         (source_rect->h==destination_surface->h) )
    {
        dest_rect.x=dest_rect.y=0;
        dest_rect.w=source_rect->w;
        dest_rect.h=source_rect->h;
        SDL_BlitSurface(
            source_surface, source_rect,
            destination_surface, &dest_rect );
        return;
    }

    SDL_LockSurface(source_surface);
    SDL_LockSurface(destination_surface);

    for (dy=0;dy<destination_surface->h; dy++)
    {
        sy=((dy)*source_rect->h)/destination_surface->h;
        sy+=source_rect->y;
        for (dx=0;dx<destination_surface->w; dx++)
        {
            sx=((dx)*source_rect->w)/destination_surface->w;
            sx+=source_rect->x;
            pixel_s=source_surface->pixels +
                (sy*(Uint32)source_surface->pitch) +
                sx*source_surface->format->BytesPerPixel;
            pixel_d=destination_surface->pixels +
                (dy*(Uint32)destination_surface->pitch) +
                dx*destination_surface->format->BytesPerPixel;
            *pixel_d =
                (*pixel_d & !(
                    destination_surface->format->Rmask |
                    destination_surface->format->Gmask |
                    destination_surface->format->Bmask |
                    destination_surface->format->Amask)) |
                (*pixel_s & (
                    source_surface->format->Rmask |
                    source_surface->format->Gmask |
                    source_surface->format->Bmask |
                    source_surface->format->Amask));
        }
    }

    SDL_UnlockSurface(source_surface);
    SDL_UnlockSurface(destination_surface);
}

/* ------------------------------------------------------------------------- */

