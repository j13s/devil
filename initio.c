/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    initio.c - reading "devil.ini" file
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
#include "macros.h"
#include "insert.h"
#include "readlvl.h"
#include "do_event.h"
#include "initio.h"

void readpoint(FILE *f, struct point *p);
int readobjtype(FILE *f, struct objtype *ot);
void readstring(FILE *f, char **s);
void printobjtypelists(struct infoitem *is, int num, int indent);

/*! @var   init_test
 *  @brief Stores the debugging message level for various INI files.
 *
 *         Ranges from 0-8, with zero being the lowest level.  More
 *         information is needed here.
 */
int init_test;

/*! @var   txtoffsets
 *  @brief Stores an integer array of texture offsets for the Descent and
 *         Descent 2 PIG files?
 *
 *  Each index in the array corresponds to a Descent version.  The order is as
 *  follows:
 *  
 *  - Descent 1 Shareware 1.0
 *  - Descent 1 Registered 1.0
 *  - Descent 1 Registered 1.4
 *  - Descent 2 Shareware 1.0
 */
int txtoffsets[desc_number];

/*! @brief Find a marker in a Devil INI file.
 *
 * Look for a marker in a Devil INI file and return the number associated with
 * that marker.  A marker looks like:
 * 
 *      <tt>:INITDATA 0</tt>
 *      
 * where the colon indicates the string is a marker.  The identifer follows.
 * An identifier can be any character except null and the newline character.
 * The number is placed after one space or tab.  This number will be converted
 * to an integer using the atoi function.  After the number, a newline
 * follows.  The marker plus the number may be up to 255 characters including
 * the newline.
 * 
 * 
 * 
 * @param[in]   f       f points to the opened INI file.
 * @param[in]   m       m is a string that contains the marker being looked
 *                      for.
 * @param[out]  number  number stores the value of the marker.
 * 
 * @retval  0   The marker was not found.
 * @retval  1   The marker was found.  
 */
int findmarker(FILE *f, const char *m, int *number) {
    char puffer[255],       /* Stores the line read from the INI file. */
         newmarker[200];    /* Stores the concatenated symbol and identifer
                               string. */

    
    /* Start from the beginning of the file to find the marker. */
    fseek(f, 0, SEEK_SET);      /* not the smartest solution */
    
    /* Build the marker string. */
    newmarker[0] = ':';
    strcpy(&newmarker[1], m);   
    newmarker[strlen(m) + 1] = 0;

    /* Loop through each line in the file looking for the marker.  If it isn't
       found by the end of the file, return 0. */
    do {
        if (fgets(puffer, 255, f) == NULL) {
            return 0;
        }
    } while ( strncmp( newmarker, puffer, strlen(newmarker) ) );

    /* Convert the string after the marker to an integer.  The +1 is for the
       whitespace character after the marker. */
    *number = atoi(&puffer[strlen(newmarker) + 1]);
    
    return 1;
}


/*! @brief Skips to the next line.
 *
 *  This function will skip to the next line by reading the rest of the INI
 *  file contents into a buffer.  The size of the line must not exceed 79
 *  characters or Devil will throw an error.  This can be used to insert
 *  comments into the INI file.
 *
 *  @param[in] f    FILE Pointer to Devil INI file.
 */
void skipline(FILE *f) {
    char puffer[255];


    if (fgets(puffer, 255, f) == NULL) {
        printf("Can't skip line.\n");
        exit(2);
    }

    if (strlen(puffer) > 80) {
        printf("Skipline: line too long\n");
        exit(2);
    }
}


/*! @brief Reads a point as defined in structs.h from a Devil INI file.
 *
 *  @param[in]  f   The Devil INI file.
 *  @param[out] p   A pointer to a point structure that will store the point
 *                  from the INI file.
 *
 *  The function will read three signed floating-point literals from a Devil
 *  INI file.  If the point could not be read, Devil will exit to the prompt.
 */
