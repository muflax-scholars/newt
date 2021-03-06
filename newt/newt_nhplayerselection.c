#include "hack.h"
#include "newt_nhwindows.h"
#include "newt_nhmenus.h"
#include "newt_main.h"


/* ------------------------------------------------------------------------- */

static void
newt_player_selection_rolename(char *rolename) {
    int prev;

    rolename[0]=0;
    prev=FALSE;

    if (flags.initalign!=-1) {
        strcat(rolename,aligns[flags.initalign].adj);
        prev=TRUE;
    }

    if (flags.initgend!=-1) {
        if (prev) strcat(rolename, " ");
        strcat(rolename,genders[flags.initgend].adj);
        prev=TRUE;
    }

    if (flags.initrace!=-1) {
        if (prev) strcat(rolename, " ");
        strcat(rolename,races[flags.initrace].noun);
        prev=TRUE;
    }

    if (flags.initrole!=-1) {
        if (prev) strcat(rolename, " ");
        if (flags.initgend==1&&roles[flags.initrole].name.f) {
            strcat(rolename,roles[flags.initrole].name.f);
        } else {
            strcat(rolename,roles[flags.initrole].name.m);
        }
        prev=TRUE;
    }

}

/* ------------------------------------------------------------------------- */

static int
newt_player_selection_role (void) {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen,items,item=0;
    char rolename[BUFSZ];
    char selection[BUFSZ];
    char thisch, lastch;

    menu_list=0;
    window=newt_create_nhwindow(NHW_MENU);
    newt_start_menu(window);
    items=0;
    lastch=0;
    for (chosen=0;roles[chosen].name.m||roles[chosen].name.f;chosen++) {
        identifier.a_int=chosen+1;
        if (ok_role(chosen,flags.initrace,flags.initgend,flags.initalign)) {
            items++;
            item=chosen;
            if (flags.initgend==1&&roles[chosen].name.f) {
                thisch = lowc(roles[chosen].name.f[0]);
                if (thisch == lastch) thisch = highc(thisch);
                newt_add_menu ( window, NO_GLYPH, &identifier, thisch, 0, 0, roles[chosen].name.f, chosen==flags.initrole);
            } else {
                thisch = lowc(roles[chosen].name.m[0]);
                if (thisch == lastch) thisch = highc(thisch);
                newt_add_menu ( window, NO_GLYPH, &identifier, thisch, 0, 0, roles[chosen].name.m, chosen==flags.initrole);
            }
            lastch=thisch;
        }
    }

    if (items==1) {
        flags.initrole=item;
        free(menu_list);
        newt_destroy_nhwindow(window);
        return 1;
    }

    identifier.a_int=-1; newt_add_menu ( window, NO_GLYPH, &identifier, '*', 0, 0, "Random", 0);
    newt_player_selection_rolename((char *)&rolename);
    sprintf(selection,"Pick a role for your %s", strlen(rolename) ? rolename : "character");
    newt_end_menu(window, selection);
    if (newt_select_menu (window, PICK_ONE, &menu_list)==1) {
        chosen=menu_list->item.a_int;
        if (chosen==-1) {
            chosen=pick_role(flags.initrace,flags.initgend,flags.initalign,PICK_RANDOM);
        } else {
            chosen--;
        }
        free(menu_list);
        flags.initrole=chosen;
        newt_destroy_nhwindow(window);
        return 1;
    };
    flags.initrole=-1;
    newt_destroy_nhwindow(window);
    return 0;

}

/* ------------------------------------------------------------------------- */

