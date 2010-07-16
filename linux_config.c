/*
    Pass the full path and return a char* to the period of the file extension.
    
    /tmp/test.txt
        ^
*/

#include <stdio.h>
#include <string.h>

char *
linux_find_ext_p(char* filename) {
    /* Loop backwards until a period is found */
    FIND_EXT:
    for (int i = strlen(filename); i >= 0; i--) {
        if (filename[i] == '.') {
            return (char *)(filename + i);
        }
    }
}

/*
    Pass the full path and return a char* to the last forward-slash of the 
    file's path.
    
   /tmp/test.txt
       ^
*/

char *
linux_find_path_p(char* filename) {
    /* Loop backwards until a forward-slash is found */
    FIND_PATH:
    for (int i = strlen(filename); i >= 0; i--) {
        if (filename[i] == '/') {
            /*
               Point to the last character of the path.  This will be the last
               forward-slash
            */
            return (char *)(filename + i);
        }
    }
}

/*
    Pass the full path and return a char* to the first character of the
    filename.
    
    /tmp/test.txt
         ^
*/

char*
linux_find_basename_p(char* filename) {
    return linux_find_path_p(filename) + 1;
}

/*
    Pass the full path and a character array and copy the file extension into
    that character array.
*/

void
linux_find_ext(char* filename, char* ext) {
    /*
        The pointer for the extension points to the the period and the
        extension then follows.
    */
    char* ext_p = linux_find_ext_p(filename) + 1;
    
    strcpy(ext, ext_p);
}

/*
    Pass the full path and a character array and the full path will be copied
    into that character array
*/

void
linux_find_path(char* filename, char* path) {
    char* path_p = linux_find_path_p(filename);
    
    /*
        Find the length of the path. The +1 makes it point to the last 
        forward-slash
    */
    int path_length = path_p - filename + 1;
    
    /* Copy the path into the passed character array */
    strncpy(path, filename, path_length);
}

/*
    Pass the full path and a character array and the basename will be copied
    into that array
*/

void
linux_find_basename(char* filename, char* basename) {
    char* basename_p = linux_find_basename_p(filename);
    
    /* Find the length of the basename */
    int basename_length = linux_find_ext_p(filename) - basename_p;
    
    strncpy(basename, basename_p, basename_length);
}

/*
    Change the extension of a file.
    
    Pass a character array that will hold the new filename, the full path in
    another character array, and a string literal with the desired extension
*/

void
linux_change_ext(char* new_filename, char* old_filename, const char* ext) {
    char path[FILENAME_MAX],
         base[FILENAME_MAX];
          
    linux_find_path(old_filename, path);
    linux_find_basename(old_filename, base);
    
    strcpy(new_filename, path);
    strcat(new_filename, base);
    strcat(new_filename, ext);
}

/*
    Change the basename of a file.
    
    This is like linux_change_ext(), except the third argument will be the
    basename.
*/

void
linux_change_basename(char* new_filename, char* old_filename, const char* basename) {
    char path[FILENAME_MAX],
         ext[FILENAME_MAX];
         
    linux_find_path(old_filename, path);
    linux_find_ext(old_filename, ext);
    
    strcpy(new_filename, path);
    strcat(new_filename, basename);
    strcat(new_filename, ".");
    strcat(new_filename, ext);
}
    
    
    