void readpoint(FILE *f, struct point *p) {
    if (fscanf(f, "%g%g%g", &p->x[0], &p->x[1], &p->x[2]) != 3) {
        printf("Can't read point.\n");
        exit(2);
    }
}


/*! @brief Read a character string from a Devil INI file.
 *  
 *  Devil INI file strings are formatted like so:
 *
 *  <tt>{string}</tt>
 *
 *  The string must be delinated with opening and closing braces.  A '}'
 *  character cannot be escaped and is excluded from the list of allowed
 *  characters since it closes a string.
 *
 *  The maximum length of a string is 255 characters.
 *
 *  @param[in]  f   FILE pointer to the Devil INI file.
 *  @param[out] s   Pointer to a character string.
 */
void readstring(FILE *f, char **s) {
    int i = 0;
    char puffer[255];

    /* Look for the start of the string.  If one isn't found, then exit the
     * program. */
    while (fgetc(f) != '{') {
        if ( feof(f) ) {
            printf("Can't find start of string.\n");
            exit(2);
        }
    }

    
    /* Read the string into puffer until a '}' character is reached, 255
     * characters are read, or the end of the file is reached. */
    do {
        puffer[i++] = fgetc(f);
    } while ( puffer[i - 1] != '}' && i < 255 && !feof(f) );

    puffer[i - 1] = 0;

    /* If the string exceeded the 255 character length or the end of the file
     * was reached, then exit the program with an error. */
    if (feof(f) || i == 255) {
        printf("Can't read string.\n");
        exit(2);
    }

    if ( ( *s = MALLOC(strlen(puffer) + 1) ) == NULL ) {
        printf("No mem for string.\n");
        exit(2);
    }

    strcpy(*s, puffer);
}


/*! @brief Read an objtype struct from a Devil INI file. 
 *
 *  @param[in]   f      Devil INI file pointer.
 *  @param[out]  ot     Struct that holds an objtype.
 *
 *  @retval      num    Returns the number of infotypes under this objtype.
 *
 *  The objtype is formatted like so:
 *
 *  <tt>0x00 0 {string}</tt>
 *
 *  The first value is an unsigned integer in hexadecimal.  The next is a
 *  signed integer.  The last is a string with a description of the object.
 *  The string follows the Devil INI string format (see readstring()).
 */
int readobjtype(FILE *f, struct objtype *ot) {
    int num;

    /* Read the object ID(?) and the number of child infotypes. */
    if (fscanf(f, "%x%d", (unsigned int *)&ot->no, &num) != 2) {
        printf("Can't read objtype.\n");
        exit(2);
    }


    /* Read the description string. */
    readstring(f, &ot->str);

    if (init_test & 1) {
        fprintf(errf, "Read objtype: %x %d {%s}\n", ot->no, num, ot->str);
    }

    return num;
}

/*! @brief Read a w_keycode struct and its text from a Devil CFG file.
 *
 *  @param[in] f The filehandle for the INI file.
 *  @param[out] kc A w_keycode struct that will store the info read from the
 *                 CFG file.
 *  @param[out] s A pointer to a character string that will store the text
 *                that follows the keycode.
 *
 *
 *  The keycode is formatted like so:
 *
 *      0 27 0 {quit}
 *
 *  The first digit is an int that indicates the keyboard modifier key status.
 *  The second digit is the key hit.  The third is the event keycode.  After
 *  that follows a Devil INI string.
 *
 */
void readkeycode(FILE *f, struct w_keycode *kc, char **s) {
    /* Read the modifier key status, the key, and the corresponding event
       code. */
    if (fscanf(f, "%i%i%i", &kc->kbstat, &kc->key, &kc->event) != 3) {
        printf("Can't read keycode.\n");
        exit(2);
    }

    /* Read in the string describing the keycode that follows. */
    readstring(f, s);
}