static int
newt_player_selection_race (void) {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen,items,item=0;
    char rolename[BUFSZ];
    char selection[BUFSZ];
    char thisch, lastch;

    menu_list=0;
    window=newt_create_nhwindow(NHW_MENU);
    newt_start_menu(window);
    items=0;
    lastch=0;
    for (chosen=0;races[chosen].noun;chosen++) {
        identifier.a_int=chosen+1;
        if (ok_race(flags.initrole,chosen,flags.initgend,flags.initalign)) {
            items++;
            item=chosen;
            thisch = lowc(races[chosen].noun[0]);
            if (thisch == lastch) thisch = highc(thisch);
            lastch=thisch;
            newt_add_menu ( window, NO_GLYPH, &identifier, thisch, 0, 0, races[chosen].noun, chosen==flags.initrace);
        }
    }

    if (items==1) {
        flags.initrace=item;
        free(menu_list);
        newt_destroy_nhwindow(window);
        return 1;
    }

    identifier.a_int=-1; newt_add_menu ( window, NO_GLYPH, &identifier, '*', 0, 0, "Random", 0);

    newt_player_selection_rolename((char *)&rolename);
    sprintf(selection,"Pick the race of your %s", strlen(rolename) ? rolename : "character");
    newt_end_menu(window, selection);

    if (newt_select_menu (window, PICK_ONE, &menu_list)==1) {
        chosen=menu_list->item.a_int;
        if (chosen==-1) {
            chosen=pick_race(flags.initrole,flags.initgend,flags.initalign,PICK_RANDOM);
        } else {
            chosen--;
        }
        free(menu_list);
        flags.initrace=chosen;
        newt_destroy_nhwindow(window);
        return 1;
    };
    flags.initrace=-1;
    newt_destroy_nhwindow(window);
    return 0;

}

/* ------------------------------------------------------------------------- */

static int
newt_player_selection_gender (void) {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen,item=0,items;
    char rolename[BUFSZ];
    char selection[BUFSZ];
    char thisch, lastch;

    menu_list=0;
    window=newt_create_nhwindow(NHW_MENU);
    newt_start_menu(window);
    chosen=0;
    items=0;
    lastch=0;
    while (chosen<ROLE_GENDERS) {
        identifier.a_int=chosen+1;
        if (ok_gend(flags.initrole,flags.initrole,chosen,flags.initalign)) {
            thisch = lowc(genders[chosen].adj[0]);
            if (thisch == lastch) thisch = highc(thisch);
            lastch=thisch;
            newt_add_menu ( window, NO_GLYPH, &identifier, thisch, 0, 0, genders[chosen].adj, chosen==flags.initgend);
            items++;
            item=chosen;
        }
        chosen++;
    }

    if (items==1) {
        flags.initgend=item;
        free(menu_list);
        newt_destroy_nhwindow(window);
        return 1;
    }

    identifier.a_int=-1; newt_add_menu ( window, NO_GLYPH, &identifier, '*', 0, 0, "Random", 0);

    newt_player_selection_rolename((char *)&rolename);
    sprintf(selection,"Pick the gender of your %s", strlen(rolename) ? rolename : "character");
    newt_end_menu(window, selection);

    if (newt_select_menu (window, PICK_ONE, &menu_list)==1) {
        chosen=menu_list->item.a_int;
        if (chosen==-1) {
            chosen=pick_gend(flags.initrole,flags.initrace,flags.initalign,PICK_RANDOM);
        } else {
            chosen--;
        }
        flags.initgend=chosen;
        free(menu_list);
        newt_destroy_nhwindow(window);
        return 1;
    };
    flags.initgend=-1;
    newt_destroy_nhwindow(window);
    return 0;
}

/* ------------------------------------------------------------------------- */

