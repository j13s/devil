/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    devil.c - main()
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program (file COPYING); if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include "structs.h"
#include "userio.h"
#include "tools.h"
#include "grfx.h"
#include "readlvl.h"
#include "initio.h"
#include "tag.h"
#include "config.h"
#include "plottxt.h"
#include "version.h"
#include "do_event.h"
#include "askcfg.h"

/* Descent and Descent 2 level file extensions */
const char *extnames[desc_number] = {
    "SDL",  /* Descent Demo */
    "RDL",  /* Descent 1.0 */
    "RDL",  /* Descent 1.4 */
    "SL2",  /* Descent 2 Interactive Demo */
    "RL2",  /* Descent 2 1.0 */
    "RL2",  /* Descent 2 1.1 */
    "RL2"   /* Descent 2 1.2 (Vertigo) */
};

/* INI files for the option windows (Cube, edge, etc.) */
#ifdef GER
    const char *ininames[desc_number] = {
        NULL, "d1r_g.ini", "d1r_g.ini", NULL, "d2r_g_10.ini", "d2r_g_11.ini",
        "d2r_g_11.ini"
    };
#else
    /*! @var ininames
     *  @brief Lists the INI files for the differing versions of Descent and
     *         Descent 2.
     *
     *  The @ref descent enum aligns with the indices of the ininames array.
     *  If a Descent version is not editable, then a NULL char pointer is
     *  stored at that position in the array.
     */
    const char *ininames[desc_number] = {
        NULL, "d1reg.ini", "d1reg.ini", NULL, "d2reg_10.ini", "d2reg_11.ini",
        "d2reg_11.ini"
    };
#endif


/* Decoded Descent version strings */
/*! @var vernames
    @brief Strings that fully describe the encoded Descent version.

    Also aligns with the @ref descent enum.
 */
const char *vernames[desc_number] = {
    "Descent 1 V1.0 shareware",
    "Descent 1 V1.0 registered",
    "Descent 1 V1.4 (or higher) registered",
    "Descent 2 V1.0 shareware",
    "Descent 2 V1.0 registered",
    "Descent 2 V1.1 registered",
    "Descent 2 V1.2 (or higher) registered"
};

/* Stores information on program presentation and rendering details. */
struct viewdata view;
/* Store paths to different files, the Descent version, etc. */
struct initdata init;
/* Store textures from the current PIG file. */
struct pigdata pig;
/* Store the palette for the current level. */
struct palette palettes[NUM_PALETTES];
/* Store info about the path, full level name, in or outside of level, ...  */
struct leveldata *l;


/* Called if there is an assertion error. */
void my_exit(void) {
    /* I guess this is done to prevent savestatus from getting called multiple
       times in savestatus()? */
    static int no_loop = 0;


    /* Try to save the editor's status. */
    if (!no_loop) {
        no_loop = 1;
        fprintf(errf, "Severe bug. Trying to save current work...");
        savestatus(-1);
        fprintf(errf, "Done. Maybe you are a lucky guy.\n");
    }

    /* Go back to the console and explain the error. */
    ws_textmode();
    releasetimer();
    printf("Severe bug.\n");
    printf("This is the modified version of Devil made by\n");
    printf("Lars Christensen. Try to redo your last operation with\n");
    printf("the original version of Devil. If the problem persists then\n");
    printf("follow the instructions below, if not then don't bother Achim\n");
    printf("with this bug, it's probably introduced by me (Lars).\n");
    printf("------------------------------------------------------------\n");
    printf("Please have a look in the devil.err and try to\n");
    printf("reconstruct how this happened and mail your bug report to the\n");
    printf("Descent Designer Mailing List (see http://www.warpcore.org)\n");
    printf("If you're lucky, your current work will be restored when you\n");
    printf(
        "enter the program again. If this doesn't work and it won't start\n");
    printf("anymore at all, try devil /new.\n");
    printf("WARNING: It is not sure that your levels will be properly\n");
    printf("working after the reconstruction. You should save them\n");
    printf("under a different filename than the last 'real' saved version\n");
    exit(2);
}


const char *signame[5] = {
    "Unknown", "Floating point exception",
    "Illegal opcode", "Segment violation", "Terminate"
};
void my_abort(int sigcode) {
    int x;


    switch (sigcode) {
        case SIGFPE:
            x = 1;
            break;

        case SIGILL:
            x = 2;
            break;

        case SIGSEGV:
            x = 3;
            break;

        case SIGTERM:
            x = 4;
            break;

        default:
            x = 0;
    }

    if (errf != NULL) {
        fprintf(errf, "Unexpected signal: %s\n", signame[x]);
        fflush(errf);
    }
    else {
        printf("Unexpected signal: %s\n", signame[x]);
    }

    if (x == 1) {
        my_exit();
    }
    else {
        exit(2);
    }
}


