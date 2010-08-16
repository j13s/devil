#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "userio.h"
#include "lac_cfg.h"

int isF2SavingAll = 0;
int isAlwaysSmoothing = 0;
int theMinDeltaLight = 0;
int theMaxLight = 65535;
int changeCubeEnabled = 1;


/*! @brief Processes the value from a DevilX INI file parameter.
 *
 *  @param[in] s The parameter from the DevilX INI file.
 *
 *  @retval p       A character pointer that points to the first character of
 *                  the value in s.
 *  @retval NULL    If no equal sign is found, the function returns NULL.
 *
 *  The function will read a parameter string and return a pointer to the
 *  first character of the value.
 *
 */
char* lac_find_value(char* s) {
    char* p = NULL;

    
    /* Point to the equal sign, then point to the first character after it. */
    p = strstr(s, "=");
    p += strspn(p, "= ");

    if (*p == 0) {
        return NULL;
    }

    return p;
}


/*! @brief Reads the DevilX INI file and processes the results.
 *
 *  The INI file can only be from config/devilx.ini.  The DevilX INI parameter
 *  has the format:
 *
 *      CamelCaseValue=value
 *
 *  The name and value can be anything, as long as the parameter is less than
 *  79 characters. 
 *
 */
void lac_read_cfg(void) {
    FILE* cfgfile = fopen("config/devilx.ini", "rt");


    fprintf(stderr, "Reading DevilX.ini ...\n");

    if (cfgfile) {
        char s[80];
        char* p;


        /* Read each line in and store the values for each name. */
        while ( !feof(cfgfile) ) {
            fgets(s, sizeof(s), cfgfile);
            s[79] = 0;
            
            /* Convert each name to lowercase. */
            for (int i = 0; i < 80; i++) {
                if (s[i] == 0) {
                    break;
                }
                
                s[i] = (char)tolower((int)s[i]);
            }


            if ( ( p = strstr(s, "alwayssmooth") ) ) {
                if ( ( p = lac_find_value(p) ) ) {
                    sscanf(p, "%d", &isAlwaysSmoothing);

                    if (isAlwaysSmoothing != 0) {
                        isAlwaysSmoothing = 1;
                    }
                    else {
                        isAlwaysSmoothing = 0;
                    }

                    fprintf(stderr, "AlwaysSmoothing = %d\n",
                            isAlwaysSmoothing);
                }

            }
            else if ( ( p = strstr(s, "mindeltalight") ) ) {
                if ( ( p = lac_find_value(p) ) ) {
                    float f;
                    sscanf(p, "%f", &f);
                    f = f * 4.0 * 63.0 / 200.0;

                    if ( (0 <= f) && (f <= 63) ) {
                        theMinDeltaLight = f;
                    }

                    fprintf(stderr, "MinDeltaLight = %.1f\n",
                            (float)theMinDeltaLight * 200 / 63 / 4);
                }
            }
            else if ( ( p = strstr(s, "maxlightvalue") ) ) {
                if ( ( p = lac_find_value(p) ) ) {
                    float f;
                    sscanf(p, "%f", &f);
                    f = 65535 * f / 200.0;

                    if ( (0 <= f) && (f <= 65535) ) {
                        theMaxLight = f;
                    }

                    fprintf(stderr, "MaxLightValue = %.1f\n",
                            (float)theMaxLight * 200 / 65535.0);
                }

            }
            else if ( ( p = strstr(s, "f2fullsave") ) ) {
                if ( ( p = lac_find_value(p) ) ) {
                    sscanf(p, "%d", &isF2SavingAll);

                    if (isF2SavingAll != 0) {
                        isF2SavingAll = 1;
                    }
                    else {
                        isF2SavingAll = 0;
                    }

                    fprintf(stderr, "isF2SavingAll = %d\n", isF2SavingAll);
                }

            }
            else if ( ( p = strstr(s, "changecube") ) ) {
                if ( ( p = lac_find_value(p) ) ) {
                    sscanf(p, "%d", &changeCubeEnabled);

                    if (changeCubeEnabled != 0) {
                        changeCubeEnabled = 1;
                    }
                    else {
                        changeCubeEnabled = 0;
                    }

                    fprintf(stderr, "changeCubeEnabled = %d\n",
                            changeCubeEnabled);
                }

            }
        }
    }
    else {
        fprintf(stderr,
                "Error\nCould not open config/devilx.ini\nPress enter\n");
        getchar();
    }
}