/*! @brief Reads an @ref infoitem struct from a Devil INI file.
 *
 *  @param[in]  f The Devil INI file being read.
 *  @param[out] i The infoitem struct that will store the information read
 *                from the INI file.
 *
 *  @retval     p An int containing the number of child infoitem types under
 *                the current infoitem type.
 *
 *  {desc} type data offset length multifunc num_children se_func
 *
 *  - {desc}        A Devil INI string that describes this infoitem.  See @ref
 *                  readstring.
 *  - type          Encoded type of infoitem.  See @ref infotype.
 *  - data          Encoded data type?  Something to do with @ref datastructs.
 *  - offset        Probably tells Devil where to modify a value in a struct.
 *  - length        Probably tells Devil the length of the data to be
 *                  modified.
 *  - multifunc     ???
 *  - num_children  The number of child objtypes under this infotype.
 *  - se_func       The encoded side effect function to be called.  If no 
 *                  function is to be called, the set this to -1.  If this
 *                  value can't be changed by the user, set to -2?  Is this
 *                  for specific Descent data?
 */
int readinfotype(FILE *f, struct infoitem *i) {
    int p, inr, tnr;


    readstring(f, &i->txt);

    if (strlen(i->txt) == 0) {
        FREE(i->txt);
        i->txt = NULL;
    }

    if (fscanf(f, "%d%d%d%d%d%d%d", &tnr, &inr, &i->offset, &i->length,
               &i->multifuncnr, &p, &i->sidefuncnr) != 7) {
        printf("Can't read info item header.\n");
        exit(2);
    }

    i->infonr = inr;
    i->type = tnr;
    i->numchildren = (tnr == it_selbutton) ? i->multifuncnr : 0;
    return p;
}


/*! @brief A variadic function that reads the values below a marker in a Devil
 *         INI file. 
 *
 *  @param[in]  f   Devil INI file.
 *  @param[in]  t   The string that describes each data type.
 *  @param[out] ... Pointers to variables that will store the values read from
 *                  the INI file.
 *
 *  This function reads values from an INI file.  The types of data Devil
 *  reads from an INI file is hard-coded into the application.  The types
 *  are like so:
 *
 *  @c g    - A C floating point literal.
 *
 *  @c p    - A point structure as defined by structs.h.
 *
 *  @c U    - A C unsigned long integer literal in decimal.
 *
 *  @c X    - A C unsigned long integer literal in hexadecimal.
 *
 *  @c d    - A C signed integer literal.
 *
 *  @c x    - A C unsigned integer literal in hexadecimal.
 *
 *  @c s    - A Devil INI file string.  See @ref readstring.
 *
 *  @c o    - An @ref objtype struct.  See @ref readobjtype.
 *
 *  @c i    - An @ref infoitem struct.  See @ref readinfotype.
 *
 *  @c k    - A w_keycode struct and its text from a Devil CFG file.
 *
 *
 *  @retval ret The number of children under this parameter.  Only valid for
 *              i or o tokens.  This value is only valid for the last call to
 *              readinfotype or readobjtype.
 */