static int
newt_player_selection_alignment (void) {
    winid window;
    anything identifier;
    menu_item *menu_list;
    int chosen,items,item=0;
    char rolename[BUFSZ];
    char selection[BUFSZ];
    char thisch, lastch;

    menu_list=0;
    window=newt_create_nhwindow(NHW_MENU);
    newt_start_menu(window);
    chosen=0;
    items=0;
    lastch=0;
    while (chosen<ROLE_ALIGNS) {
        identifier.a_int=chosen+1;
        if (ok_align(flags.initrole,flags.initrace,flags.initgend,chosen)) {
            items++;
            item=chosen;
            thisch = lowc(aligns[chosen].adj[0]);
            if (thisch == lastch) thisch = highc(thisch);
            lastch=thisch;
            newt_add_menu ( window, NO_GLYPH, &identifier, thisch, 0, 0,
                aligns[chosen].adj, chosen==flags.initalign);
        }
        chosen++;
    }

    if (items==1) {
        flags.initalign=item;
        free(menu_list);
        newt_destroy_nhwindow(window);
        return 1;
    }

    identifier.a_int=-1; newt_add_menu ( window, NO_GLYPH, &identifier, '*', 0, 0, "Random", 0);

    newt_player_selection_rolename((char *)&rolename);
    sprintf(selection,"Pick the alignment of your %s", strlen(rolename) ? rolename : "character");
    newt_end_menu(window, selection);

    if (newt_select_menu (window, PICK_ONE, &menu_list)==1) {
        chosen=menu_list->item.a_int;
        if (chosen==-1) {
            chosen=pick_align(flags.initrole,flags.initrace,flags.initgend,PICK_RANDOM);
        } else {
	    chosen--;
        }
        flags.initalign=chosen;
        free(menu_list);
        newt_destroy_nhwindow(window);
        return 1;
    };
    flags.initalign=-1;
    newt_destroy_nhwindow(window);
    return 0;
}

/* ------------------------------------------------------------------------- */

/*
player_selection()
		-- Do a window-port specific player type selection.  If
		   player_selection() offers a Quit option, it is its
		   responsibility to clean up and terminate the process.
		   You need to fill in pl_character[0].
*/
void
newt_player_selection (void)
{
#ifdef DEBUG
    printf("- newt_player_selection();\n");
#endif
    /* randomize those selected as "random" within .nethackrc */
    if ( (flags.initrole ==ROLE_RANDOM) ||
         (flags.randomall && (flags.initrole ==ROLE_NONE) ) )
        flags.initrole =
        pick_role(flags.initrace,flags.initgend,flags.initalign,PICK_RANDOM);
    if ( (flags.initrace ==ROLE_RANDOM) ||
         (flags.randomall && (flags.initrace ==ROLE_NONE)))
        flags.initrace =
        pick_race(flags.initrole,flags.initgend,flags.initalign,PICK_RANDOM);
    if ( (flags.initgend ==ROLE_RANDOM) ||
         (flags.randomall && (flags.initgend ==ROLE_NONE)))
        flags.initgend =
        pick_gend(flags.initrole,flags.initrace,flags.initalign,PICK_RANDOM);
    if ( (flags.initalign==ROLE_RANDOM) ||
         (flags.randomall && (flags.initalign==ROLE_NONE)))
        flags.initalign =
        pick_align(flags.initrole,flags.initrace,flags.initgend ,PICK_RANDOM);

    /* here this interface should present it's character selection */
    switch (newt_yn_function("Shall I pick a character's race, role, gender and alignment for you?","ynq",'y')) {
        case 'q':
            clearlocks();
            newt_exit_nhwindows("Quit from character selection.");
            terminate(EXIT_SUCCESS);
        case 'y':
            break;
        case 'n':
            newt_player_selection_role();
            newt_player_selection_race();
            newt_player_selection_gender();
            newt_player_selection_alignment();
            break;
    }

    /* any manually selected as random or left out ? randomize them ... */
    if (flags.initrole == ROLE_NONE)
        flags.initrole =
        pick_role(flags.initrace,flags.initgend,flags.initalign,PICK_RANDOM);
    if (flags.initrace == ROLE_NONE)
        flags.initrace =
        pick_race(flags.initrole,flags.initgend,flags.initalign,PICK_RANDOM);
    if (flags.initgend == ROLE_NONE)
        flags.initgend =
        pick_align(flags.initrole,flags.initrace,flags.initalign,PICK_RANDOM);
    if (flags.initalign == ROLE_NONE)
        flags.initalign =
        pick_gend(flags.initrole,flags.initrace,flags.initgend ,PICK_RANDOM);

}


/* ------------------------------------------------------------------------- */

