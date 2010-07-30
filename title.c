/*  DEVIL - Descent Editor for Vertices, Items and Levels at all
    title.c - show title screen
    Copyright (C) 1995  Achim Stremplat (ubdb@rz.uni-karlsruhe.de)
     (further authors see below)

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
#include <grx20.h>
#include <gif_lib.h>


/* Read a GIF file and return a GrContext with the data.  Written to display
   the titlex.gif that comes with Devil 2.2lc2.
   
   Returns NULL when an error occurs. */
GrContext *readgif(const char *fname) {
    /* Hold the information needed from the GIF file, such as width, height,
       palette, and the bitmap data */
    GifFileType *gif;
    
    /* If an error occurred opening the GIF, explain the error and have giflib's
       error handler tell the user what went wrong.  Then return NULL*/
    if ((gif = DGifOpenFileName(fname)) == NULL) {
        fprintf(errf, "Could not open %s.  Does the file exist?", fname);
        PrintGifError();
        
        return NULL;
    }
    
    /* If something went wrong trying to read data from the GIF file, then let
       the user know and exit. */
    if (DGifSlurp(gif) == GIF_ERROR) {
        fprintf(errf, "There was a problem loading the data from %s\n", fname);
        fprintf(errf, "Is the file intact?\n");
        
        return NULL;
    }
    
    /* Stores the 256 color palette from the GIF.  Each pixel has a value that
       ranges from 0-255, and this is the lookup table for the RGB values. */
    GifColorType *palette;
    
    /* If the user tried to replace titlex.gif with something else, let them
       know that Devil didn't like that. */
    if ((palette = gif->SColorMap[0].Colors) == NULL) {
        fprintf(errf, "%s is not a 256 color GIF.\n", fname);
        return NULL;
    }
    
    /* The 8-bit values that make up the bitmap. */
    unsigned char *rgb = gif->SavedImages[0].RasterBits;
    
    /* The drawing space for the splashscreen. */
    GrContext *splashscreen;
    
    if ((splashscreen = GrCreateContext(gif->SWidth, gif->SHeight, NULL, NULL))
        == NULL) {
        fprintf(errf, "Error allocating memory for the splashscreen.\n");
        return NULL;
    }
    
    /* Transfer the data in the GIF to the newly created context */
    GrSetContext(splashscreen);    
    
    /* Iterate over each row of the GIF in memory and place the color values
       in the corresponding row of the context */
    for (int row = 0; row < gif->SHeight; row++) {
        /* Stores the RGB values for the context */
        GrColor *grpixels;
        
        checkmem(grpixels = malloc(sizeof(GrColor) * gif->SWidth));
        
        /* Grab a pixel from the current GIF file's row and translate it to
           GRX's color encoding */
        for (int col = 0; col < gif->SWidth; col++) {
            /* Since the bitmap is stored linearly, perform the calculation to
               find the pixel at row x column.  The offset is then used to
               grab the lookup value from the ColorMap */
            int index = rgb[row * gif->SWidth + col];    
            
            /* Use the calulated index to get the RGB values and create the
               GrColor. */
            GifByteType red   = palette[index].Red;
            GifByteType green = palette[index].Green;
            GifByteType blue  = palette[index].Blue;
            
            grpixels[col] = GrAllocColor(red, green, blue);
        }
        
        /* Put the row of created GrColors onto the current context. Loop over
           each row to build the whole image. */
        GrPutScanline(0, gif->SWidth - 1, row, grpixels, GrWRITE);
        
        free(grpixels);
    }
    
    /* Let the user know something went wrong closing the file. */
    if(DGifCloseFile(gif) == GIF_ERROR) {
        fprintf(errf, "Something went wrong closing %s\n", fname);
    }
    
    return splashscreen;
}

/* Draws the titlex.gif as a splashscreen.  Returns 1 on success, 0 on
   failure.  Will happily chug along loading Devil if there is a problem
   loading the image. */
int titlescreen(void) {
    /* Holds the data for the titlescreen. */
    GrContext *gifpic;

    /* If there an an error loading the titlescreen, let the user know. */
    if ( (gifpic = readgif("titlex.gif")) == NULL ) {
        fprintf(
            errf, "Devil encountered an error loading the title screen.\n");
        return 0;
    }

    /* Draw the titlescreen. */
    GrSetContext(NULL);
    GrBitBlt(NULL, 0, 0, gifpic, 0, 0, 639, 479, GrWRITE);
    
    /* Free memory allocated for the titlescreen. */
    GrDestroyContext(gifpic);
    
    return 1;
}