/* Encodes the command line parameters. */
enum cmdline_params {
    clp_new, clp_notitle, clp_config, num_cmdlineparams
};

/* The command line parameters. */
const char *cmdline_switches[num_cmdlineparams] = {
    "NEW",      /* Write a new Devil configuation file? */
    "NOTITLE",  /* Skip the splash screen. */
    "CONFIG"    /* Configure Devil and exit without launching the editor. */
};

/* Strings for the usage statement if a bad parameter is passed to Devil. */
const char *cmdline_txts[num_cmdlineparams] = {
    TXT_CMDSTARTNEW, TXT_CMDDONTSHOWTITLE, TXT_CMDCONFIG
};


int main(int argn, char *argc[]) {
    int i, j, title = 1;    /* Show splash screen if title is true. */
    long int with_cfg = 1,
             reconfig = 0;
    char buffer[128];
    char* load_file_name = NULL;


    signal(SIGFPE, my_abort);
    signal(SIGILL, my_abort);
    signal(SIGSEGV, my_abort);
    signal(SIGTERM, my_abort);
    printf("Devil %s%s\nCompiler: %s\nCompiled: %s %s\n", VERSION, LC_VERSION,
           SYS_COMPILER_NAME, __DATE__, __TIME__);

    if (sizeof(float) != 4 || sizeof(long int) != 4 || sizeof(short int) != 2
       || sizeof(int) != 4 || sizeof(char) != 1) {
        printf("Wrong float/int size. Check your compiler flags.\n");
        exit(2);
    }

    errf = stdout;

    /* Process the command line arguments. */
    for (j = 1; j < argn; j++) {
        sscanf(argc[j], " %s", buffer);

        /* Parameters may be uppercase or lowercase. */
        for (i = 0; i < strlen(buffer); i++) {
            buffer[i] = toupper(buffer[i]);
        }

        /* Check to see if this is a parameter by looking at the first
           character.  If it is a forward-slash, it is a parameter.  If
           not then it is a level to be loaded by the editor and the filename
           is stored in load_file_name.
           
           Only one parameter can be used.  Once a match is found, the search
           for the rest stops. */
        if (buffer[0] == '/') {
            for (i = 0; i < num_cmdlineparams; i++) {
                /* If the parameter passed at runtime doesn't match a known
                   parameter, then skip it and move on to the next one. */
                if (strcmp(&buffer[1], cmdline_switches[i]) == 0) {
                    switch (i) {
                        /* Start Devil with a new configuration file? */
                        case clp_new:
                            with_cfg = 0;
                            break;
                            
                        /* Skip the splash screen if NOTITLE is specified. */
                        case clp_notitle:
                            printf("Devil is sponsored by PC Player!\n");
                            title = 0;
                            break;

                        /* If CONFIG is passed, then launch the configuration
                           menu and exit to the command line after Devil is
                           done being configured. */
                        case clp_config:
                            reconfig = 1;
                            break;
                    }

                    /* Break out of the for-loop once a match is found. */
                    break;
                }
            }
            
            /* If a parameter was passed that didn't match any of the known
               parameters, then print the usage statement for the user and
               exit. */
            if (i == num_cmdlineparams) {
                printf(TXT_CMDUNKNOWNPARAM, &buffer[1]);

                for (i = 0; i < num_cmdlineparams; i++) {
                    printf("%s -- %s\n", cmdline_switches[i], cmdline_txts[i]);
                }

                exit(1);
            }
        }
        else {
            /* The configuration file to load? */
            load_file_name = argc[j];
        }
    }

    initeditor(INIFILE, title);

    if ( !readconfig() ) {
        writeconfig(0);
    }
    else if (reconfig) {
        writeconfig(1);
    }

    initgrph(title);
    ws_disablectrlc();
    l = NULL;

    if (with_cfg) {
        readstatus(load_file_name);
    }
    else {
        printmsg(TXT_NOCFGFILE);
    }

    /*
       if(strlen(load_file_name))
       {
        for(n=view.levels.head;n->next!=NULL;n=n->next)
           closelevel(n->d.lev,1);
        openlevel(load_file_name);
       }
     */
    w_handleuser(0, NULL, 0, NULL, view.num_keycodes, view.ec_keycodes,
                 do_event);
    return 1;
}