int iniread(FILE *f, const char *t, ...) {
    int i, 
        n,          /* Stores the number of values to read in. */
        ret = 0;
    struct w_keycode *wkey;
    va_list alist;


    va_start(alist, t);
    n = strlen(t);

    /* Read the values from the Devil INI file into the list of variables
     * passed into the function according to the string of conversion specs
     * passed. */
    for (i = 0; i < n; i++) {
        switch (t[i]) {
            case 'g':   /* Read a float. */

                if (fscanf( f, "%g", va_arg(alist, float *) ) != 1) {
                    printf("Can't read float.\n");
                    exit(2);
                }

                break;

            case 'p':   /* Read a point.  See structs.h. */
                readpoint( f, va_arg(alist, struct point *) );
                break;

            case 'U':   /* Read an unsigned long integer. */

                if (fscanf( f, "%lu",
                           va_arg(alist, unsigned long *) ) != 1) {
                    printf("Can't read unsigned long.\n");
                    exit(2);
                }

                break;

            case 'X':   /* Read a long unsigned hexadecimal integer. */

                if (fscanf( f, "%lx",
                           va_arg(alist, unsigned long *) ) != 1) {
                    printf("Can't read unsigned long hex.\n");
                    exit(2);
                }

                break;

            case 'd':   /* Read an integer. */

                if (fscanf( f, "%d", va_arg(alist, int *) ) != 1) {
                    printf("Can't read int.\n");
                    exit(2);
                }

                break;

            case 'x':   /* Read an unsigned hex integer. */

                if (fscanf( f, "%x", (unsigned int *)va_arg(alist, int *) ) != 1) {
                    printf("Can't read int.\n");
                    exit(2);
                }

                break;

            case 's':   /* Read a brace-delineated string. */
                readstring( f, va_arg(alist, char **) );
                break;

            case 'o':   /* Read data for an objtype struct */
                ret = readobjtype( f, va_arg(alist, struct objtype *) );
                break;

            case 'i':   /* Read an @ref infoitem struct into memory. */
                ret = readinfotype( f, va_arg(alist, struct infoitem *) );
                break;

            case 'k':   /* Read in a w_keycode struct and its text. */
                wkey = va_arg(alist, struct w_keycode *);
                readkeycode( f, wkey, va_arg(alist, char **) );
                break;

            default:    /* The wrong character was passed into iniread(). */
                printf("Unknown type %c\n", t[i]);
                exit(2);
        }

        /* Move to the next line. */
        skipline(f);
    }

    va_end(alist);
    
    return ret;
}

/*! @brief Concatenates the base configuation path with a string.
 *
 *  @param[in,out] s A pointer to a string that contains the INI filename.
 *                   s must be free'd after it is used.
 *
 *  The directory stored in @c init.cfgpath is concatenated with a
 *  forward-slash and the string pointed to by s.  The string pointed to by
 *  s then becomes the relative path to the INI file.
 *
 *  For example, if a pointer to the string @c "try.ini" is passed into the
 *  function, and @c "conf" is stored in @c init.cfgpath, then the pointer
 *  passed into the function will point to a new string @c "conf/try.ini". 
 */
void addcfgpath(char **s) {
    char *r;


    checkmem( r = MALLOC(strlen(*s) + strlen(init.cfgpath) + 2) );
    strcpy(r, init.cfgpath);
    strcat(r, "/");
    strcat(r, *s);
    FREE(*s);
    *s = r;
}

/*! @brief Reads 256 color palettes from a Devil palette file.
 *
 *  @param[in] f Pointer to Devil palette.  The pointer must point to the
 *               first byte of the file for the function to work correctly.
 *
 *  The Devil palette file is a collection of the seven Descent and
 *  Descent 2 palettes with additional metadata.  The first byte of the file
 *  indicates how many palettes are stored in the file.  Then the next eight
 *  bytes are the seven character palette name and the null terminator.  The
 *  256 RGB values are then read in and left shifted twice to get the RGB 
 *  values.
 *
 *  The lighttables can be accessed from view.lightcolors.
 *
 *  This function also implements security lighttables for rounding errors
 *  that occur when plotting a texture.  These are not understood yet.
 *
 *  @retval 0 Function failed to load the palette file.
 *  @retval 1 Function succeeded in loading the palette file.
 */
