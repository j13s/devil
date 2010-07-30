/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    Wins: A Grfx-Windows system for DOS.
    w_test.c - This is just a test if wins works as it should
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "wins.h"
#include "w_init.h"
#include "w_event.h"

void dec_quit(int ec) {
    exit(1);
}

enum evcodes {ec_quit, ec_num_of_codes};

void(*do_event[ec_num_of_codes]) (int ec) = {dec_quit};

extern FILE *errf;

void my_exit() {
    exit(2);
}

int main() {
    char test_string[] = "1234567890";
    struct w_window *w1, *w2, iw1 = {
        0, 100, 100, 200, 200, 200, 200, 0, "BLA", 0,
        "Dies ist ein Helptext der automatisch formatiert wird"
    };
    const char *opt[5] = {
        "Switch", "Only texture", "Shoot through", "Normal", "Door"
    };
    struct w_b_choose b_choose = {
        0, 5, opt, 0, NULL, NULL
    };
    struct w_b_string b_string = {
        0, 10, 0, 0, test_string, isdigit, NULL, NULL, NULL, NULL
    };
    FILE *mf;

    do_event[ec_quit] = dec_quit;

    if ( !w_initwins(640, 480, 256, "wins.fnt") ) {
        printf("Can't initialize Wins.\n");
        exit(5);
    }

    if ( ( mf = fopen("wins.ini", "r") ) == NULL ) {
        fprintf(errf, "No ini-file.\n");
        exit(5);
    }
    
    if ( !w_initmenu(mf, do_event, ec_num_of_codes) ) {
        fprintf(errf, "Can't init menu.\n");
        exit(5);
    }

    if ( ( w1 = w_openwindow(&iw1) ) == NULL ) {
        fprintf(errf, "Can't open window 1\n");
        exit(5);
    }

    if (w_addstdbutton(w1, w_b_choose, 10, 10, -1, -1, "Test", &b_choose,
                       1) == NULL) {
        fprintf(errf, "Can't open button 1\n");
        exit(5);
    }

    iw1.xpos = 150;
    iw1.ypos = 150;

    if ( ( w2 = w_openwindow(&iw1) ) == NULL ) {
        fprintf(errf, "Can't open window 2\n");
        exit(5);
    }

    if (w_addstdbutton(w2, w_b_string, 10, 10, -1, -1, "Test2", &b_string,
                       1) == NULL) {
        fprintf(errf, "Can't open button 2\n");
        exit(5);
    }

    struct w_keycode ec_keycodes = {0, 0, 0, 0};
        
    w_handleuser(0, NULL, 0, NULL, 0, &ec_keycodes,
                 do_event);

    w_closewins();
    
    return 0;
}


