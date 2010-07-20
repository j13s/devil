#include <stdio.h>
#include <string.h>


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
    char* ext_p = strrchr(filename, '.') + 1;
    
    strcpy(ext, ext_p);
}

/*
    Pass the full path and a character array and the full path will be copied
    into that character array
*/

void
linux_find_path(char* filename, char* path) {
    char* path_p = strrchr(filename, '/');
    
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
    /* Basename starts at the first character after the last forward-slash */
    char* basename_p = strrchr(filename, '/') + 1;
    
    /* Find the length of the basename */
    int basename_length = strrchr(filename, '.') - basename_p;
    
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
         
    /* Find the path and extension so we can paste them together */
    linux_find_path(old_filename, path);
    linux_find_ext(old_filename, ext);
    
    /* path + basename + dot + extension */
    strcpy(new_filename, path);
    strcat(new_filename, basename);
    strcat(new_filename, ".");
    strcat(new_filename, ext);
}
    
    
    