int readpalettes(FILE *f) {
    int i, j;
    /* Stores the number of palettes in the Devil palette file. */
    unsigned char num_pal;  

    /* A char8 at offset 0x0 marks the number of palettes that are stored in
       the file.  If this number doesn't match NUM_PALETTES then Devil will
       stop loading the INI file. */
    if (fread(&num_pal, 1, 1, f) != 1 || NUM_PALETTES != num_pal) {
        return 0;
    }

    for (i = 0; i < NUM_PALETTES; i++) {
        /* The palette's name is stored in seven bytes plus a null 
           terminator.  If the name is shorter than seven bytes, then it is
           padded with null bytes. */
        if (fread(palettes[i].name, 1, 8, f) != 8) {
            return 0;
        }

        /* Don't trust that the palette file terminates the name correctly. */
        palettes[i].name[8] = 0;

        /* Read in the RGB values from the palette. */
        if (fread(palettes[i].palette, 3, 256, f) != 256) {
            return 0;
        }

        /* The values are right shifted in the palette files, so left shift
           them here. */
        for (j = 0; j < 3 * 255; j++) {
            palettes[i].palette[j] = palettes[i].palette[j] << 2;
        }

        /* Load the light tables from the palette. */
        /* num_security is the number of extra lighttables at the end and the
           beginning of the table, to handle the rounding errors which occur
           during the plotting of a texture */
        checkmem(
            palettes[i].mem_lighttables = malloc(
                (NUM_SECURITY * 2 + NUM_LIGHTCOLORS + 1) * 256
            )
        );
        
        palettes[i].lighttables = (unsigned char *)
                                  ( (unsigned long)palettes[i].
                                   mem_lighttables /* & 0xffffff00 */ );
        /* 0xffffff00 is magic.  I have no idea why it is here. */

        /* Read the lighttables into memory.  The lighttables are copied into
           the array at the point where the security lighttables end. */
        if (fread(&palettes[i].lighttables[256 * NUM_SECURITY], 256,
                  NUM_LIGHTCOLORS, f) != NUM_LIGHTCOLORS) {
            return 0;
        }

        /* this is to get proper light values even if we have some trouble
           with rounding */
           
        /* Setup the security lighttables. */
        for (j = 0; j < NUM_SECURITY; j++) {
            /* Copy the first real lighttable into the first three security
               lighttables. */
            memcpy(&palettes[i].lighttables[256 * j],
                   &palettes[i].lighttables[NUM_SECURITY * 256], 256);
            
            /* Copy the last real lighttable into the last three security 
               tables. */
            memcpy(&palettes[i].lighttables[256 *
                                            (NUM_LIGHTCOLORS + NUM_SECURITY +
                                             j)],
                   &palettes[i].lighttables[(NUM_SECURITY + NUM_LIGHTCOLORS -
                                             1) * 256], 256);
        }
    }

    /* Set the lighttables pointer in the viewdata struct to point to the
       first real lighttable. */
    view.lightcolors = &palettes[0].lighttables[NUM_SECURITY * 256];
    return 1;
}


/*!
 *  @brief Reads the information in config/devil.ini into a viewdata struct.
 *
 *  @param[in] fn A string that contains the path to the devil.ini file.
 *  @param[in] c  Stores the titlescreen flag (does nothing in this function).
 *
 *  Reads the values in config/devil.ini and sets up the GUI and editor.
 *
 */
void initeditor(const char *fn, int c) {
    int i, n;
    FILE *f;
    char *palname;  /* Name of the Devil palette file. */

    /* config/devil.ini must exist, otherwise Devil won't know which files to
       load. */
    if ( ( f = fopen(fn, "r+") ) == NULL ) {
        printf(TXT_CANTOPENINI TXT_UNZIPWITHD, fn);
        exit(2);
    }

    /* Let the user know Devil is parsing devil.ini. */
    printf(TXT_READINI);
    
    /* If :INITDATA is not present in devil.ini, then fail with an assertion
       error. */
    my_assert( findmarker(f, "INITDATA", &init_test) );
    
    /* Read devil.ini into the view, txtoffsets, init, and pig globals.  Also
       read the palette filename into memory. */
    iniread(
        f,
        "dddpppdgggggggggdggggggggggggggggdddddddddddddddddsssssssssssssss",
        &view.movemode, &view.currmode, &view.whichdisplay, &view.e0,
        &view.e[0],
        &view.e[1], &view.doubleclick, &view.distcenter, &view.dist,
        &view.distscala,
        &view.movefactor, &view.pmovefactor, &view.movescala,
        &view.rotangle,
        &view.protangle, &view.rotscala, &view.bm_movefactor,
        &view.flatsideangle, &view.bm_stretchfactor, &view.maxvisibility,
        &view.visscala, &view.tsize, &view.dsize, &view.minclicksodist,
        &view.mincorner, &view.minweirdwall, &view.maxconndist,
        &view.maxuserconnect,
        &view.gridlength, &view.gridscala, &view.mapangle,
        &view.illum_quarterway,
        &view.illum_brightness, &n, &view.gridonoff, &view.coord_axis,
        &view.clickradius, &view.displayed_group,
        &view.askone, &view.asktagged, &view.warn_convex,
        &view.warn_doublekeys,
        &view.warn_tworeactors, &view.warn_thingoutofbounds,
        &view.warn_gridrot,
        
        &txtoffsets[0], &txtoffsets[1], &txtoffsets[2], &txtoffsets[3],
        
        &init.waittime, &init.macropath, &init.levelpath, &init.pogpath,
        &init.txtlistpath, &init.cfgpath, &init.fontname,
        
        &palname,
        
        &init.cfgname, &init.lightname, &init.convtablename,
        &init.lastname,
        &init.batchfilename, &init.menuname,
        
        &pig.bulbname,
        &pig.brokenbulbname
    );
    
    view.illum_minvalue = n;
    view.render = 0;
    view.gamma_corr = 7 << 9;
    view.blinkinglightson = 0;
    view.timescale = 1.0;
    view.warn_frameratetoosmall = 1;
    view.warn_illuminate = 1;
    
    /* Prepend the config directory to this list of filenames read from
       devil.ini. */
    addcfgpath(&init.cfgname);
    addcfgpath(&init.lastname);
    addcfgpath(&init.menuname);
    addcfgpath(&init.fontname);
    addcfgpath(&init.batchfilename);
    addcfgpath(&init.lightname);
    addcfgpath(&init.convtablename);
    addcfgpath(&pig.bulbname);
    addcfgpath(&pig.brokenbulbname);
    init.xres = 640;
    init.yres = 480;
    view.drawwhat = DW_DEFAULT;

    if (init_test & 1) {
        printf("Init output on: %d.\n", init_test);
    }

    normalize(&view.e[0]);
    normalize(&view.e[1]);
    VECTOR(&view.e[2], &view.e[1], &view.e[0]); /* left-handed system */
    
    /* Find the number of Cube, Side, etc. windows to open. */
    my_assert( findmarker(f, "BUTTONS", &init.numbuttons) );

    if (init.numbuttons != in_number) {
        printf("Wrong number of buttons: %d.\n", init.numbuttons);
        exit(2);
    }

    /* Read the names of the editing windows from devil.ini. */
    for (i = 0; i < init.numbuttons; i++) {
        iniread(f, "s", &init.bnames[i]);
    }

    /* Close devil.ini */
    fclose(f);

    /* Open the Devil palette file. */
    addcfgpath(&palname);
    if ( ( f = fopen(palname, "rb") ) == NULL ) {
        printf("Can't open palette-file: %s.\n", palname);
        exit(2);
    }
    FREE(palname);

    /* Read the Descent and Descent 2 palettes stored in the Devil palette
       file into memory. */
    if ( !readpalettes(f) ) {
        printf("Can't read palette file.\n");
        exit(2);
    }
    fclose(f);
    
    /* Start a linked list of the levels currently open? */
    initlist(&view.levels);
    view.txt_window = NULL;
    view.defwall = 0;
    view.pdefcube = NULL;
    view.pdeflevel = NULL;
    view.pcurrmacro = NULL;
    pig.current_pigname = NULL;
    pig.default_pigname = NULL;
    pig.pig_txts = NULL;
    pig.num_pigtxts = 0;
    
    /* Flag for drawing the light bulb on the chosen texture in the side
       window that indicates a texture produces light. */
    view.littlebulbson = 1;
}


